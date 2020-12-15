#include "helper.h"

using namespace no_libevent;

void run(void) {
  int listener;
  struct fd_state *state[FD_SETSIZE];
  struct sockaddr_in sin;
  int i, maxfd;
  fd_set readset, writeset, exset;

  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = 0;
  sin.sin_port = htons(LISTENING_PORT);

  for (i = 0; i < FD_SETSIZE; ++i) state[i] = NULL;

  listener = socket(AF_INET, SOCK_STREAM, 0);
  make_nonblocking(listener);

  if (bind(listener, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    perror("bind");
    return;
  }

  if (listen(listener, 16) < 0) {
    perror("listen");
    return;
  }

  FD_ZERO(&readset);
  FD_ZERO(&writeset);
  FD_ZERO(&exset);

  while (1) {
    maxfd = listener;

    FD_ZERO(&readset);
    FD_ZERO(&writeset);
    FD_ZERO(&exset);

    FD_SET(listener, &readset);

    for (i = 0; i < FD_SETSIZE; ++i) {
      if (state[i]) {
        if (i > maxfd) maxfd = i;
        FD_SET(i, &readset);
        if (state[i]->writing) {
          FD_SET(i, &writeset);
        }
      }
    }

    if (select(maxfd + 1, &readset, &writeset, &exset, NULL) < 0) {
      perror("select");
      return;
    }

    if (FD_ISSET(listener, &readset)) {
      struct sockaddr_storage ss;
      socklen_t slen = sizeof(ss);
      int fd = accept(listener, (struct sockaddr *)&ss, &slen);
      if (fd < 0) {
        perror("accept");
      } else if (fd > FD_SETSIZE) {
        close(fd);
      } else {
        make_nonblocking(fd);
        state[fd] = alloc_fd_state();
        assert(state[fd]); /*XXX*/
      }
    }

    for (i = 0; i < maxfd + 1; ++i) {
      int r = 0;
      if (i == listener) continue;

      if (FD_ISSET(i, &readset)) {
        r = do_read(i, state[i]);
      }
      if (r == 0 && FD_ISSET(i, &writeset)) {
        r = do_write(i, state[i]);
      }
      if (r) {
        free_fd_state(state[i]);
        state[i] = NULL;
        close(i);
      }
    }
  }
}

int main(int c, char **v) {
  setvbuf(stdout, NULL, _IONBF, 0);

  run();
  return 0;
}
