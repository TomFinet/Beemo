/* Class wrapping the IO queue used to send and receive packets.
   On windows this creates an IO completion port. */

#pragma once

#include <stdexcept>
#include <vector>
#include <memory>
#include <functional>

#include <winsock2.h>
#include <Ws2tcpip.h>

#include <sockpp/io_ctx.h>
#include <sockpp/socket.h>


namespace sockpp
{

    /* A queue for all network IO requests and responses. We can add socket handlers
    whose requests and responses will be managed through this queue. Worker threads then
    listen for new IO and service them. The ctx_t gives the workers all the context needed to
    service the network IO. */
    template<typename key_t>
    class io_queue {
         
        private:

            typedef std::function<void(std::shared_ptr<key_t>, std::shared_ptr<io_ctx>)> io_cb_t;

            HANDLE queue_handle_;

            /* stores the per io contexts. */
            std::vector<std::shared_ptr<io_ctx>> io_ctxs;

            /* whoever is using the io_queue will specify two callbacks for
            the case of the io queue holding a received IO data and sent IO data. */
            io_cb_t on_io;
            std::function<void()> on_client_close;

        public:

            /* the callbacks used to handle receiving and sending data. */
            io_queue(unsigned int thread_num, io_cb_t on_io,
                     std::function<void()> on_client_close)
                : on_io(on_io), on_client_close(on_client_close)
            {
                /* create the io queue. */
                queue_handle_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, thread_num);
                if (queue_handle_ == NULL) {
                    throw std::runtime_error("Failed to create io queue.");
                }
            }

            ~io_queue() { }

            /* add socket handle to IOCP so that we may send and receive its response/reqs to the IO queue. */
            void register_socket(const socket_t handle, const key_t *key)
            {
                queue_handle_ = CreateIoCompletionPort(reinterpret_cast<HANDLE>(handle), queue_handle_,
                                                       reinterpret_cast<ULONG_PTR>(key), 0);
                if (queue_handle_ == NULL) {
                    throw std::runtime_error("Failed to add socket to completion port.");
                }
            }

            std::shared_ptr<io_ctx> add_io_ctx(io::type type)
            {
                std::shared_ptr<io_ctx> ctx = std::make_shared<io_ctx>(type);
                io_ctxs.push_back(ctx);
                return ctx;
            }

            /* listens for queued io, depending on the context calls on_rx or on_tx. */
            void listen(void)
            {
                DWORD io_size = 0;
                std::shared_ptr<key_t> key;
                std::shared_ptr<io_ctx> ctx;
                

                while (true) {
                    /* attempt to dequeue an IO completion packet from the completion port. */
                    bool success = GetQueuedCompletionStatus(queue_handle_, &io_size, reinterpret_cast<PULONG_PTR>(key.get()),
                                                            reinterpret_cast<LPOVERLAPPED*>(ctx.get()), INFINITE);
                    if (!success) {
                        throw std::runtime_error("Failed to dequeue from io queue.");
                    }

                    if (key == nullptr) {
                        return;
                    }

                    if (io_size == 0) {
                        /* client closed the connection. */
                        on_client_close();
                        continue;
                    }

                    on_io(key, ctx);
                }
            }
    };

}