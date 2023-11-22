*Transfer-Encoding:* encoding applied to the content to get the message body.
Chunked is used when the content length is not known ahead of time, and gzip is
a compressed encoding (we will not want to manually decompress, so use a lib).

*Content-Length:* 



need to add the request to the connection metadata.



Transfer-encoding: used for delimiting the end of a dynamically generated content.
If it is absent, then the content-length indicates the number of bytes of content.
Transfer encoding distinguish between encodings applied in transfer, and encodings that are
part of their representation.

Server must support chunked transfer encoding.
We will also support gzip encoding by means of an external library.

When we detect an error in the http message, we have to read in the rest of the message,
with a do nothing parse, and once done, return. That way when we are done with the
erroneous message we can cleanly start reading the next.

How to know when erroneous message ends? Maybe when the next request line is encountered.
Actually, we can't use that since it may be part of the payload data. The server should
simply close the connection after sending a 400 bad request.

Today, we focus on parsing the message body from content-length and transfer-encoding.


---------------------------------------

Today, we want to design and start work on enabling a user of this http server to add custom
logic to endpoints.
We also want to implement some checks and semantics for the various http methods.

We probably want a configuration structure. That way we can use it during msg routing and such.