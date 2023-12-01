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


How to represent a resource?

First what sorts of resources: anything identified by uri, or pointed to by url.
- html, js, css
- images
- text
- in memory like a session perhaps.

We do not care really, so long as for each type of resource, we have a way to read/write to it.
We may also want to specify permissions for a resource type instance.

How to implement request timeouts?

When we first start reading request data in handle_rx, that is when we start a timer and schedule it to wake up in X ms from now.
When a request completes, we disarm the timer, and if the timeout occurs, we simply respond with a timeout error and close the connection.


At the OS level, sleep places our thread into the waiting queue. On system clock hardware interrupts, the schedule handler is called,
which must check the wait queue to see if the front thread has had its timeout point reached, in which case it is added to the ready queue.
At some future scheduling point, it will be scheduled and the thread will awaken from slumber.

In C++ we have the std::this_thread::sleep_for(std::chrono__milliseconds(x)) function which will make a sleep syscall to do this, in a cross-platform
way.

Why is sleep a syscall? Syscalls are used to escalate the priveledge level of the CPU in order to execute kernel priveledge instructions.
It sounds bad for a process to be able to move itself off the running queue, this should be a kernel op, hence the syscall. The user process could
move other processes around.