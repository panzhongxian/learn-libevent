#include "helper.h"

using namespace no_libevent;

int accept_fd = 0;
pthread_mutex_t lock;
int est_conn = 0;
pthread_t thread_array[MAX_EVENTS];

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

void *task(void *) {
  while (true) {
    if (accept_fd) {
      pthread_mutex_lock(&lock);
      if (accept_fd) {
        child(accept_fd);
        accept_fd = 0;
      }
      pthread_mutex_unlock(&lock);
    } else {
      usleep(100);
    }
  }
  return nullptr;
}
void run(void) {
  int listener = CreateListener();

  for (int i = 0; i < MAX_EVENTS; ++i) {
    pthread_create(&thread_array[i], NULL, task, NULL);
  }

  while (1) {
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    accept_fd = accept(listener, (struct sockaddr *)&ss, &slen);
    if (accept_fd < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
    }
    while (accept_fd) {
      usleep(100);
    }
  }

  for (int i = 0; i < MAX_EVENTS; ++i) {
    pthread_join(thread_array[i], NULL);
  }
}

int main(int c, char **v) {
  run();
  return 0;
}
