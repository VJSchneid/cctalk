#include <cctalk/bus.hpp>
#include <iostream>

namespace cctalk {
    Bus::Bus(boost::asio::io_context &ioContext): serialPort(ioContext) {
        isReading = false;
    }

    bool Bus::open(const char *path) {
        boost::system::error_code errorCode;
        serialPort.open(path, errorCode);
        if (errorCode) {
            return false;
        }
        configure();
        return true;
    }

    void Bus::configure() {
        using boost::asio::serial_port_base;
        serialPort.set_option(serial_port_base::baud_rate(9600));
        serialPort.set_option(serial_port_base::character_size(8));
        serialPort.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
        serialPort.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
        serialPort.set_option(serial_port_base::parity(serial_port_base::parity::none));
    }

    bool Bus::ready() {
        return serialPort.is_open();
    }

    Bus::operator bool() {
        return ready();
    }

    void Bus::send(Command command, std::function<void (bool)> callback) {
        auto buffer = createMessage(std::move(command));
        write(std::move(buffer), std::move(callback));
    }

    void Bus::send(DataCommand command, std::function<void (bool)> callback) {
        auto buffer = createMessage(std::move(command));
        write(std::move(buffer), std::move(callback));
    }

    Bus::Buffer Bus::createMessage(const Command &&command) {
        Buffer buffer;
        buffer.reserve(CCTALK_SIZE_WITHOUT_DATA);
        addFrontMessage(buffer, command);
        addChecksum(buffer);
        return buffer;
    }

    Bus::Buffer Bus::createMessage(const DataCommand &&dataCommand) {
        Buffer buffer;
        buffer.reserve(CCTALK_SIZE_WITHOUT_DATA + dataCommand.length);
        addFrontMessage(buffer, dataCommand, dataCommand.length);
        addDataMessage(buffer, dataCommand);
        addChecksum(buffer);
        return buffer;
    }

    void Bus::addFrontMessage(Buffer &buffer,
                              const Command &command,
                              unsigned char dataLength) {
        buffer.emplace_back(command.destination);
        buffer.emplace_back(dataLength);
        buffer.emplace_back(command.source);
        buffer.emplace_back(command.header);
    }

    void Bus::addDataMessage(Buffer &buffer,
                             const DataCommand &dataCommand) {
        auto dataBegin = buffer.size();
        buffer.resize(buffer.size() + dataCommand.length);
        std::memcpy(buffer.data() + dataBegin, dataCommand.data, dataCommand.length);
    }

    void Bus::addChecksum(Buffer &buffer) {
        unsigned char sum = 0;
        for (auto &element: buffer) {
            std::cout << (int)element << std::endl;
            sum += element;
        }
        buffer.emplace_back((~sum) + 1);
    }

    void Bus::write(Buffer &&buffer, std::function<void (bool)> &&callback) {
        using boost::asio::transfer_all;

        auto bufferWrap = boost::asio::buffer(buffer.data(), buffer.size());
        auto handler = [cache = std::move(buffer), callback = std::move(callback)]
                       (boost::system::error_code errorCode, std::size_t) {
            if (callback) {
                callback(!errorCode);
            }
        };

        boost::asio::async_write(serialPort, std::move(bufferWrap), transfer_all(), std::move(handler));
    }

    void Bus::receive(unsigned char destination, CommandCallback callback) {
        if (!callback) {
            throw std::logic_error("invalid callback passed");
        }

        addReadCallback(std::make_pair(std::move(destination), callback));
        auto expectedReadingState = false;
        if (isReading.compare_exchange_strong(expectedReadingState, true)) {
            startReading();
        }
    }

    void Bus::addReadCallback(std::pair<unsigned char, CommandCallback> callback) {
        std::lock_guard lock(readCallbackMutex);
        readCallbacks.emplace_back(std::move(callback));
    }

    void Bus::startReading() {
        using boost::asio::transfer_all;

        readBuffer.resize(CCTALK_SIZE_WITHOUT_DATA);

        auto bufferWrap = boost::asio::buffer(readBuffer.data(), readBuffer.size());
        boost::asio::async_read(serialPort, std::move(bufferWrap), transfer_all(),
                                [this] (boost::system::error_code errorCode, std::size_t) {
            if (errorCode) {
                return cancelReading();
            }
            processReceived();
        });
    }

    void Bus::cancelReading() {
        std::lock_guard lock(readCallbackMutex);
        for (auto &callback: readCallbacks) {
            callback.second(std::nullopt);
        }
        readCallbacks.clear();
        isReading = false;
    }

    void Bus::processReceived() {
        auto header = reinterpret_cast<const MessageHeader*>(readBuffer.data());
        if (header->dataLength == 0) {
            receivedAll();
        } else {
            readMissing(header->dataLength);
        }
    }

    void Bus::readMissing(const unsigned char dataLength) {
        using boost::asio::transfer_all;

        auto bufferPos = readBuffer.size();

        readBuffer.resize(CCTALK_SIZE_WITHOUT_DATA + dataLength);

        auto bufferWrap = boost::asio::buffer(readBuffer.data() + bufferPos,
                                              readBuffer.size() - bufferPos);

        boost::asio::async_read(serialPort, std::move(bufferWrap), transfer_all(),
                                [this] (boost::system::error_code errorCode, std::size_t) {
            if (errorCode) {
                return cancelReading();
            }
            receivedAll();
        });
    }

    void Bus::receivedAll() {
        callReadCallback();

        std::lock_guard lock(readCallbackMutex);
        if (!readCallbacks.empty()) {
            startReading();
        }
    }

    void Bus::callReadCallback() {
        auto header = reinterpret_cast<const MessageHeader*>(readBuffer.data());

        if (auto callback = popCorrespondingReadCallback(header->destination)) {
            DataCommand command;
            command.destination = header->destination;
            command.source = header->source;
            command.header = static_cast<HeaderCode>(header->headerCode);

            command.length = header->dataLength;
            command.data = readBuffer.data() + CCTALK_SIZE_WITHOUT_DATA - 1;

            (*callback)(command);
        }
    }

    std::optional<Bus::CommandCallback> Bus::popCorrespondingReadCallback(const unsigned char destination) {
        std::lock_guard lock(readCallbackMutex);

        auto iterator = std::find_if(readCallbacks.begin(),
                        readCallbacks.end(),
                        [destination] (const std::pair<unsigned char, CommandCallback> &callback) {
            return callback.first == destination;
        });

        if (iterator != readCallbacks.end()) {
            CommandCallback callback = std::move(iterator->second);
            readCallbacks.erase(iterator);

            return std::move(callback);
        }
        return std::nullopt;
    }
}
