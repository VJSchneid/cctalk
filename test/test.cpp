#include <cctalk.hpp>
#include <iostream>

int main() {
    boost::asio::io_context ioContext;
    cctalk::Bus bus(ioContext);

    if (!bus.open("/dev/ttyUSB0")) {
        return 1;
    }

    cctalk::Bus::Command command;
    command.destination = 2;
    command.source = 1;
    command.header = cctalk::Bus::RESET_DEVICE;

    std::function<void (std::optional<cctalk::Bus::DataCommand>)> handler = [&] (std::optional<cctalk::Bus::DataCommand> data) {
        std::cout << "received something" << std::endl;
        std::cout.write(reinterpret_cast<char*>(data->data), data->length);
        std::cout << std::endl;

        bus.send(command);
        bus.receive(1, handler);
    };

    int money = 0;

    cctalk::CoinAcceptor coinAcceptor(bus);

    coinAcceptor.setCoinCallback([&money] (const cctalk::Coin &coin) {
        money += coin.getValue();
        std::cout << static_cast<float>(money) / 100 << std::endl;
    });

    coinAcceptor.enableCoin(cctalk::Coin(cctalk::Coin::makeCurrency("EU"), 0));

    coinAcceptor.open(2, [&coinAcceptor] (bool success) {
        if (success) {
            std::cout << "opened successfully!" << std::endl;
            coinAcceptor.start();
        } else {
            std::cout << "failed to open" << std::endl;
        }
    });

    ioContext.run();
}
