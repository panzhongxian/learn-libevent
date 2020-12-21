#ifndef HELPER_H_
#define HELPER_H_

// clang-format off
/* for gethostbyname */
#include <netdb.h>
/* for sockaddr_in */
#include <netinet/in.h>
/* for socket functions */
#include <sys/socket.h>
/* for fcntl */
#include <unistd.h>
#include <fcntl.h>
/* for select */
#include <sys/select.h>
/* for epoll */
#include <sys/epoll.h>
/* for multiple threads */
#include <pthread.h>
// clang-format on

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define MAX_LINE 16384
#define MAX_EVENTS 100
#define LISTENING_PORT 40713

char rot13_char(char c) {
  /* We don't want to use isalpha here; setting the locale would change
   * which characters are considered alphabetical. */
  if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
    return c + 13;
  else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
    return c - 13;
  else
    return c;
}

int CreateListener(bool block_flag = false);

int CreateListener(bool block_flag) {
  struct sockaddr_in sin;

  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = 0;
  sin.sin_port = htons(LISTENING_PORT);

  int listener = socket(AF_INET, SOCK_STREAM, 0);
  if (block_flag) {
    fcntl(listener, F_SETFL, O_NONBLOCK);
  }
  if (bind(listener, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if (listen(listener, 16) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  return listener;
}

namespace no_libevent {

struct fd_state {
  char buffer[MAX_LINE];
  size_t buffer_used;

  size_t n_written;
  size_t write_upto;

  bool writing;
};

struct fd_state *alloc_fd_state(void) {
  struct fd_state *state = (struct fd_state *)malloc(sizeof(struct fd_state));
  if (!state) return NULL;
  state->buffer_used = state->n_written = state->write_upto = 0;
  state->writing = false;
  return state;
}

void free_fd_state(struct fd_state *state) { free(state); }

void make_nonblocking(int fd) { fcntl(fd, F_SETFL, O_NONBLOCK); }

int do_read(int fd, struct fd_state *state) {
  char buf[1024];
  int i;
  ssize_t result;
  while (1) {
    result = recv(fd, buf, sizeof(buf), 0);
    if (result <= 0) break;

    for (i = 0; i < result; ++i) {
      if (state->buffer_used < sizeof(state->buffer))
        state->buffer[state->buffer_used++] = rot13_char(buf[i]);
      if (buf[i] == '\n') {
        state->writing = true;
        state->write_upto = state->buffer_used;
      }
    }
  }

  if (result == 0) {
    return 1;
  } else if (result < 0) {
    if (errno == EAGAIN) return 0;
    return -1;
  }

  return 0;
}

int do_write(int fd, struct fd_state *state) {
  while (state->n_written < state->write_upto) {
    ssize_t result = send(fd, state->buffer + state->n_written,
                          state->write_upto - state->n_written, 0);
    if (result < 0) {
      if (errno == EAGAIN) return 0;
      return -1;
    }
    assert(result != 0);

    state->n_written += result;
  }

  if (state->n_written == state->buffer_used)
    state->n_written = state->write_upto = state->buffer_used = 0;

  state->writing = false;
  return 0;
}

}  // namespace no_libevent

#endif  // HELPER_H_
