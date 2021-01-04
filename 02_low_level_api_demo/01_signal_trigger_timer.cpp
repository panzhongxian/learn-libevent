#include <event2/event.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

static int n_calls = 0;

void PeriodTask(evutil_socket_t fd, short what, void* arg) {
  struct event* me = (struct event*)arg;
  printf("cb_func called %d times so far.\n", ++n_calls);
  if (n_calls > 5) {
    event_del(me);
  }
}

void TimingTask(evutil_socket_t fd, short what, void* arg) {
  struct event* me = (struct event*)arg;
  kill(getpid(), 10);
  evtimer_del(me);
  // event_base_loopbreak(event_get_base(me));
}

void UsrSigCb(evutil_socket_t fd, short what, void* arg) {
  printf("get signal SIGUSR1\n");
  struct event* me = (struct event*)arg;
  evsignal_del(me);
}

void Run() {
  struct event_base* base = event_base_new();
  struct timeval one_sec = {1, 0};
  struct timeval three_sec = {3, 500000};
  struct event* ev;

  // print per second.
  ev = event_new(base, -1, EV_PERSIST, PeriodTask, event_self_cbarg());
  event_add(ev, &one_sec);

  // send signal to self after 5.5 seconds.
  ev = evtimer_new(base, TimingTask, event_self_cbarg());
  evtimer_add(ev, &three_sec);

  // proc SIGUSR1 signal
  ev = evsignal_new(base, SIGUSR1, UsrSigCb, event_self_cbarg());
  evsignal_add(ev, NULL);

  event_base_dump_events(base, stderr);
  printf("------\n");
  event_base_dispatch(base);
  printf("------\n");
  event_base_dump_events(base, stderr);
}

int main() {
  Run();
  return 0;
}
