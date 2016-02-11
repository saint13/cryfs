#include <gtest/gtest.h>
#include "cpp-utils/network/FakeHttpClient.h"
#include "cpp-utils/pointer/unique_ref_boost_optional_gtest_workaround.h"

using boost::none;

using namespace cpputils;

TEST(FakeHttpClientTest, Empty) {
        EXPECT_EQ(none, FakeHttpClient().get("http://example.com"));
}

TEST(FakeHttpClientTest, Nonexisting) {
        FakeHttpClient client;
        client.addWebsite("http://existing.com", "content");
        EXPECT_EQ(none, client.get("http://notexisting.com"));
}

TEST(FakeHttpClientTest, Existing) {
        FakeHttpClient client;
        client.addWebsite("http://existing.com", "content");
        EXPECT_EQ("content", client.get("http://existing.com").value());
}

TEST(FakeHttpClientTest, TwoExisting) {
        FakeHttpClient client;
        client.addWebsite("http://firstexisting.com", "first_content");
        client.addWebsite("http://secondexisting.com", "second_content");
        EXPECT_EQ("first_content", client.get("http://firstexisting.com").value());
        EXPECT_EQ("second_content", client.get("http://secondexisting.com").value());
        EXPECT_EQ(none, client.get("http://notexisting.com"));
}

TEST(FakeHttpClientTest, Overwriting) {
        FakeHttpClient client;
        client.addWebsite("http://existing.com", "content");
        client.addWebsite("http://existing.com", "new_content");
        EXPECT_EQ("new_content", client.get("http://existing.com").value());
}
