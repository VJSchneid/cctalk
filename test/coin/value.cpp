#include <boost/test/unit_test.hpp>
#include <cctalk/coin.hpp>

BOOST_AUTO_TEST_SUITE(Value)

BOOST_AUTO_TEST_CASE(SimpleValue) {
    using cctalk::Coin;

    auto value = Coin::makeValue("123");
    BOOST_CHECK_EQUAL(value, 123);

    value = Coin::makeValue("012");
    BOOST_CHECK_EQUAL(value, 12);

    value = Coin::makeValue("004");
    BOOST_CHECK_EQUAL(value, 4);
}

BOOST_AUTO_TEST_CASE(ValueWithK) {
    using cctalk::Coin;

    auto value = Coin::makeValue("1K5");
    BOOST_CHECK_EQUAL(value, 1500);

    value = Coin::makeValue("K32");
    BOOST_CHECK_EQUAL(value, 320);

    value = Coin::makeValue("14K");
    BOOST_CHECK_EQUAL(value, 14000);

    value = Coin::makeValue("03K");
    BOOST_CHECK_EQUAL(value, 3000);
}

BOOST_AUTO_TEST_CASE(ValueWithM) {
    using cctalk::Coin;

    auto value = Coin::makeValue("1M2");
    BOOST_CHECK_EQUAL(value, 1200000);

    value = Coin::makeValue("M12");
    BOOST_CHECK_EQUAL(value, 120000);
}

BOOST_AUTO_TEST_CASE(StringToSmall) {
    using cctalk::Coin;

    auto value = Coin::makeValue("12");
    BOOST_CHECK_EQUAL(value, 12);

    value = Coin::makeValue("4");
    BOOST_CHECK_EQUAL(value, 4);
}

BOOST_AUTO_TEST_CASE(StringWithInvalidChars) {
    using cctalk::Coin;

    auto value = Coin::makeValue("1S3");
    BOOST_CHECK_EQUAL(value, 13);

    value = Coin::makeValue("3V!");
    BOOST_CHECK_EQUAL(value, 3);
}

BOOST_AUTO_TEST_CASE(StrangeValidStrings) {
    using cctalk::Coin;

    auto value = Coin::makeValue("1KM");
    BOOST_CHECK_EQUAL(value, 1000000);

    value = Coin::makeValue("4MM");
    BOOST_CHECK_EQUAL(value, 4000000);

    value = Coin::makeValue("1MK");
    BOOST_CHECK_EQUAL(value, 1000);
}

BOOST_AUTO_TEST_SUITE_END()
