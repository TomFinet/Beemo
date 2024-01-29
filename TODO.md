In Progress:
------------

- write response builder methods, maybe for each request method we have a response builder.
- add linux support to sockpp. (HIGH PRIORITY). Make linux sockets non-blocking, since we will be calling them only when they are ready.


Specific Tasks:
---------------

- use configuration file. avoids having to recompile when values are changed.
- add a connection idle timeout which when triggered closes the connection.
- support compress, deflate, identity and gzip encodings.
- write test to stress number of concurrent connections.
- support more http headers.
- Test sending a response which overflows an io_ctx buffer.

General Improvements:
---------------------

- write more expansive tests.
- refactor code (lot's of raw pointers used, probably lots of constexpr to be added, other modern c++ improvements).
- profile the code.
- improve code consistency: use struct keyword, end member variables with _ .
- add clang tidy tools to format the code.

Potential Bugs:
---------------

- we flush the conn_ctx reassembly buffer before sending a response, when
  really we should only be flushing up to where the request ended. Pipelined requests
  may otherwise be flushed. 
- io_ctx memory leaks due to manually managed allocation with `new`.


Done:
-----

- split configuration struct into transport and http configs.
- add timeout to request handlers.
- fix uri error propagation (used to be using exceptions, now using err handler field of req struct).


PROBLEMS:
---------

Not all connections are being closed. WHY?

Maybe unrelated, but we store the transport connection pointer in two unordered_maps:
1. The transport server's table of skt handles to connections
2. The http server's table of skt handles to http connections,
   since a http connection has a pointer to the underlying transport connection.

Can't we just store it in the http server's map? Actually this is fine.


BENCHMARKS:
-----------

1. Number of requests per second.
2. Latency of a request.
3. Throughput of server in bytes per second.

It seems that the wrk project is well suited for creating
large concurrent workloads.


In edge triggered mode, we only get notified of events if the state goes from
non-ready to ready. If we don't read/write all data to a socket while it is ready,
such that it goes to non-ready, then it will stay ready and not show up in subsequent
epoll_wait calls.


The server should be the sole manager of a connections lifetime.
So shared_ptr with weak_ptrs

What if the server crashes?

Add a feature to dynamically start a backup process running the server on crash event.