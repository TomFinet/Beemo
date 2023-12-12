In Progress:
------------

- improve routing file.
- write response builder methods, maybe for each request method we have a response builder.
- add linux support to sockpp. (HIGH PRIORITY). Make linux sockets non-blocking, since we will be calling them only when they are ready.


Specific Tasks:
---------------

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