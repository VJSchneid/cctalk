#include <boost/test/unit_test.hpp>

#include <cctalk/coin.hpp>

BOOST_AUTO_TEST_SUITE(Revision)

BOOST_AUTO_TEST_CASE(NormalChars) {
    using cctalk::Coin;

    auto revision = Coin::makeRevision('A');
    BOOST_CHECK_EQUAL(revision, Coin::A);

    revision = Coin::makeRevision('D');
    BOOST_CHECK_EQUAL(revision, Coin::D);
}

BOOST_AUTO_TEST_CASE(LowerCaseChars) {
    using cctalk::Coin;

    auto revision = Coin::makeRevision('a');
    BOOST_CHECK_EQUAL(revision, Coin::A);

    revision = Coin::makeRevision('c');
    BOOST_CHECK_EQUAL(revision, Coin::C);
}

BOOST_AUTO_TEST_CASE(InvalidChars) {
    using cctalk::Coin;

    auto revision = Coin::makeRevision('%');
    BOOST_CHECK_EQUAL(revision, Coin::NONE);

    revision = Coin::makeRevision('!');
    BOOST_CHECK_EQUAL(revision, Coin::NONE);
}

BOOST_AUTO_TEST_SUITE_END()
