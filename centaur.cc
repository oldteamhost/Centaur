/*
 * CENTAUR
 * by oldteam & lomaster
 * license GPL-3.0
 *   Сделано от души 2023.
*/

#include <cstdio>
#include <sstream>
#include <cstdlib>
#include <mutex>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <vector>
#include <iomanip>

#include "network/include/icmp4.h"
#include "network/include/connect.h"
#include "network/include/socks5.h"

#include "base/include/nescathread.h"
#include "include/log.h"
#include "include/send.h"
#include "base/include/getopt.h"
#include "include/status.h"
#include "include/other.h"
#include "include/pinger.h"
#include "base/include/file.h"

const struct option longopts[] =
{
  {"help", no_argument, 0, 'h'},
  {"version", no_argument, 0, 'v'},
  {"message", required_argument, 0, 'm'},
  {"port", required_argument, 0, 'p'},
  {"ping-threads", required_argument, 0, 1},
  {"ping-delay", required_argument, 0, 2},
  {"icmp-timeout", required_argument, 0, 3},
  {"tcp-timeout", required_argument, 0, 4},
  {"ping-port", required_argument, 0, 5},
  {"ttl", required_argument, 0, 6},
  {"non-stop", no_argument, 0, 7},
  {"myself", required_argument, 0, 8}
};
const char* shortopts = "hvm:p:l:";

struct options_centaur
{
  std::unordered_map<std::string, double> rtts;
  int ping_threads;
  pinger_options po;
  send_options so;
};

struct centaur_data_md5
{
  std::vector<std::string> temp_file;
  std::string combined_values; /* 192.168.1.1;192.168.1.2 */
  std::string key; /* combined_values in encode md5 */
  std::unordered_map<std::string, std::vector<std::string>> md5_hash_map;
};

#define VERSION "0.4"
// #define __VERBOSE

std::vector<std::string> split_string_string(const std::string& str, char delimiter);
std::vector<char> const_char_ptr_to_vector(const char* data);
std::vector<char> std_string_to_vector(const std::string& data);
std::vector<char> char_ptr_to_vector(char* data);
template <std::size_t N> std::vector<char> char_array_to_vector(const char (&data)[N]);

void encode_md5_data(centaur_data_md5 *cd5);
void thread_pinger(const char* host, int type, struct pinger_options *po);

centaur_data_md5 cd5;
options_centaur oc;
socks_5_connection connection;
std::mutex lol;

void default_init(struct options_centaur *oc)
{
  oc->ping_threads    = 200;
  oc->so.non_stop     = false;
  oc->so.send_port    = 80;
  oc->so.myself       = "anonymous";
  oc->po.delay_ping   = 0;
  oc->po.delay_ping   = 0;
  oc->po.icmp_timeout = 1000;
  oc->po.icmp_ttl     = 121;
  oc->po.tcp_timeout  = 2000;
  oc->po.tcp_port     = 80;
  oc->so.send_port    = 80;
  oc->so.bytes        = std_string_to_vector("Hello Internet!");
}

