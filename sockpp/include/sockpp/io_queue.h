/* Class wrapping the IO queue used to send and receive packets.
   On windows this creates an IO completion port. */

#pragma once

#include <stdexcept>
#include <vector>
#include <memory>
#include <functional>
#include <thread>

#include <sockpp/platform.h>
#include <sockpp/conn_ctx.h>
#include <sockpp/io_ctx.h>
#include <sockpp/socket.h>


namespace sockpp
{

    using io_cb_t = std::function<void(socket_t)>;

    /* A queue for all network IO requests and responses. We can add socket handlers
    whose requests and responses will be managed through this queue. Worker threads then
    listen for new IO and service them. The ctx_t gives the workers all the context needed to
    service the network IO. */
    class io_queue {
         
        private:
            io_cb_t on_rx;
            io_cb_t on_tx;
            io_cb_t on_client_close;
            io_queue_t queue_handle_;

        public:

            io_queue(io_cb_t on_rx, io_cb_t on_tx, io_cb_t on_client_close);
            ~io_queue() { }

            void register_socket(const socket_t handle, const conn_ctx *const conn); 
            void dequeue(void);
            
            io_queue_t get_handle(void)
            {
                return queue_handle_;
            }
    };

}