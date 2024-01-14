#include <http/conn.h>

namespace beemo
{
    conn::conn(const socket_t sktfd) :
        status_(conn_keep_alive),
        parsed_to_idx_(0),
        skt_(std::make_unique<socket>(sktfd)),
        io_tx_(std::make_unique<io_buf>(TX)),
        req_(std::make_unique<req>()),
        resp_(std::make_unique<resp>()) { }

    void conn::reset_state(void)
    {
        parsed_to_idx_ = 0;
        std::unique_ptr<req> clear_req = std::make_unique<req>();
        std::unique_ptr<resp> clear_resp = std::make_unique<resp>();
        req_.swap(clear_req);
        resp_.swap(clear_resp);
    }
    
    void conn::prepare_tx(void)
    {
        prepare_tx(resp_->to_str());
    }

    void conn::prepare_tx(std::string_view tx)
    {
        if (tx.size() > io_tx_->buf_len_) {
            io_tx_ = std::make_unique<io_buf>(TX, tx.size());
        }
        io_tx_->write_buf(tx);
    }
    
    io_status conn::do_rx(void)
    {
        for (;;) {
            std::unique_ptr<io_buf> rx_io = std::make_unique<io_buf>(RX);
            while (rx_io->bytes_rx < rx_io->buf_len_) {
                int nbytes = skt_->rx(rx_io.get(), 1);

                if (nbytes == socket_error) {
                    if (skt_->would_block()) {
                        io_rx_.append(rx_io->buf, rx_io->bytes_rx);
                        return PARTIAL;
                    }
                    return ERROR;
                }

                if (nbytes == 0) {
                    io_rx_.append(rx_io->buf, rx_io->bytes_rx);
                    status_ &= ~conn_keep_alive;
                    return COMPLETE;
                }
                rx_io->bytes_rx += nbytes;
            }
            io_rx_.append(rx_io->buf, rx_io->bytes_rx);
        }
    }

    io_status conn::do_tx(void)
    {
        while (io_tx_->bytes_tx < io_tx_->bytes_to_tx) {
            int nbytes = skt_->tx(io_tx_.get(), 1); 
            if (nbytes == socket_error) {
                if (skt_->would_block()) {
                    return PARTIAL;
                }
                return ERROR;
            }
            io_tx_->bytes_tx += nbytes;
        }
        return COMPLETE;
    }
}