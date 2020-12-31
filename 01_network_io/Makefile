TARGETS=http-client multi-process-server multi-thread-server select-server epoll-server  low-libevent-server high-libevent-server
all: $(TARGETS)

clean:
	@rm -fr $(TARGETS)

.PHONY: all clean

http-client: stage_0.cpp
	g++ -g -Wall "$<" -o $@

multi-process-server: stage_1.cpp
	g++ -g -Wall "$<" -o $@

multi-thread-server: stage_2.cpp
	g++ -g -Wall "$<" -pthread -o $@

select-server: stage_3.cpp
	g++ -g -Wall "$<" -o $@

epoll-server: stage_4.cpp
	g++ -g -Wall "$<" -o $@

low-libevent-server: stage_5.cpp
	g++ -g -Wall "$<" -levent -o $@

high-libevent-server: stage_6.cpp
	g++ -g -Wall "$<" -levent -o $@
