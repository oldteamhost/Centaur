#include "include/log.h"
#include <stdio.h>

void running(const char* version)
{
  time_t now   = time(NULL);
  struct tm *t = localtime(&now); char datetime[20];
  strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", t);

  printf("Starting CENTAUR %s at %s\n", version, datetime);
}

void usage(const char *run)
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

void qutting(void)
{
  puts("Goodbye)\n");
  exit(0);
}

void version(const char* version)
{
  printf("by lomaster & oldteam CENTAUR %s\n", version);
}

void sudorun(void)
{
#include <unistd.h>
  if(geteuid()) {
    printf("Only root run, please run with sudo.\n");
    exit(1);
  }

}
