# CCTalk

Library to talk with CCTalk compatible devices like CoinAcceptors.

## Dependencies

 * CMake at least 3.8.2
 * C++17 compatible compiler
 * boost::system at least 1.62
 * boost::asio at least 1.62 (with serial support)

## Build steps

```bash

# create build folder
mkdir build && cd build

# generate makefiles
cmake ..

# compile cctalk
cmake --build .

```

# Bus example

```cpp
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
    command.header = cctalk::Bus::REQUEST_EQUIPMENT_CATEGORY_ID;

    bus.send(command);

    bus.receive(2, [] (std::optional<cctalk::Bus::DataCommand> dataCommand) {
        std::cout << "device type: ";
        std::cout.write(reinterpret_cast<char*>(dataCommand->data), dataCommand->length);
        std::cout << std::endl;
    });

    ioContext.run();
}

```
