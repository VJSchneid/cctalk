# CCTalk

Library to talk with CCTalk compatible devices like CoinAcceptors.

## Dependencies

 * CMake at least 3.8.2
 * C++17 compatible compiler
 * boost::system at least 1.62
 * boost::asio at least 1.62

## Build steps

``` bash

# create build folder
mkdir build && cd build

# generate makefiles
cmake ..

# compile cctalk
cmake --build . -- -j9

```
