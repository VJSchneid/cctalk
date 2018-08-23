#pragma once

#include "bus.hpp"
#include "coin.hpp"

namespace cctalk {
    class CoinAcceptor {
    public:
        CoinAcceptor(cctalk::Bus &bus, unsigned char sourceAddress = 1);

        void open(unsigned char address, std::function<void (bool)> callback);

        bool ready();
        operator bool();

        void disableCoin(const Coin &coin);
        void enableCoin(const Coin &coin);
        bool isWantedCoin(const Coin &coin);

        void start();
        void stop();

        void setCoinCallback(const std::function<void (const Coin &)>);

    private:
        void initialize(const std::function<void (bool)> &&callback);
        void validateEquipmentCategory(const std::function<void (bool)> &&callback);
        void initializeSupportedCoins(const std::function<void (bool)> &&callback);
        void initializeCounter(const std::function<void (bool)> &&callback);

        void addLeftSupportedCoins(const std::function<void (bool)> &&callback);
        bool addSupportedCoin(const std::string_view &coinCode);

        Bus::Command createCommand(const Bus::HeaderCode headerCode);
        Bus::DataCommand createDataCommand(const Bus::HeaderCode headerCode, unsigned char *data, unsigned char length);

        static bool removeCoin(std::vector<Coin> &vector, const Coin &coin);
        inline static bool coinExists(std::vector<Coin> &vector, const Coin &coin);
        static int getCoinRating(std::vector<Coin> &rater, const Coin &coin);

        void activateInhibitState(std::function<void (bool)> callback);

        void setMasterInhibitState(bool value, std::function<void (bool)> callback);
        void setInhibitState(std::vector<unsigned char> state, std::function<void (bool)> callback);

        std::vector<unsigned char> generateInhibitState();

        void disableInhibitState(std::function<void (bool)> callback);
        void enterPollLoop();
        void handlePollLoop(std::optional<Bus::DataCommand> command);

        cctalk::Bus &bus;
        unsigned char destinationAddress = 0;
        unsigned char sourceAddress = 0;

        unsigned char lastCounter = 0;

        std::atomic_bool changedCoins;
        std::atomic_bool validated;

        std::vector<Coin> supportedCoins;

        std::vector<Coin> disabledCoins;
        std::vector<Coin> enabledCoins;

        std::function<void(const Coin&)> coinCallback;
    };
}
