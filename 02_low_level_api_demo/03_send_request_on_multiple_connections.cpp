#include <arpa/inet.h>
#include <assert.h>
#include <event2/event.h>
#include <event2/util.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "utility.h"

void OnWrite(evutil_socket_t fd, short what, void* arg);
void OnRead(evutil_socket_t fd, short what, void* arg);

void OnWrite(evutil_socket_t fd, short what, void* arg) {
  struct event* write_event = (struct event*)arg;
  if (what & EV_WRITE) {
    int ret = write(fd, kDemoHttpRequestStr, sizeof(kDemoHttpRequestStr));
    if (ret < (int)strlen(kDemoHttpRequestStr)) {
      printf("write ret non-zero value: %d\n", ret);
      event_del((struct event*)arg);
      return;
    }
    printf("writing...ret: %d\n", ret);
  }

  if (what & EV_TIMEOUT) {
    event_del((struct event*)arg);
    printf("write timeout.\n");
    return;
  }

  struct timeval five_sec = {5, 0};
  struct event* e = event_new(event_get_base(write_event), fd,
                              EV_READ | EV_TIMEOUT, OnRead, event_self_cbarg());
  event_add(e, &five_sec);
  event_del(write_event);
  printf("connected.\n");
}

void OnRead(evutil_socket_t fd, short what, void* arg) {
  if (what & EV_READ) {
    char buff[1024];
    printf("reading...\n");
    while (true) {
      int n = read(fd, buff, sizeof(buff));
      printf("%.*s", n, buff);
      if (n == 0) {
        close(fd);
        event_del((struct event*)arg);
        return;
      }
      if (n < 0) {
        // FIXME: if (errno == EAGAIN || errno == EWOULDBLOCK)
        perror("read() error:");
        close(fd);
        event_del((struct event*)arg);
        return;
      }
    }
  }
  if (what & EV_TIMEOUT) {
    printf("read timeout.\n");
    event_del((struct event*)arg);
    return;
  }
}

void OnConnect(evutil_socket_t fd, short what, void* arg) {
  assert(arg);
  struct event* connect_event = (struct event*)arg;
  if ((what & EV_WRITE)) {
    int result;
    socklen_t result_len = sizeof(result);
    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &result, &result_len) < 0) {
      // error, fail somehow, close socket
      perror("getsockopt failed");
      event_del(connect_event);
      return;
    }

    if (result != 0) {
      // connection failed; error code is in 'result'
      printf("result != 0: %s\n", strerror(result));
      event_del(connect_event);
      return;
    }

    struct timeval five_sec = {5, 0};
    struct event* e =
        event_new(event_get_base(connect_event), fd, EV_WRITE | EV_TIMEOUT,
                  OnWrite, event_self_cbarg());
    event_add(e, &five_sec);
    event_del(connect_event);
    printf("connected.\n");
    return;
  }

  if (what & EV_TIMEOUT) {
    printf("connect timeout.\n");
    event_del((struct event*)arg);
    return;
  }
  abort();
}

void Run() {
  struct event_base* base = event_base_new();
  struct timeval five_sec = {5, 0};

  for (int i = 0; i < kMaxHttpClientNum; i++) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(fd, F_SETFL, O_NONBLOCK);
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(kDemoHttpServerIp);
    sin.sin_port = htons(kDemoHttpServerPort);
    if (connect(fd, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
      if (errno != EINPROGRESS) {
        perror("connect failed");
        event_base_loopbreak(base);
        return;
      }
    }
    struct event* e = event_new(base, fd, EV_WRITE | EV_TIMEOUT, OnConnect,
                                event_self_cbarg());
    event_add(e, &five_sec);
  }
  timeval t;
  gettimeofday(&t, NULL);
  fprintf(stderr, "start time: %ld.%06ld\n", t.tv_sec, t.tv_usec);
  event_base_dispatch(base);
  gettimeofday(&t, NULL);
  fprintf(stderr, "end time: %ld.%06ld\n", t.tv_sec, t.tv_usec);
  return;
}

int main() {
  Run();
  return 0;
}
