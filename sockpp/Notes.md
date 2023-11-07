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

