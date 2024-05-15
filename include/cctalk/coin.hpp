#pragma once

#include <cmath>
#include <string_view>

namespace cctalk {

    class Coin {
    public:

        enum Revision {NONE, A, B, C, D, E, F, G};

        static constexpr unsigned short makeCurrency(const char *currencyPrefix) noexcept {
            if (*currencyPrefix == 0) {
                return 0;
            } else {
                return *reinterpret_cast<const unsigned short*>(currencyPrefix);
            }
        }

        static constexpr unsigned int makeValue(const char *valueCode) noexcept {
            unsigned int value = 0;
            unsigned int exponent = 0;
            for (int pos = 0; pos < 3; pos++) {
                if (valueCode[pos] >= '0' && valueCode[pos] <= '9') {
                    value *= 10;
                    value += (valueCode[pos] - '0');
                } else if (valueCode[pos] == 'K') {
                    exponent = 1 + pos;
                } else if (valueCode[pos] == 'M') {
                    exponent = 4 + pos;
                } if (valueCode[pos] == 0) {
                    break;
                }
            }
            return value * std::pow(10, exponent);
        }

        static constexpr Revision makeRevision(const char revisionChar) noexcept {
            if (revisionChar >= 'A' && revisionChar <= 'G') {
                return static_cast<Revision>(revisionChar - ('A' - A));
            } else if (revisionChar >= 'a' && revisionChar <= 'g') {
                return static_cast<Revision>(revisionChar - ('a' - A));
            } else {
                return NONE;
            }
        }

        Coin() = default;
        Coin(const std::string_view &coinCode) noexcept(false);
        Coin(unsigned short currency, unsigned int value, const Revision revision = NONE) noexcept;

        unsigned short getCurrency() const noexcept;
        unsigned int getValue() const noexcept;
        Revision getRevision() const noexcept;
        int getSpecialization() const noexcept;

        void setCurrency(unsigned short newCurrency) noexcept;
        void setValue(unsigned int newValue) noexcept;
        void setRevision(Revision newRevision) noexcept;

        bool sameType(const Coin &coin) const noexcept;

        bool operator==(const Coin &coin) const noexcept;
        bool operator!=(const Coin &coin) const noexcept;

    private:


        Revision revision = NONE;
        unsigned short currency = 0;
        unsigned int value = 0;
    };

    constexpr short getCurrencyShort(const char *prefix) {
        return (prefix[0] << 8) + prefix[1];
    }

    static const short CurrencyPrefix = {
        getCurrencyShort("EU")
    };
}

std::ostream &operator<<(std::ostream &os, const cctalk::Coin &coin);
