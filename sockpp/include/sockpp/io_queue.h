/* Class wrapping the IO queue used to send and receive packets.
   On windows this creates an IO completion port. */

#pragma once

#include <stdexcept>
#include <vector>
#include <memory>
#include <functional>
#include <thread>

#include <winsock2.h>
#include <Ws2tcpip.h>

#include <sockpp/io_ctx.h>
#include <sockpp/socket.h>

#include <iostream>

namespace sockpp
{

    /* A queue for all network IO requests and responses. We can add socket handlers
    whose requests and responses will be managed through this queue. Worker threads then
    listen for new IO and service them. The ctx_t gives the workers all the context needed to
    service the network IO. */
    template<typename key_t>
    class io_queue {
         
        private:

            typedef std::function<void(std::shared_ptr<key_t>, std::unique_ptr<io_ctx>)> io_cb_t;

            void *queue_handle_;

            io_cb_t on_io;
            std::function<void(std::shared_ptr<key_t>)> on_client_close;

        public:

            io_queue(unsigned int thread_num, io_cb_t on_io,
                     std::function<void(std::shared_ptr<key_t>)> on_client_close)
                : on_io(on_io), on_client_close(on_client_close)
            {
                /* create the io queue. */
                queue_handle_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, thread_num);
                if (queue_handle_ == nullptr) {
                    throw std::runtime_error("Failed to create io queue.");
                }
            }

            ~io_queue() { }

            /* add socket handle to IOCP so that we may send and receive its response/reqs to the IO queue. */
            void register_socket(const socket_t handle, const key_t *const key)
            {
                queue_handle_ = CreateIoCompletionPort((HANDLE)handle, queue_handle_, (DWORD_PTR)key, 0);
                if (queue_handle_ == nullptr) {
                    throw std::runtime_error("Failed to add socket to completion port.");
                }
            }

            /* listens for queued io, depending on the context calls on_rx or on_tx. */
            void listen(void)
            {
                unsigned long io_size = 0;
                key_t *key = nullptr;
                io_ctx *io = nullptr;

                while (true) {
                    /* attempt to dequeue an IO completion packet from the completion port. */
                    bool success = GetQueuedCompletionStatus(queue_handle_, &io_size, (PULONG_PTR)&key, (LPOVERLAPPED *)&io, INFINITE);
                    if (!success) {
                        std::cout << GetLastError() << std::endl;
                        throw std::runtime_error("Failed to dequeue from io queue.");
                    }

                    if (key == nullptr) {
                        return;
                    }

                    std::shared_ptr<key_t> key_ptr;
                    key_ptr.reset(key);

                    if (io_size == 0) {
                        /* client closed the connection. */
                        std::cout << "client closed the connection" << std::endl;
                        on_client_close(key_ptr);
                        continue;
                    }

                    std::unique_ptr<io_ctx> io_ctx;
                    io_ctx.reset(io);
                    on_io(key_ptr, std::move(io_ctx));
                }
            }
            
            void* get_handle(void)
            {
                return queue_handle_;
            }
    };

}