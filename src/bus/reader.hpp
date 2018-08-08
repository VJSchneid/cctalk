#pragma once

#include <boost/asio.hpp>

namespace cctalk::bus {

    struct DataCommand;

    class Reader {
        typedef std::function<void (std::optional<DataCommand>)> Callback;


    public:
        Reader(unsigned char address, Callback callback);

        void resize(std::size_t length);

        void operator ()(boost::system::error_code errorCode, std::size_t);

        boost::asio::mutable_buffer getBuffer();

    private:

        void process();
        bool correctDestination();
        void processNextMessage();

        unsigned char address;
        Callback callback;
        std::vector<unsigned char> buffer;
    };
}
