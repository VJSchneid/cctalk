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

    bool Bus::receive(unsigned char destination, CommandCallback callback) {
        if (isReading) {
            return false;
        }
        isReading = true;
        readCallback = std::move(callback);
        address = destination;

        startReading();

        return true;
    }

    void Bus::startReading() {
        std::cout << "start reading" << std::endl;
        using boost::asio::transfer_all;

        readBuffer.resize(CCTALK_SIZE_WITHOUT_DATA);

        auto bufferWrap = boost::asio::buffer(readBuffer.data(), readBuffer.size());
        boost::asio::async_read(serialPort, std::move(bufferWrap), transfer_all(),
                                [this] (boost::system::error_code errorCode, std::size_t) {
            if (errorCode) {
                return endReading(std::nullopt);
            }
            processReceived();
        });
    }

    void Bus::endReading(std::optional<DataCommand> data) {
        readCallback(std::move(data));
        isReading = false;
    }

    void Bus::processReceived() {
        std::cout << "processReceived" << std::endl;
        auto header = reinterpret_cast<const MessageHeader*>(readBuffer.data());
        if (isCorrectDestination(*header)) {
            if (header->dataLength != 0) {
                readMissing(*header);
            } else {
                receivedAll();
            }
        } else {
            readNextMessage(*header);
        }
    }

    bool Bus::isCorrectDestination(const MessageHeader &header) {
        std::cout << "destination: " << (int)header.destination << " == " << (int)address << std::endl;
        return header.destination == address;
    }

    void Bus::readMissing(const MessageHeader &header) {
        std::cout << "read missing" << std::endl;
        using boost::asio::transfer_all;

        auto bufferPos = readBuffer.size();

        // Warning: header not usable below this line:
        readBuffer.resize(CCTALK_SIZE_WITHOUT_DATA + header.dataLength);

        auto bufferWrap = boost::asio::buffer(readBuffer.data() + bufferPos,
                                              readBuffer.size() - bufferPos);

        boost::asio::async_read(serialPort, std::move(bufferWrap), transfer_all(),
                                [this, header] (boost::system::error_code errorCode, std::size_t) {
            if (errorCode) {
                return endReading(std::nullopt);
            }
            receivedAll();
        });
    }

    void Bus::receivedAll() {
        auto header = reinterpret_cast<const MessageHeader*>(readBuffer.data());
        DataCommand command;
        command.destination = header->destination;
        command.length = header->dataLength;
        command.source = header->source;
        command.header = static_cast<HeaderCode>(header->headerCode);
        command.data = readBuffer.data() + CCTALK_SIZE_WITHOUT_DATA - 1;
        readCallback(std::move(command));
        isReading = false;
    }

    void Bus::readNextMessage(const MessageHeader &header) {
        using boost::asio::transfer_all;

        auto bufferPos = readBuffer.size();

        // Warning: header not usable below this line:
        readBuffer.resize(CCTALK_SIZE_WITHOUT_DATA + header.dataLength);

        auto bufferWrap = boost::asio::buffer(readBuffer.data() + bufferPos,
                                              readBuffer.size() - bufferPos);

        boost::asio::async_read(serialPort, std::move(bufferWrap), transfer_all(),
                                [this, header] (boost::system::error_code errorCode, std::size_t) {
            if (errorCode) {
                return endReading(std::nullopt);
            }
            startReading();
        });
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
        unsigned int sum = 0;
        for (auto &element: buffer) {
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

    void Bus::configure() {
        using boost::asio::serial_port_base;
        serialPort.set_option(serial_port_base::baud_rate(9600));
        serialPort.set_option(serial_port_base::character_size(8));
        serialPort.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
        serialPort.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
        serialPort.set_option(serial_port_base::parity(serial_port_base::parity::none));
    }
}
