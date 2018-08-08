#pragma once

#include <type_traits>
#include <functional>
#include <vector>
#include <cstdint>
#include <boost/asio.hpp>

namespace cctalk {

    class Bus {
        typedef std::vector<unsigned char> Buffer;

        struct MessageHeader {
            unsigned char destination;
            unsigned char dataLength;
            unsigned char source;
            unsigned char headerCode;
        } __attribute__((packed));

        enum {CCTALK_SIZE_WITHOUT_DATA = 5};

    public:
        enum HeaderCode {
            RESET_DEVICE = 1,
            REQUEST_COIN_ID = 184,
            MODIFY_INHIBIT_STATUS = 231,
            PERFORM_SELF_TEST = 232,
            REQUEST_SOFTWARE_VERSION = 241,
            REQUEST_SERIAL_NUMBER = 242,
            REQUEST_PRODUCT_CODE = 244,
            REQUEST_EQUIPMENT_CATEGORY_ID = 245
        };

        struct Command {
            unsigned char destination;
            unsigned char source;
            HeaderCode header;
        };

        struct DataCommand: Command {
            unsigned char *data;
            unsigned char length;
        };

        typedef std::function<void (std::optional<DataCommand> command)> CommandCallback;

        Bus(boost::asio::io_context &ioContext);
        //Bus(const char *path);
        //Bus(libusb_device_handle *device);

        bool open(const char *path);

        bool ready();
        operator bool();

        void send(const Command command, std::function<void (bool)> callback = std::function<void (bool)>());
        void send(const DataCommand command, std::function<void (bool)> callback = std::function<void (bool)>());

        bool receive(unsigned char destination, CommandCallback function);

    private:
        static Buffer createMessage(const Command &&command);
        static Buffer createMessage(const DataCommand &&command);
        static void addFrontMessage(Buffer &buffer,
                                    const Command &command,
                                    unsigned char dataLength = 0);
        static void addDataMessage(Buffer &buffer,
                                   const DataCommand &dataCommand);
        static void addChecksum(Buffer &buffer);

        void startReading();
        void endReading(std::optional<DataCommand> data);
        void processReceived();
        bool isCorrectDestination(const MessageHeader &header);
        void readMissing(const MessageHeader &header);
        void readNextMessage(const MessageHeader &header);
        void receivedAll();

        void write(Buffer &&buffer, std::function<void (bool)> &&callback);
        inline void configure();

        boost::asio::serial_port serialPort;

        std::atomic_bool isReading;
        CommandCallback readCallback;
        unsigned char address;
        std::vector<unsigned char> readBuffer;
    };

}
