#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#ifdef USING_LIBEVENT
#include <event2/util.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#endif

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "utility.h"

int get_addr_info(const std::string& domain, std::string& ip) {
#ifdef USING_LIBEVENT
  struct evutil_addrinfo hints;
  struct evutil_addrinfo* res = new evutil_addrinfo();
#else
  struct addrinfo hints;
  struct addrinfo* res = new addrinfo();
#endif

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  char paddr[INET_ADDRSTRLEN];

#ifdef USING_LIBEVENT
  int ret = evutil_getaddrinfo(domain.c_str(), NULL, &hints, &res);
#else
  int ret = getaddrinfo(domain.c_str(), NULL, &hints, &res);
#endif

  if (ret != 0) {
#ifdef USING_LIBEVENT
    printf("errno: %d, errinfo: %s\n", ret, evutil_gai_strerror(ret));
#else
    printf("errno: %d, errinfo: %s\n", ret, gai_strerror(ret));
#endif
    return 1;
  }
  while (res) {
    auto ptr = (struct sockaddr_in*)(res->ai_addr);
    res = res->ai_next;
    inet_ntop(AF_INET, &(ptr->sin_addr), paddr, INET_ADDRSTRLEN);
    ip += std::string(paddr) + ";";
  }
  return ip.empty() ? 2 : 0;
}

int main() {
  std::vector<std::string> url_vec;
  LoadChinaTop100Websites(url_vec);
  for (std::string& url : url_vec) {
    std::string ip;
    timeval start_time, end_time, cost_time;
    gettimeofday(&start_time, NULL);
    int ret = get_addr_info(url.substr(7), ip);  // remove "http://"
    gettimeofday(&end_time, NULL);
    timersub(&end_time, &start_time, &cost_time);

    fprintf(stdout, "cost: %2ld.%06lds   url: %-25s  ip: %-15s",
            cost_time.tv_sec, cost_time.tv_usec, url.c_str(), ip.c_str());

    if (ret) {
      fprintf(stdout, " get_addr_info error ret: %d\n", ret);
    } else {
      fprintf(stdout, "\n");
    }
  }
  return 0;
}
