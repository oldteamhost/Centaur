/*
 * CENTAUR
 * by oldteam & lomaster
 * license GPL-3.0
 *   Сделано от души 2023.
*/

#include "include/centaur.h"
#include "include/icmp4.h"
#include "include/nescathread.h"
#include "include/connect.h"
#include "include/send.h"
#include "include/status.h"
#include "include/other.h"
#include "include/pinger.h"
#include "include/socks5.h"
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <algorithm>

struct options_centaur
{
    bool non_stop;
    int ping_threads;

    std::vector<char> message;
    std::string myself;
    std::unordered_map<std::string, double> rtts;
    int send_port;
};

centaur_data_md5 cd5;
options_centaur oc;
pinger_options po;
socks_5_connection connection;
std::mutex lol;

const char* shortopts = "hvm:p:l:";

void thread_pinger(const char* host, int type, struct pinger_options *po)
{
    double temp = pinger(host, type, po);
    /*printf("(%s) ping is: %f\n",host, temp);*/
    lol.lock();
    oc.rtts[host] = temp;
    lol.unlock();
}

int main(int argc, char** argv)
{
    if (argc <= 1){usage(argv[0]);}

    po.delay_ping = 0;
    po.icmp_timeout = 1000;
    po.icmp_ttl = 121;
    po.tcp_timeout = 2000;
    po.tcp_port = 80;
    oc.ping_threads = 200;
    oc.non_stop = false;
    oc.send_port = 80;
    oc.myself = "anonymous";
    std::string temp_message = "Hello Internet!";
    oc.message = std_string_to_vector(temp_message);

    int rez, option_index = 0;
    while ((rez = getopt_long_only(argc, argv, shortopts, longopts, &option_index)) != EOF)
    {
        switch (rez)
        {
            case 'h':
                usage(argv[0]);
                break;
            case 'v':
                version();
                break;
            case 'p':
                oc.send_port = atoi(optarg);
                break;
            case 'm':
                oc.message = const_char_ptr_to_vector(optarg);
                break;
            case 1:
                oc.ping_threads = atoi(optarg);
                break;
            case 2:
                po.delay_ping = atoi(optarg);
                break;
            case 3:
                po.icmp_timeout = atoi(optarg);
                break;
            case 4:
                po.tcp_timeout = atoi(optarg);
                break;
            case 5:
                po.tcp_port = atoi(optarg);
                break;
            case 6:
                po.icmp_ttl = atoi(optarg);
                break;
            case 7:
                oc.non_stop = true;
                break;
            case 8:
                oc.myself = optarg;
                break;
        }
    }

    std::vector<std::string> files;
    time_t now = time(NULL);
    struct tm *t = localtime(&now); char datetime[20];
    strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", t);
    printf("Starting CENTAUR %s at %s\n", VERSION, datetime);
    sudorun();

    if (optind < argc)
	{
        files = split_string_string(argv[optind], ',');
    }
    for (const auto& file : files)
    {
        std::string file_temp = file;
        parse_file(&cd5, file_temp);
    }
    encode_md5_data(&cd5);
    double ping = icmp_ping(get_ip_by_dns("google.com"), po.icmp_timeout, 8, 0, 1, po.icmp_ttl);

    printf("> local: ip=%s, ping=%0.2fms, files=%lu, hosts=%lu\n", get_local_ip(), ping, files.size(), cd5.temp_file.size());
    printf("> icmp: type=8, code=0, ttl=%d, timeout=%d, delay=%d\n\n", po.icmp_ttl, po.icmp_timeout, po.delay_ping);

    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    printf("* Ping scan at %lu hosts, threads=%d, time=%s\n", cd5.md5_hash_map[cd5.key].size(), oc.ping_threads, get_time());

    int i = 0;
    std::vector<std::future<void>> futures_ping;
    thread_pool ping_pool(100);
    for (int i = 0; i < cd5.md5_hash_map[cd5.key].size(); i++)
    {
        const char* host = cd5.md5_hash_map[cd5.key][i].c_str();
        futures_ping.emplace_back(ping_pool.enqueue(thread_pinger, host, this_is(host), &po));
        if (futures_ping.size() >= 100){
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
        if (oc.rtts.find(ip) != oc.rtts.end() && oc.rtts[ip] != -1)
        {
            if (oc.rtts[ip] < min_rtt)
            {
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

    send_options message;

    message.bytes = oc.message;
    message.id_map = cd5.md5_hash_map;
    message.non_stop = oc.non_stop;
    message.send_port = oc.send_port;
    message.myself = oc.myself;

    __send(cd5.key, &message);
}

#include <openssl/md5.h>
std::string md5(const std::string& input)
{
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5((const unsigned char*)input.c_str(), input.length(), digest);

    std::stringstream ss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
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

/* old (not use) */
void print_md5_hash(const uint8_t* md5hash)
{
    for (int i = 0; i < 16; ++i)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(md5hash[i]);
    }
    std::cout << std::endl;
}

void parse_file(centaur_data_md5 *cd5, std::string& filename)
{
    std::vector<std::string> temp = read_file(filename);
    cd5->temp_file = temp;
}

std::vector<std::string> split_string_string(const std::string& str, char delimiter)
{
    std::vector<std::string> result;
    size_t pos = 0, found;
    std::string token;
    while ((found = str.find_first_of(delimiter, pos)) != std::string::npos)
	{
        token = str.substr(pos, found - pos);
        result.push_back(token);
        pos = found + 1;
    }
    result.push_back(str.substr(pos));
    return result;
}

void usage(const char* run)
{
    printf("usage: %s [files 1,2,3...] [flags]\n",run);

    puts("\narguments program:");
    puts("  -h, -help             Show this help message.");
    puts("  -v, -version          Show program version.");

    puts("\narguments verbose:");
    puts("  -m, -message <text>   Set message for send.");
    puts("  -myself <self>        Edit source host.");
    puts("  -p, -port <port>      Set port for send.");
    puts("  -non-stop             Not stop after success sent.");

    puts("\narguments speed:");
    puts("  -ping-threads <num>   Edit count threads for ping scan.");
    puts("  -ping-delay <num>     Set delay for ping scan.");

    puts("\narguments ping:");
    puts("  -icmp-timeout <ms>    Edit recv timeout for icmp ping.");
    puts("  -tcp-timeout <ms>     Edit recv timeout for tcp ping.");
    puts("  -ping-port <port>     Edit port for tcp ping.");
    puts("  -ttl <num>            Edit ttl for icmp ping.");

    exit(0);
}

void version(void)
{
    printf("by lomaster & oldteam CENTAUR %s\n", VERSION);
}

void sudorun(void)
{
    if(geteuid())
    {
        printf("Only root run, please run with sudo.\n");
        exit(1);
    }
}

std::vector<char> const_char_ptr_to_vector(const char* data)
{
    std::vector<char> result(data, data + std::strlen(data));
    return result;
}

void print_char_vector(const std::vector<char>& vec)
{
    for (char c : vec)
    {
        std::cout << c;
    }
    std::cout << std::endl;
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

template <std::size_t N>
std::vector<char> char_array_to_vector(const char (&data)[N])
{
    std::vector<char> result(data, data + N);
    return result;
}
