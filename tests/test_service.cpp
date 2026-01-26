#include <gtest/gtest.h>

#include <atomic>
#include <string>

#include "zerolancom/nodes/zerolancom_node.hpp"
#include "zerolancom/serialization/msppack_codec.hpp"
#include "zerolancom/sockets/client.hpp"
#include "zerolancom/zerolancom.hpp"

#include "test_utils.hpp"

using namespace zlc;
using namespace zlc_test;

// =============================================
// Test Fixture
// =============================================

class ServiceTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    node_name_ = unique_name("ServiceTestNode");
    zlc::init(node_name_, "127.0.0.1");
  }

  void TearDown() override
  {
    zlc::shutdown();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  std::string node_name_;
};

// =============================================
// Free Function Handler Tests
// =============================================

namespace
{
std::string echoHandler(const std::string &msg)
{
  return "echo:" + msg;
}

std::string emptyRequestHandler(const Empty &)
{
  return "no_request";
}

Empty stringRequestNoResponse(const std::string &)
{
  return empty;
}

Empty emptyBothHandler(const Empty &)
{
  return empty;
}
} // namespace

TEST_F(ServiceTest, FreeFunctionHandler_StringToString)
{
  std::string service = unique_name("EchoService");

  zlc::registerServiceHandler(service, echoHandler);
  zlc::waitForService(service, 1000);

  std::string response;
  Client::zlcRequest<const std::string &, std::string>(service, "hello", response);

  EXPECT_EQ(response, "echo:hello");
}

TEST_F(ServiceTest, FreeFunctionHandler_EmptyToString)
{
  std::string service = unique_name("EmptyReqService");

  zlc::registerServiceHandler(service, emptyRequestHandler);
  zlc::waitForService(service, 1000);

  std::string response;
  Client::zlcRequest<const Empty &, std::string>(service, empty, response);

  EXPECT_EQ(response, "no_request");
}

TEST_F(ServiceTest, FreeFunctionHandler_StringToEmpty)
{
  std::string service = unique_name("NoRespService");

  zlc::registerServiceHandler(service, stringRequestNoResponse);
  zlc::waitForService(service, 1000);

  Empty response;
  Client::zlcRequest<const std::string &, Empty>(service, "test", response);
  // If we reach here without exception, test passes
  SUCCEED();
}

TEST_F(ServiceTest, FreeFunctionHandler_EmptyToEmpty)
{
  std::string service = unique_name("EmptyService");

  zlc::registerServiceHandler(service, emptyBothHandler);
  zlc::waitForService(service, 1000);

  Empty response;
  Client::zlcRequest<const Empty &, Empty>(service, empty, response);
  SUCCEED();
}

// =============================================
// Lambda Handler Tests (convert to function pointer with +)
// =============================================

TEST_F(ServiceTest, LambdaHandler_StringToString)
{
  std::string service = unique_name("LambdaService");

  zlc::registerServiceHandler(
      service, +[](const std::string &req) { return "lambda:" + req; });
  zlc::waitForService(service, 1000);

  std::string response;
  Client::zlcRequest<const std::string &, std::string>(service, "test", response);

  EXPECT_EQ(response, "lambda:test");
}

// =============================================
// Member Function Handler Tests
// =============================================

class ServiceHandlerClass
{
public:
  std::string handleStringToString(const std::string &msg)
  {
    return "member:" + msg;
  }

  std::string handleEmptyToString(const Empty &)
  {
    call_count++;
    return "member_empty_req";
  }

  Empty handleStringToEmpty(const std::string &)
  {
    call_count++;
    return empty;
  }

  Empty handleEmptyToEmpty(const Empty &)
  {
    call_count++;
    return empty;
  }

  int call_count = 0;
};

TEST_F(ServiceTest, MemberFunctionHandler_StringToString)
{
  std::string service = unique_name("MemberService");
  ServiceHandlerClass handler;

  zlc::registerServiceHandler(service, &ServiceHandlerClass::handleStringToString,
                              &handler);
  zlc::waitForService(service, 1000);

  std::string response;
  Client::zlcRequest<const std::string &, std::string>(service, "test", response);

  EXPECT_EQ(response, "member:test");
}

TEST_F(ServiceTest, MemberFunctionHandler_EmptyToString)
{
  std::string service = unique_name("MemberEmptyReq");
  ServiceHandlerClass handler;

  zlc::registerServiceHandler(service, &ServiceHandlerClass::handleEmptyToString,
                              &handler);
  zlc::waitForService(service, 1000);

  std::string response;
  Client::zlcRequest<const Empty &, std::string>(service, empty, response);

  EXPECT_EQ(response, "member_empty_req");
  EXPECT_EQ(handler.call_count, 1);
}

// =============================================
// Multiple Services Tests
// =============================================

TEST_F(ServiceTest, MultipleServicesOnSameNode)
{
  std::string service1 = unique_name("MultiService1");
  std::string service2 = unique_name("MultiService2");
  std::string service3 = unique_name("MultiService3");

  zlc::registerServiceHandler(
      service1, +[](const std::string &) { return std::string("svc1"); });
  zlc::registerServiceHandler(
      service2, +[](const std::string &) { return std::string("svc2"); });
  zlc::registerServiceHandler(
      service3, +[](const std::string &) { return std::string("svc3"); });

  zlc::waitForService(service1, 1000);
  zlc::waitForService(service2, 1000);
  zlc::waitForService(service3, 1000);

  std::string r1, r2, r3;
  Client::zlcRequest<const std::string &, std::string>(service1, "", r1);
  Client::zlcRequest<const std::string &, std::string>(service2, "", r2);
  Client::zlcRequest<const std::string &, std::string>(service3, "", r3);

  EXPECT_EQ(r1, "svc1");
  EXPECT_EQ(r2, "svc2");
  EXPECT_EQ(r3, "svc3");
}

// =============================================
// Custom Message Type Tests
// =============================================

struct CustomRequest
{
  int id;
  std::string command;
  MSGPACK_DEFINE(id, command)
};

struct CustomResponse
{
  bool success;
  std::string result;
  MSGPACK_DEFINE(success, result)
};

namespace
{
CustomResponse customMsgHandler(const CustomRequest &req)
{
  CustomResponse resp;
  resp.success = true;
  resp.result = "processed_" + std::to_string(req.id) + "_" + req.command;
  return resp;
}
} // namespace

TEST_F(ServiceTest, CustomMessageTypes)
{
  std::string service = unique_name("CustomMsgService");

  zlc::registerServiceHandler(service, customMsgHandler);
  zlc::waitForService(service, 1000);

  CustomRequest request{42, "execute"};
  CustomResponse response;
  Client::zlcRequest<const CustomRequest &, CustomResponse>(service, request, response);

  EXPECT_TRUE(response.success);
  EXPECT_EQ(response.result, "processed_42_execute");
}

// =============================================
// waitForService Tests
// =============================================

TEST_F(ServiceTest, WaitForServiceSucceeds)
{
  std::string service = unique_name("WaitService");

  zlc::registerServiceHandler(
      service, +[](const std::string &) { return std::string("ok"); });

  zlc::waitForService(service, 1000);
  SUCCEED();
}

// =============================================
// High-Level API Tests (zlc::request)
// =============================================

TEST_F(ServiceTest, HighLevelRequest_StringToString)
{
  std::string service = unique_name("HighLevelService");

  zlc::registerServiceHandler(
      service, +[](const std::string &req) { return "high:" + req; });

  std::string response;
  zlc::request(service, std::string("level"), response);

  EXPECT_EQ(response, "high:level");
}
