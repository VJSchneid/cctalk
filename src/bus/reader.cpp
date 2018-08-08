#include "reader.hpp"

#include <optional>
#include <cctalk/bus.hpp>

#include <iostream>

namespace cctalk::bus {
    Reader::Reader(unsigned char address, Callback callback)
            : address(address), callback(callback) {
    }

    void Reader::resize(std::size_t length) {
        buffer.resize(length);
    }

    void Reader::operator()(boost::system::error_code errorCode, std::size_t) {
        if (errorCode) {
            return callback(std::nullopt);
        }
        process();
    }

    void Reader::process() {
        if (correctDestination()) {

        } else {
            processNextMessage();
        }
    }

    boost::asio::mutable_buffer Reader::getBuffer() {
        return boost::asio::buffer(buffer.data(), buffer.size());
    }
}
