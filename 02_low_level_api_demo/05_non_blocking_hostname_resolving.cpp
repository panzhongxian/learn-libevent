#include <arpa/inet.h>
#include <event2/dns.h>
#include <event2/event.h>
#include <event2/util.h>
#include <sys/time.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <string>

#include "utility.h"

static int hostname_count = 0;

struct Context {
  timeval start_time;
  timeval cost_time;
  std::string url;
  std::string ip;
  std::string err;
  struct event_base* base;
};

void callback(int errcode, struct evutil_addrinfo* addr, void* ptr) {
  Context* context_ptr = (Context*)ptr;
  if (errcode) {
    context_ptr->err = evutil_gai_strerror(errcode);
    fprintf(stderr, "error: %s\n", context_ptr->err.c_str());
  } else {
    struct evutil_addrinfo* ai;
    for (ai = addr; ai; ai = ai->ai_next) {
      char buf[128];
      const char* s = NULL;
      if (ai->ai_family == AF_INET) {
        struct sockaddr_in* sin = (struct sockaddr_in*)ai->ai_addr;
        s = evutil_inet_ntop(AF_INET, &sin->sin_addr, buf, 128);
      } else if (ai->ai_family == AF_INET6) {
        struct sockaddr_in6* sin6 = (struct sockaddr_in6*)ai->ai_addr;
        s = evutil_inet_ntop(AF_INET6, &sin6->sin6_addr, buf, 128);
      }
      if (s) {
        context_ptr->ip += std::string(s) + ";";
      }
    }
    evutil_freeaddrinfo(addr);
  }
  timeval end_time;
  gettimeofday(&end_time, NULL);
  timersub(&end_time, &context_ptr->start_time, &context_ptr->cost_time);

  if (--hostname_count == 0) event_base_loopexit(context_ptr->base, NULL);
}

int main() {
  std::vector<std::string> url_vec;
  LoadChinaTop100Websites(url_vec);

  std::vector<struct Context> context_vec(url_vec.size());

  struct event_base* base;
  base = event_base_new();

  struct evdns_base* dnsbase;
  dnsbase = evdns_base_new(base, 1);

  for (size_t i = 0; i < url_vec.size(); ++i) {
    std::string& url = url_vec[i];
    std::string ip;
    struct evdns_getaddrinfo_request* req;
    evutil_addrinfo hints;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    req = evdns_getaddrinfo(dnsbase, url.substr(7).c_str(), NULL, &hints,
                            callback, &context_vec[i]);
    context_vec[i].base = base;
    context_vec[i].url = url_vec[i];
    gettimeofday(&context_vec[i].start_time, NULL);
    assert(req);
  }

  hostname_count = url_vec.size();
  event_base_dispatch(base);

  double total_cost = 0;
  for (Context& context : context_vec) {
    fprintf(stdout, "cost: %2ld.%06lds   url: %-25s  ip: %-15s\n",
            context.cost_time.tv_sec, context.cost_time.tv_usec,
            context.url.c_str(), context.ip.c_str());
    total_cost += context.cost_time.tv_sec + context.cost_time.tv_usec * 1e-6;
  }

  fprintf(stdout, "total cost: %lfs\n", total_cost);
  evdns_base_free(dnsbase, 0);
  event_base_free(base);
}
