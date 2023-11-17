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