int main(int argc, char** argv)
{
  if (argc <= 1){usage(argv[0]);}
  default_init(&oc);
  int rez, option_index = 0;

  while ((rez = getopt_long_only(argc, argv, shortopts, longopts, &option_index)) != EOF) {
    switch (rez) {
      case 'h':
        usage(argv[0]);
        break;
      case 'v':
        version(VERSION);
        break;
      case 'p':
        oc.so.send_port = atoi(optarg);
        break;
      case 'm':
        oc.so.bytes = const_char_ptr_to_vector(optarg);
        break;
      case 1:
        oc.ping_threads = atoi(optarg);
        break;
      case 2:
        oc.po.delay_ping = atoi(optarg);
        break;
      case 3:
        oc.po.icmp_timeout = atoi(optarg);
        break;
      case 4:
        oc.po.tcp_timeout = atoi(optarg);
        break;
      case 5:
        oc.po.tcp_port = atoi(optarg);
        break;
      case 6:
        oc.po.icmp_ttl = atoi(optarg);
        break;
      case 7:
        oc.so.non_stop = true;
        break;
      case 8:
        oc.so.myself = optarg;
        break;
    }
  }

  std::vector<std::string> files;
  running(VERSION);
  sudorun();

  if (optind < argc) {
    files = split_string_string(argv[optind], ',');
  }
  for (const auto& file : files) {
    std::string file_temp = file;
    std::vector<std::string> temp = read_file(file_temp);
    cd5.temp_file = temp;
  }
  encode_md5_data(&cd5);
  double ping = icmp_ping(get_ip_by_dns("google.com"), oc.po.icmp_timeout, 8, 0, 1, oc.po.icmp_ttl);

  printf("> local: ip=%s, ping=%0.2fms, files=%lu, hosts=%lu\n", get_local_ip(), ping, files.size(), cd5.temp_file.size());
  printf("> icmp: type=8, code=0, ttl=%d, timeout=%d, delay=%d\n\n", oc.po.icmp_ttl, oc.po.icmp_timeout, oc.po.delay_ping);

  struct timespec start_time;
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  printf("* Ping scan at %lu hosts, threads=%d, time=%s\n", cd5.md5_hash_map[cd5.key].size(), oc.ping_threads, get_time());

  int i = 0;
  std::vector<std::future<void>> futures_ping;
  thread_pool ping_pool(100);

  for (int i = 0; i < cd5.md5_hash_map[cd5.key].size(); i++) {
      const char* host = cd5.md5_hash_map[cd5.key][i].c_str();
      futures_ping.emplace_back(ping_pool.enqueue(thread_pinger, host, this_is(host), &oc.po));
      if (futures_ping.size() >= 100) {
        for (auto& future : futures_ping){future.get();}
        futures_ping.clear();
    }
  }

  for (auto& future : futures_ping){future.wait();}
  futures_ping.clear();

  struct timespec end_time;
  clock_gettime(CLOCK_MONOTONIC, &end_time);
  double elapsed_time = (end_time.tv_sec - start_time.tv_sec) +
      (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

  double min_rtt = std::numeric_limits<double>::max();
  std::string ip_fast;
  for (const std::string& ip : cd5.md5_hash_map[cd5.key]) {
    if (oc.rtts.find(ip) != oc.rtts.end() && oc.rtts[ip] != -1) {
      if (oc.rtts[ip] < min_rtt) {
        min_rtt = oc.rtts[ip];
        ip_fast = ip;
       }
    }
  }
  std::vector<std::string> ips = cd5.md5_hash_map[cd5.key];
  std::unordered_map<std::string, double> rtts_this = oc.rtts;

  std::sort(ips.begin(), ips.end(), [&rtts_this](const std::string& ip1, const std::string& ip2) {
    double rtt1 = rtts_this[ip1];
    double rtt2 = rtts_this[ip2];
    if (rtt1 < 0) return false;
    if (rtt2 < 0) return true;
    return rtt1 < rtt2;
  });

  oc.rtts = rtts_this;
  cd5.md5_hash_map[cd5.key] = ips;
  rtts_this.clear();
  ips.clear();

  printf("* Finished ping scan at %.2fs, fasted=%s, rtt=%0.2fms\n\n", elapsed_time, ip_fast.c_str(), min_rtt);
  printf("* Sending loop start... \n");

  oc.so.id_map = cd5.md5_hash_map;

  __send(cd5.key, &oc.so);
}

#include <openssl/md5.h>
std::string md5(const std::string& input)
{
  unsigned char digest[MD5_DIGEST_LENGTH];
  MD5((const unsigned char*)input.c_str(), input.length(), digest);

  std::stringstream ss;
  for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
    ss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
  }
  return ss.str();
}

void encode_md5_data(centaur_data_md5 *cd5)
{
  std::vector<std::string> temp_vector;
  for (const std::string& value : cd5->temp_file) {cd5->combined_values += value + ";"; temp_vector.push_back(value);}
  if (!cd5->combined_values.empty()) {cd5->combined_values.pop_back();}

  uint8_t _md5[16];
  char temp_buffer[cd5->combined_values.size() + 1];
  strcpy(temp_buffer, cd5->combined_values.c_str());
  cd5->key = md5(temp_buffer);
  cd5->md5_hash_map[cd5->key] = temp_vector;
}

void thread_pinger(const char* host, int type, struct pinger_options *po)
{
  double temp = pinger(host, type, po);
#ifdef __VERBOSE
  printf("(%s) ping is: %f\n",host, temp);
#endif
  lol.lock();
  oc.rtts[host] = temp;
  lol.unlock();
}

std::vector<std::string> split_string_string(const std::string& str, char delimiter)
{
  std::vector<std::string> result;
  size_t pos = 0, found;
  std::string token;
  while ((found = str.find_first_of(delimiter, pos)) != std::string::npos) {
      token = str.substr(pos, found - pos);
      result.push_back(token);
      pos = found + 1;
  }
  result.push_back(str.substr(pos));
  return result;
}

std::vector<char> const_char_ptr_to_vector(const char* data)
{
  std::vector<char> result(data, data + std::strlen(data));
  return result;
}

std::vector<char> std_string_to_vector(const std::string& data)
{
  std::vector<char> result(data.begin(), data.end());
  return result;
}

std::vector<char> char_ptr_to_vector(char* data)
{
  std::vector<char> result(data, data + std::strlen(data));
  return result;
}

template <std::size_t N> std::vector<char> char_array_to_vector(const char (&data)[N])
{
  std::vector<char> result(data, data + N);
  return result;
}
