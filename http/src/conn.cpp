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
}