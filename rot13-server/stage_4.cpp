#include "helper.h"

using namespace no_libevent;

void run(void) {
  struct fd_state* state[MAX_EVENTS];
  int epfd = epoll_create1(0);
  if (epfd == -1) {
    perror("epoll_create1");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < MAX_EVENTS; ++i) state[i] = NULL;

  int listener = CreateListener(true);

  struct epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = listener;
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, listener, &ev) == -1) {
    perror("epoll_ctl: listener");
    exit(EXIT_FAILURE);
  }

  struct epoll_event events[MAX_EVENTS];
  while (1) {
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
    if (nfds == -1) {
      perror("epoll_wait");
      exit(EXIT_FAILURE);
    }

    for (int i = 0; i < nfds; ++i) {
      if (events[i].data.fd == listener) {
        struct sockaddr_in addr;
        socklen_t addrlen;
        int conn_sock = accept(listener, (struct sockaddr*)&addr, &addrlen);
        if (conn_sock == -1) {
          perror("accept");
          exit(EXIT_FAILURE);
        }

        make_nonblocking(conn_sock);
        state[conn_sock] = alloc_fd_state();
        assert(state[conn_sock]);
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
        ev.data.fd = conn_sock;
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
          perror("epoll_ctl: conn_sock");
          exit(EXIT_FAILURE);
        }
      } else {
        int r = 0;
        int fd = events[i].data.fd;
        if (events[i].events & EPOLLIN) {
          r = do_read(fd, state[fd]);
        }
        if (events[i].events & EPOLLOUT) {
          r = do_write(fd, state[fd]);
        }
        if (r) {
          free_fd_state(state[i]);
          state[i] = NULL;
          close(i);
        }
      }
    }
  }
}

int main(int c, char** v) {
  setvbuf(stdout, NULL, _IONBF, 0);

  run();
  return 0;
}
