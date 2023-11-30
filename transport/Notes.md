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

We can have edge triggered or level triggered event notifications.

EPOLLONESHOT flag deregisters a fd when epoll_wait returns an even associated with it.
To write an IO class that is compatible with windows IOCP, we can use:

- epoll_ctl as a read and write request.
- epoll_wait as a dequeue, combined with EPOLLONESHOT to avoid packets for a single connection being spread over multiple threads simultaneously.

We can use edge triggered events to help prevent data


We don't want lower level modules i.e. sockpp to depend on how the higher levels use it.
Otherwise, we lose the power of modularity and reuse. It is fine for higher levels to depend on lower levels
however, since they by definition build on the lower layer apis.

----------------------------------------------------

Issue with unifying the windows and linux transport layers is that linux uses an
"on ready" model and windows uses an "on complete" model of notifying events.

In http/server.cpp, handle_rx/handle_tx are called when socket io completes receiving/transmitting.

At the moment, connection->rx() means make a request for rx io, when this request completes handle_rx is called.

But on linux connection->rx() should ask to be notified when a socket file descriptor is ready for reading, and in the io_queue we would actually do the rx read.

What we could do to unify these is implement two seperate transport server classes, one for windows and one for linux.

Linux server would have the epoll instance as a member, and windows server would have the iocp queue as a member.

The http server would then make rx requests by calling the respective platform server rx method.
On windows it would call the connections skt->rx method, and on linux it would call the server->rx method which would register the socket on the epoll instance.

The linux server would launch a bunch of threads waiting on io readiness, and the windows server the same but for completedness.
Either way the threads on both plaform servers would call the http server handle_rx/handle_tx methods with the completed io operation.



The http module calls conn_ctx->rx/conn_ctx->tx to start a recv/send.
conn_ctx->rx calls the transport server to actually recv/send the message.














