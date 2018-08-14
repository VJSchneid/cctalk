#define BOOST_TEST_MODULE Coin
#include <boost/test/included/unit_test.hpp>

#include <cctalk/coin.hpp>

BOOST_AUTO_TEST_CASE(CoinCodeConstructor) {
    using cctalk::Coin;

    Coin coin("EU200B");
    BOOST_CHECK_EQUAL(coin.getCurrency(), Coin::makeCurrency("EU"));
    BOOST_CHECK_EQUAL(coin.getValue(), 200);
    BOOST_CHECK_EQUAL(coin.getRevision(), Coin::B);
}

BOOST_AUTO_TEST_CASE(BadCoinCodeConstructor) {
    using cctalk::Coin;

    BOOST_CHECK_THROW(Coin("EU200B34"), std::logic_error);
    BOOST_CHECK_THROW(Coin("EU1"), std::logic_error);
}

BOOST_AUTO_TEST_CASE(SimpleConstruction) {
    using cctalk::Coin;

    Coin coin(1234, 400, Coin::D);
    BOOST_CHECK_EQUAL(coin.getCurrency(), 1234);
    BOOST_CHECK_EQUAL(coin.getValue(), 400);
    BOOST_CHECK_EQUAL(coin.getRevision(), Coin::D);
}

BOOST_AUTO_TEST_CASE(Setter) {
    using cctalk::Coin;

    Coin coin;
    coin.setCurrency(321);
    coin.setValue(100);
    coin.setRevision(Coin::F);

    BOOST_CHECK_EQUAL(coin.getCurrency(), 321);
    BOOST_CHECK_EQUAL(coin.getValue(), 100);
    BOOST_CHECK_EQUAL(coin.getRevision(), Coin::F);
}

BOOST_AUTO_TEST_CASE(ExactEqual) {
    using cctalk::Coin;

    Coin coin1(Coin::makeCurrency("EU"), 100, Coin::D);
    Coin coin2(Coin::makeCurrency("EU"), 100, Coin::D);

    BOOST_CHECK(coin1 == coin2);
    BOOST_CHECK(coin2 == coin1);
}

BOOST_AUTO_TEST_CASE(CompareCoinWithoutCurrency) {
    using cctalk::Coin;

    Coin coin1(Coin::makeCurrency("EU"), 100, Coin::D);
    Coin coin2(0, 100, Coin::D);

    BOOST_CHECK(coin1.sameType(coin2));
    BOOST_CHECK(coin2.sameType(coin1));
}

BOOST_AUTO_TEST_CASE(CompareCoinWithoutValue) {
    using cctalk::Coin;

    Coin coin1(Coin::makeCurrency("EU"), 100, Coin::D);
    Coin coin2(Coin::makeCurrency("EU"), 0, Coin::D);

    BOOST_CHECK(coin1.sameType(coin2));
    BOOST_CHECK(coin2.sameType(coin1));
}

BOOST_AUTO_TEST_CASE(CompareCoinWithoutRevision) {
    using cctalk::Coin;

    Coin coin1(Coin::makeCurrency("EU"), 100, Coin::D);
    Coin coin2(Coin::makeCurrency("EU"), 100, Coin::NONE);

    BOOST_CHECK(coin1.sameType(coin2));
    BOOST_CHECK(coin2.sameType(coin1));
}

BOOST_AUTO_TEST_CASE(CompareCoinWithDifferentCurrency) {
    using cctalk::Coin;

    Coin coin1(Coin::makeCurrency("EU"), 100, Coin::D);
    Coin coin2(Coin::makeCurrency("DE"), 100, Coin::D);

    BOOST_CHECK(!coin1.sameType(coin2));
    BOOST_CHECK(!coin2.sameType(coin1));
}

BOOST_AUTO_TEST_CASE(CompareCoinWithDifferentValue) {
    using cctalk::Coin;

    Coin coin1(Coin::makeCurrency("EU"), 100, Coin::D);
    Coin coin2(Coin::makeCurrency("EU"), 150, Coin::D);

    BOOST_CHECK(!coin1.sameType(coin2));
    BOOST_CHECK(!coin2.sameType(coin1));
}

BOOST_AUTO_TEST_CASE(CompareCoinWithDifferentRevision) {
    using cctalk::Coin;

    Coin coin1(Coin::makeCurrency("EU"), 100, Coin::B);
    Coin coin2(Coin::makeCurrency("EU"), 100, Coin::D);

    BOOST_CHECK(!coin1.sameType(coin2));
    BOOST_CHECK(!coin2.sameType(coin1));
}

BOOST_AUTO_TEST_CASE(Specialization) {
    using cctalk::Coin;

    Coin coin;
    BOOST_CHECK_EQUAL(coin.getSpecialization(), 0);

    coin.setCurrency(Coin::makeCurrency("EU"));
    BOOST_CHECK_EQUAL(coin.getSpecialization(), 1);

    coin.setValue(10);
    BOOST_CHECK_EQUAL(coin.getSpecialization(), 2);

    coin.setRevision(Coin::D);
    BOOST_CHECK_EQUAL(coin.getSpecialization(), 3);
}
