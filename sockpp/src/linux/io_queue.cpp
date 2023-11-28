#include <sockpp/io_queue.h>

#include <sys/epoll.h>

namespace sockpp
{

    constexpr int max_epoll_events = 1000;
    constexpr int block_indefinitely = -1;

    io_queue::io_queue(io_cb_t on_rx, io_cb_t on_tx, io_cb_t on_client_close)
        : on_rx(on_rx), on_tx(on_tx), on_client_close(on_client_close)
    {
        /* create the epoll instance. */
        queue_handle_ = epoll_create1(EPOLL_CLOEXEC); 
        if (queue_handle_ == invalid_handle) {
            throw std::runtime_error("Failed to create epoll instance.");
        }
    }

    /* TODO: figure out epoll_event struct. */
    void io_queue::register_socket(const socket_t handle, const conn_ctx *const conn)
    {
        int err = epoll_ctl(queue_handle_, EPOLL_CTL_ADD, handle, );
        if (err == socket_error) {
            throw std::runtime_error("Failed to register a socket to this epoll instance.");
        }
    }

    void io_queue::dequeue(void)
    {
        while (true) {
            // an event loop calling epoll_wait every iteration.
            int nready = epoll_wait(queue_handle_, events, max_epoll_events, block_indefinitely);
            for (int i = 0; i < nready; i++) {
                // use events[i] to get a hold of the event.
            }
        }
    }
}