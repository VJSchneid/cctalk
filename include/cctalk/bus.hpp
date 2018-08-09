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

        bool open(const char *path);

        bool ready();
        operator bool();

        void send(const Command command, std::function<void (bool)> callback = std::function<void (bool)>());
        void send(const DataCommand command, std::function<void (bool)> callback = std::function<void (bool)>());

        void receive(unsigned char destination, CommandCallback function);

    private:
        static Buffer createMessage(const Command &&command);
        static Buffer createMessage(const DataCommand &&command);
        static void addFrontMessage(Buffer &buffer,
                                    const Command &command,
                                    unsigned char dataLength = 0);
        static void addDataMessage(Buffer &buffer,
                                   const DataCommand &dataCommand);
        static void addChecksum(Buffer &buffer);


        void addReadCallback(std::pair<unsigned char, CommandCallback> callback);
        void startReading();
        void cancelReading();
        void processReceived();
        std::optional<CommandCallback> popCorrespondingReadCallback(const unsigned char destination);
        void readMissing(const unsigned char dataLength);
        void receivedAll();
        void callReadCallback();

        void write(Buffer &&buffer, std::function<void (bool)> &&callback);
        inline void configure();

        boost::asio::serial_port serialPort;

        std::mutex readCallbackMutex;
        std::atomic_bool isReading;
        std::vector<std::pair<unsigned char, CommandCallback>> readCallbacks;
        std::vector<unsigned char> readBuffer;
    };

}
