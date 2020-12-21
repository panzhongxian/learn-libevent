#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/event.h>

#include "helper.h"

void readcb(struct bufferevent *bev, void *ctx) {
  struct evbuffer *input, *output;
  char *line;
  size_t n;
  input = bufferevent_get_input(bev);
  output = bufferevent_get_output(bev);

  while ((line = evbuffer_readln(input, &n, EVBUFFER_EOL_LF))) {
    for (size_t i = 0; i < n; ++i) line[i] = rot13_char(line[i]);
    evbuffer_add(output, line, n);
    evbuffer_add(output, "\n", 1);
    free(line);
  }

  if (evbuffer_get_length(input) >= MAX_LINE) {
    /* Too long; just process what there is and go on so that the buffer
     * doesn't grow infinitely long. */
    char buf[1024];
    while (evbuffer_get_length(input)) {
      int n = evbuffer_remove(input, buf, sizeof(buf));
      for (int i = 0; i < n; ++i) buf[i] = rot13_char(buf[i]);
      evbuffer_add(output, buf, n);
    }
    evbuffer_add(output, "\n", 1);
  }
}

void errorcb(struct bufferevent *bev, short error, void *ctx) {
  if (error & BEV_EVENT_EOF) {
    /* connection has been closed, do any clean up here */
    /* ... */
  } else if (error & BEV_EVENT_ERROR) {
    /* check errno to see what error occurred */
    /* ... */
  } else if (error & BEV_EVENT_TIMEOUT) {
    /* must be a timeout event handle, handle it */
    /* ... */
  }
  bufferevent_free(bev);
}

void do_accept(evutil_socket_t listener, short event, void *arg) {
  struct event_base *base = (struct event_base *)arg;
  struct sockaddr_storage ss;
  socklen_t slen = sizeof(ss);
  int fd = accept(listener, (struct sockaddr *)&ss, &slen);
  if (fd < 0) {
    perror("accept");
  } else if (fd > FD_SETSIZE) {
    close(fd);
  } else {
    struct bufferevent *bev;
    evutil_make_socket_nonblocking(fd);
    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, readcb, NULL, errorcb, NULL);
    bufferevent_setwatermark(bev, EV_READ, 0, MAX_LINE);
    bufferevent_enable(bev, EV_READ | EV_WRITE);
  }
}

void run(void) {
  struct event_base *base;

  base = event_base_new();
  if (!base) return;

  evutil_socket_t listener = CreateListener(true);

  struct event *listener_event =
      event_new(base, listener, EV_READ | EV_PERSIST, do_accept, (void *)base);
  assert(listener_event);
  event_add(listener_event, NULL);

  event_base_dispatch(base);
}

int main(int c, char **v) {
  setvbuf(stdout, NULL, _IONBF, 0);

  run();
  return 0;
}
