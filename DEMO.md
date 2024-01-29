Hi, I am going to be demo-ing Beemo, a HTTP/1.1 web server.

First, let me give you some idea of how performant it is.
Running wrk with 200 concurrent connections for 2 mins
pinging the web server which responded with 64B of data,
resulted in approx 60 thousand requests per second,
with a 1.8ms average latency,
which checks out to a throughput of 3.6 MB/s.
NGINX comparatively achieves , with the same setup,
100K req/s, with average latency 1ms and a 22 MB/s throughput.
So NGINX is about 2x more performant than Beemo V0.1 .

Let's examine Beemo's design. The system is composed of 4 "modules".
The transport module just knows how to send and receive packets over the network.
The events module exposes an API for registering and being notified of file events.
The http module looks after tracking the state of http connections, parsing complete http
packets and routing parsed requests to the correct handler.
All OS dependent source is placed in either the Linux or Windows folders.
The build system selects the correct OS sources depending on the platform being built for.

The server is multithreaded, not in the sense that every connection is given to a thread,
either freshly made or from a threadpool, but in the sense that every thread except T0
runs an event loop.
The job of T0 is to start the server, and accept incoming connections.
When a connection is made, T0 registers the IN event with that connection.
When an IN event occurs on that connection, exactly ONE worker thread receives it
in its queue of triggered events.
At the same time, this connection deregisters its interest in the IN event
to prevent a later in event from being given to a different worker thread.
The worker then calls the event handler associated with the IN event,
and once done, the http layer is notified of the event's completion.
The http layer uses the state of the connection
after the event to decide what event, if any to register next.

You may be asking yourself: why use an event loop?

We could have used a threadpool where each thread handles one connection at a time,
but this severely limits the number of concurrent connections we can tolerate.
Furthermore, if a connection blocks, the thread is just left waiting for IO.

With an event loop, as soon as a connection blocks, the worker moves on to the next
connection that is ready, that way we swap in and swap out connections as and when they
are ready for the CPU. This means each worker can handle hundreds or thousands of
concurrent connections.

Beemo uses a multithreaded event loop, getting the best of both worlds.

That's all for now. For access to the source code, check out the github repo.
Thanks and bye.

A demo
-------

