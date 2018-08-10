#include <boost/test/unit_test.hpp>
#include <cctalk/coin.hpp>

BOOST_AUTO_TEST_SUITE(Currency)

BOOST_AUTO_TEST_CASE(MakeCurrencyWithNormalValue) {
    using cctalk::Coin;

    auto currency = Coin::makeCurrency("EU");
    BOOST_CHECK_EQUAL(currency, 'E' + ('U' << 8));

    currency = Coin::makeCurrency("ZW");
    BOOST_CHECK_EQUAL(currency, 'Z' + ('W' << 8));
}

BOOST_AUTO_TEST_CASE(MakeCurrencyWithToShortString) {
    using cctalk::Coin;

    auto currency = Coin::makeCurrency("E");
    BOOST_CHECK_EQUAL(currency, 'E');
}

BOOST_AUTO_TEST_CASE(MakeCurrencyWithEmptyString) {
    using cctalk::Coin;

    auto currency = Coin::makeCurrency("");
    BOOST_CHECK_EQUAL(currency, 0);
}


BOOST_AUTO_TEST_SUITE_END()
