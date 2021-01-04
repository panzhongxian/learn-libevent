## Stage 1: Timing event, period event and signal event

There are 3 tasks:

1. PeriodTask: print one line per second for five times.
2. TimingTask: send `SIGUSR1` signal to the process self.
3. SignalHandler: a callback will be triggered when recieve `SIGUSR1` signal

## Stage 2: `connect()`

Create multiple connections(>1000) to hosts with non-block socket.

## Stage 3: `send()` and `recv()`

One `send()` and one `recv()` on multiple non-block socket connections.

