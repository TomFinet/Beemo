We opt to allocate request data dynamically for the following reasons:

- we can construct the request object once and refer to it by pointer for the lifetime
of the request, thus avoiding all copies.

- if we use automatic stack memory, we are forced to copy the data around to persist it
past its scope.


What IDIOM to use?

- Pimpl is an option but:
    - adds additional read due to pointer indirection when accessing private data. Is this true?
    surely: impl->x is one read at address of (impl) + offset(x)



Where to put the http message parsing code?

- since it is stateless, we structure it as functions in the http namespace.
- a header file with the forward declarations of these functions.
- an impl file with the definitions.
- server's implementation will import the header file.
- test's will import the header file. 
- the server header does not mention any of these internals.


A server must manage many connections at the same time.
It makes sense for it to store information about each connection:

- client socket file descriptor.
- connection status. 


Currently, we accept incoming connections and rx data into a receive buffer.
Then copy this receive buffer as a parameter to the request handler.

We actually want to receive the data into a request specific buffer which has the same lifetime
as the server object. Then we can just use string_view to point to it in all our request handling.
This way we have 0-copies made.


We also need a way for a user of this library to register handlers based on the uri path.


Currently, we accept a connection, receive from it once, and then close it I think.
We want to be able to keep connections open, and read from them whenever.
We want to be able to keep many connections open at once.