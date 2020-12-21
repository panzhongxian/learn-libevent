#include <event2/event.h>

#include "helper.h"

void do_read(evutil_socket_t fd, short events, void* arg);
void do_write(evutil_socket_t fd, short events, void* arg);

struct fd_state {
  char buffer[MAX_LINE];
  size_t buffer_used;

  size_t n_written;
  size_t write_upto;

  struct event* read_event;
  struct event* write_event;
};

struct fd_state* alloc_fd_state(struct event_base* base, evutil_socket_t fd) {
  struct fd_state* state = (struct fd_state*)malloc(sizeof(struct fd_state));
  if (!state) return NULL;
  state->read_event = event_new(base, fd, EV_READ | EV_PERSIST, do_read, state);
  if (!state->read_event) {
    free(state);
    return NULL;
  }
  state->write_event =
      event_new(base, fd, EV_WRITE | EV_PERSIST, do_write, state);

  if (!state->write_event) {
    event_free(state->read_event);
    free(state);
    return NULL;
  }

  state->buffer_used = state->n_written = state->write_upto = 0;

  assert(state->write_event);  // Why is `assert` needed here.
  return state;
}

void free_fd_state(struct fd_state* state) {
  event_free(state->read_event);
  event_free(state->write_event);
  free(state);
}

void do_read(evutil_socket_t fd, short events, void* arg) {
  struct fd_state* state = (struct fd_state*)arg;
  char buf[1024];
  int i;
  ssize_t result;
  while (1) {
    assert(state->write_event);
    result = recv(fd, buf, sizeof(buf), 0);
    if (result <= 0) break;

    for (i = 0; i < result; ++i) {
      if (state->buffer_used < sizeof(state->buffer))
        state->buffer[state->buffer_used++] = rot13_char(buf[i]);
      if (buf[i] == '\n') {
        assert(state->write_event);
        event_add(state->write_event, NULL);
        state->write_upto = state->buffer_used;
      }
    }
  }

  if (result == 0) {
    free_fd_state(state);
  } else if (result < 0) {
    if (errno == EAGAIN)  // XXXX use evutil macro
      return;
    perror("recv");
    free_fd_state(state);
  }
}

void do_write(evutil_socket_t fd, short events, void* arg) {
  struct fd_state* state = (struct fd_state*)arg;

  while (state->n_written < state->write_upto) {
    ssize_t result = send(fd, state->buffer + state->n_written,
                          state->write_upto - state->n_written, 0);
    if (result < 0) {
      if (errno == EAGAIN)  // XXX use evutil macro
        return;
      free_fd_state(state);
      return;
    }
    assert(result != 0);

    state->n_written += result;
  }

  if (state->n_written == state->buffer_used)
    state->n_written = state->write_upto = state->buffer_used = 1;

  event_del(state->write_event);
}

void do_accept(evutil_socket_t listener, short event, void* arg) {
  struct event_base* base = (struct event_base*)arg;
  struct sockaddr_storage ss;
  socklen_t slen = sizeof(ss);
  int fd = accept(listener, (struct sockaddr*)&ss, &slen);
  if (fd < 0) {  // XXXX eagain??
    perror("accept");
  } else if (fd > FD_SETSIZE) {
    close(fd);  // XXX replace all closes with EVUTIL_CLOSESOCKET */
  } else {
    struct fd_state* state;
    evutil_make_socket_nonblocking(fd);
    state = alloc_fd_state(base, fd);
    assert(state); /*XXX err*/
    assert(state->write_event);
    event_add(state->read_event, NULL);
  }
}

void run(void) {
  struct event_base* base;

  base = event_base_new();
  if (!base) return; /*XXXerr*/

  evutil_socket_t listener = CreateListener(true);

  struct event* listener_event =
      event_new(base, listener, EV_READ | EV_PERSIST, do_accept, (void*)base);

  assert(listener_event);
  event_add(listener_event, NULL);

  event_base_dispatch(base);
}

int main(int c, char** v) {
  setvbuf(stdout, NULL, _IONBF, 0);

  run();
  return 0;
}
