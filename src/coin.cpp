#include <cctalk/coin.hpp>

#include <stdexcept>
#include <ostream>

namespace cctalk {

    Coin::Coin(const std::string_view &coinCode) noexcept(false) {
        if (coinCode.size() != 6) {
            throw std::logic_error("invalid coin code");
        }

        currency = makeCurrency(coinCode.data());
        value = makeValue(coinCode.data() + 2);
        revision = makeRevision(coinCode[5]);
    }

    Coin::Coin(unsigned short currency, unsigned int value, const Revision revision) noexcept
            : currency(currency), value(value), revision(revision) {
    }


    unsigned short Coin::getCurrency() const noexcept {
        return currency;
    }

    unsigned int Coin::getValue() const noexcept {
        return value;
    }

    Coin::Revision Coin::getRevision() const noexcept {
        return revision;
    }

    void Coin::setCurrency(unsigned short newCurrency) noexcept {
        currency = newCurrency;
    }

    void Coin::setValue(unsigned int newValue) noexcept {
        value = newValue;
    }

    void Coin::setRevision(Revision newRevision) noexcept {
        revision = newRevision;
    }

    bool Coin::sameType(const Coin &coin) const noexcept {
        if (currency == coin.currency || (currency == 0 || coin.currency == 0)) {
            if (value == coin.value || (value == 0 || coin.value == 0)) {
                if (revision == coin.revision || (revision == 0 || coin.revision == 0)) {
                    return true;
                }
            }
        }
        return false;
    }

    bool Coin::operator==(const Coin &coin) const noexcept {
        if (currency == coin.currency) {
            if (value == coin.value) {
                if (revision == coin.revision) {
                    return true;
                }
            }
        }
        return false;
    }

    bool Coin::operator!=(const Coin &coin) const noexcept {
        return !(*this == coin);
    }

    int Coin::getSpecialization() const noexcept {
        int specialization = 0;
        specialization += currency != 0;
        specialization += value != 0;
        specialization += revision != 0;
        return specialization;
    }
}

std::ostream &operator<<(std::ostream &os, const cctalk::Coin &coin) {
    auto currency = coin.getCurrency();
    os.write(reinterpret_cast<char *>(&currency), sizeof(currency));
    os << '-' << coin.getValue();
    os << '-' << (char)(coin.getRevision() + ('A' - cctalk::Coin::A));
    return os;
}
