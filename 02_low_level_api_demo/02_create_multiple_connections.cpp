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

static int max_connections;

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
    printf("connected.\n");
    close(fd);
    event_del(connect_event);
    return;
  }

  if (what & EV_TIMEOUT) {
    printf("timeout.\n");
    event_del((struct event*)arg);
    return;
  }
}

void CreateConections() {
  struct event_base* base = event_base_new();
  struct timeval five_sec = {5, 0};

  for (int i = 0; i < max_connections; i++) {
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
    } else {
      printf("connect return 0.");
    }
    struct event* e = event_new(base, fd, EV_WRITE | EV_TIMEOUT, OnConnect,
                                event_self_cbarg());
    event_add(e, &five_sec);
  }
  event_base_dispatch(base);
  return;
}

int main(int argc, char* argv[]) {
  max_connections = kMaxHttpClientNum;
  if (argc == 2) {
    max_connections = atoi(argv[1]);
  }
  printf("kMaxHttpClientNum: %d\n", kMaxHttpClientNum);
  for (int i = 0; i < 1; i++) {
    CreateConections();
  }
  return 0;
}
