Socket IO with Winsock 2:
-------------------------

Blocking: IO operation returns when fully completed.
A thread can only handle one IO operation at a time.

Non-blocking: IO operation either completes immediately, or returns WOULD_BLOCK
indicating that the operation is not finished right away. To determine when
the operation completes, we can either poll using WSPSelect or register a callback.

Overlapping: IO operations return immediately. A value of 0 indicates that
the operation completed immediately, and that the event or completion routine ha
been queued using WPUQueueApc. A value of SOCKET_ERROR with error code WSA_IO_PENDING
indicates successful initiation of operation and that a completion indication will be queued
on completion. We don't want to use these though. We want to use completion ports.

CreateIoCompletionPort creates a completion port and associates one or more file handles
with that port.

Request handling threads should call the GetQueuedCompletionStatus function to read from the
IOCP queue.
We only want one IOCP and X worker threads.

---------------------------------------------

LINUX: epoll? great guide: https://copyconstruct.medium.com/the-method-to-epolls-madness-d9d2d6378642

stands for event poll.
purpose: multiplex I/O associated with file descriptors.

it is in fact a data-structure.
create the ds using epoll_create(int size) from sys/epoll.h
size is number of fd to multiplex, but is ignored because
it now grows dynamically.

can add to epoll ds by calling epoll_ctl(...)

when we accept a new connection, we want to add the socket fd
to epoll instance.

we can use this function to specify which events we want to
monitor, e.g. data rx'ed on fd => use EPOLLIN

a thread can be notified of events that happened on a monitored fd by calling epoll_wait(epfd, event_list, max_events, timeout)

