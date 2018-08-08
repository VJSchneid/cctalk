#include <cctalk.hpp>
#include <iostream>

int main() {
    boost::asio::io_context ioContext;
    cctalk::Bus bus(ioContext);

    if (!bus.open("/dev/ttyUSB0")) {
        return 1;
    }

    if (!bus) {
        return 2;
    }

    cctalk::Bus::Command command;
    command.destination = 2;
    command.source = 1;
    command.header = cctalk::Bus::REQUEST_EQUIPMENT_CATEGORY_ID;

    bus.send(command);

    bus.receive(1, [] (std::optional<cctalk::Bus::DataCommand> dataCommand) {
        std::cout << dataCommand->data << std::endl;
    });

    ioContext.run();
}
