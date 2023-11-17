We opt to allocate request data dynamically for the following reasons:

- we can construct the request object once and refer to it by pointer for the lifetime
of the request, thus avoiding all copies.

- if we use automatic stack memory, we are forced to copy the data around to persist it
past its scope.


HTTP responses:

- include Content-Length field to indicate the size of the response body.
- 


The memory for a request object should exist within the connection object.
We use it to store parsed requests, and once done with one, re-use it for any other potential requests.


TODO:
-----

Timeouts
Blacklist


If we handle request errors (bad_requests/semantics) with exceptions, we hit the slow exception path
for badly formed requests. We want request error handling to be fast, so we should favour early returning
with an error return value.

can have a

struct http_err {

    code_t code;
    action_t action;

}

as return value.

handle_rx ==calls==> parse_headers

if parse headers returns nullptr, then no errors.
else handle_rx calls an error handling method to handle the error appropriately. Plus we want to handle
the error immediately, without needing to pass the error up the call stack. Thus exceptions are avoided in favour of returning errors as part of the request object metadata.

instead of http error object with a field for the error type and action to take,
where a large switch statement would be needed for each of the field values, we make a class hierarchy
of error objects each overriding the handle method. If no error exists, the req error is 


Today: work on transfer-encoding and content-length.
