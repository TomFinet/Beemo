Specific Tasks:
---------------

- integrate configuration struct into server.
- write request semantic validation.
- add timeout to request handlers.
- add linux support to sockpp. (HIGH PRIORITY)
- fix uri error propagation (used to be using exceptions, now using err handler field of req struct).
  also update uri tests to no longer expect exceptions thrown.
- support compress, deflate, identity and gzip encodings.
- write test to stress number of concurrent connections.

General Improvements:
---------------------

- write more expansive tests.
- refactor code (lot's of raw pointers used, probably lots of constexpr to be added, other modern c++ improvements).
- profile the code.
- add logging to ease debugging.
- remove useless comments.