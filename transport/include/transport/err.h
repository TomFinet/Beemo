#pragma once


namespace beemo
{

    enum err_t {
        skt_reg_fail,
    };

    class transport_err {
        private:
            err_t code_;
        public:
            transport_err() = default;
            transport_err(err_t code) : code_(code) { }
            ~transport_err() = default;
    };

}