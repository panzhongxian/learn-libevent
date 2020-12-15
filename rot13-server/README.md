# Different Implementations of ROT13 Server

## Compilation

```bash
make
```

## Stage descriptions

### Stage 0

A synchrony http client.

### Stage 1

A multiple-processing server using `fork`.

### Stage 2

A multiple-thread server using `pthread_create`.

### Stage 3

A async server using `select`.

### Stage 4

A asynchronous server using `epoll`.

### Stage 5

A asynchronous server using low-level Libevent API.

### Stage 6

A asynchronous server using high-level Libevent API.
