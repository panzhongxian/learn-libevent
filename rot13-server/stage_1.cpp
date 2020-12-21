#include "helper.h"

using namespace no_libevent;

void child(int fd) {
  char outbuf[MAX_LINE + 1];
  size_t outbuf_used = 0;
  ssize_t result;

  while (1) {
    char ch;
    result = recv(fd, &ch, 1, 0);
    if (result == 0) {
      break;
    } else if (result == -1) {
      perror("read");
      break;
    }

    /* We do this test to keep the user from overflowing the buffer. */
    if (outbuf_used < sizeof(outbuf)) {
      outbuf[outbuf_used++] = rot13_char(ch);
    }

    if (ch == '\n') {
      send(fd, outbuf, outbuf_used, 0);
      outbuf_used = 0;
      continue;
    }
  }
}

void run(void) {
  int listener = CreateListener();
  while (1) {
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    int fd = accept(listener, (struct sockaddr *)&ss, &slen);
    if (fd < 0) {
      perror("accept");
    } else {
      if (fork() == 0) {
        child(fd);
        exit(0);
      }
    }
  }
}

int main(int c, char **v) {
  run();
  return 0;
}
