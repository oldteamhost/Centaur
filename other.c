/*
 * CENTAUR
 * by oldteam & lomaster
 * license GPL-3.0
 *   Сделано от души 2023.
*/

#include "include/other.h"

int
dns_or_ip(const char* host)
{
  struct sockaddr_in sa;
  return (inet_pton(AF_INET, host, &(sa.sin_addr)) == 0);
}

const char* clear_url(const char* url)
{
  const char* https_prefix = "https://";
  const char* http_prefix = "http://";
  const char* slash = "/";

  const char* cleaned_url = url;

  if (strncmp(url, https_prefix, strlen(https_prefix)) == 0) {
    cleaned_url += strlen(https_prefix);
  }
  else if (strncmp(url, http_prefix, strlen(http_prefix)) == 0) {
    cleaned_url += strlen(http_prefix);
  }

  size_t url_length = strlen(cleaned_url);
  if (url_length > 0 && cleaned_url[url_length - 1] == '/') {
    char* modified_url = (char*)malloc(url_length);
    strncpy(modified_url, cleaned_url, url_length - 1);
    modified_url[url_length - 1] = '\0';
    return modified_url;
  }

  return cleaned_url;
}

void delay_ms(int milliseconds)
{
	struct timespec ts;
	ts.tv_sec = milliseconds / 1000;
	ts.tv_nsec = (milliseconds % 1000) * 1000000;
	nanosleep(&ts, NULL);
}

const char* get_time()
{
  time_t rawtime;
  struct tm * timeinfo;
  static char time_str[9];

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  sprintf(time_str, "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
  return time_str;
}

const char* get_ip_by_dns(const char* dns)
{
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == EOF) {return "n/a";}

  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  int status = getaddrinfo(dns, NULL, &hints, &res);

  if (status != 0) {close(sock);return "n/a";}

  struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
  const char* ip = inet_ntoa(addr->sin_addr);

  freeaddrinfo(res);
  close(sock);
  return ip;
}

const char* get_local_ip(void)
{
  struct sockaddr_in serv;
  static char buffer[100];
  socklen_t namelen;

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {return "-1";}

  const char *kGoogleDnsIp = "8.8.8.8";
  const int dns_port             = 53;

  memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_addr.s_addr = inet_addr(kGoogleDnsIp);
  serv.sin_port = htons(dns_port);

  const int err = connect(sock,(const struct sockaddr*)&serv, sizeof(serv));
  if (err < 0) {close(sock);return "-1";}

  struct sockaddr_in name;
  namelen = sizeof(name);
  memset(&name, 0, sizeof(name));
  if (getsockname(sock,(struct sockaddr*)&name, &namelen)){close(sock);return "-1";}

  const char *p = inet_ntop(AF_INET, &name.sin_addr, buffer, sizeof(buffer));
  close(sock);
  return p;
}
