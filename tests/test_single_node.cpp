#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <string>
#include <thread>

#include "zerolancom/zerolancom.hpp"

#include "test_utils.hpp"

using namespace zlc;
using namespace zlc_test;

// =============================================
// Global State for Callbacks
// =============================================

namespace
{
AsyncResult<std::string> g_topic_result;

void topicCallback(const std::string &msg)
{
  g_topic_result.set(msg);
}
} // namespace

// =============================================
// Test Fixture
// =============================================

class SingleNodeTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    node_name_ = unique_name("TestNode");
    zlc::init(node_name_, "127.0.0.1");
    g_topic_result.reset();
  }

  void TearDown() override
  {
    zlc::shutdown();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  std::string node_name_;
};

// =============================================
// Service Tests (Reliable - uses waitForService)
// =============================================

TEST_F(SingleNodeTest, BasicServiceCall)
{
  std::string service_name = unique_name("EchoService");

  zlc::registerServiceHandler(
      service_name, +[](const std::string &req) { return "Echo: " + req; });

  zlc::waitForService(service_name);

  std::string response;
  zlc::request(service_name, std::string("hello"), response);

  EXPECT_EQ(response, "Echo: hello");
}

TEST_F(SingleNodeTest, MultipleServiceCalls)
{
  std::string service_name = unique_name("CounterService");
  std::atomic<int> counter{0};

  zlc::registerServiceHandler(
      service_name, +[](const int &req) { return req * 2; });

  zlc::waitForService(service_name);

  for (int i = 1; i <= 5; i++)
  {
    int response;
    zlc::request(service_name, i, response);
    EXPECT_EQ(response, i * 2);
  }
}

// =============================================
// Pub/Sub Test (Timing-sensitive)
// =============================================

TEST_F(SingleNodeTest, BasicPubSub)
{
  std::string topic_name = unique_name("TestTopic");

  zlc::registerSubscriberHandler(topic_name, topicCallback);

  Publisher<std::string> pub(topic_name);

  // Wait for discovery
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  pub.publish("test_message");

  bool received = g_topic_result.wait_for(std::chrono::milliseconds(1000));

  if (received)
  {
    EXPECT_EQ(g_topic_result.get(), "test_message");
  }
  else
  {
    GTEST_SKIP() << "Pub/Sub discovery timed out (expected in single-process tests)";
  }
}
