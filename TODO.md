In Progress:
------------

- add request handler api.
- add linux support to sockpp. (HIGH PRIORITY)
- integrate configuration struct into server. (IN PROGRESS)


Specific Tasks:
---------------

- add timeout to request handlers.
- fix uri error propagation (used to be using exceptions, now using err handler field of req struct).
  also update uri tests to no longer expect exceptions thrown.
- support compress, deflate, identity and gzip encodings.
- write test to stress number of concurrent connections.
- write response builder methods.
- support more http headers.

General Improvements:
---------------------

- write more expansive tests.
- refactor code (lot's of raw pointers used, probably lots of constexpr to be added, other modern c++ improvements).
- profile the code.
- remove useless comments.
- improve code consistency: use struct keyword, end member variables with _ .

Potential Bugs:
---------------

- we flush the conn_ctx reassembly buffer before sending a response, when
  really we should only be flushing up to where the request ended. Pipelined requests
  may otherwise be flushed. 
- io_ctx memory leaks due to manually managed allocation with `new`.