#include "zerolancom/zerolancom.hpp"
#include <iostream>

std::string callbackTypeA(const std::string &msg)
{
  zlc::info("Received request on CallbackTypeA: {}", msg);
  return "Response from CallbackTypeA";
}

std::string callbackTypeB()
{
  zlc::info("Received request on CallbackTypeB");
  return "Response from CallbackTypeB";
}

void callbackTypeC(const std::string &msg)
{
  zlc::info("Received request on CallbackTypeC: {}", msg);
}

void callbackTypeD()
{
  zlc::info("Received request on CallbackTypeD");
}

class ServiceHandlerClass
{
public:
  std::string memberFunctionHandlerA(const std::string &msg)
  {
    zlc::info("Received request on memberFunctionHandler: {}", msg);
    return "Response from memberFunctionHandler";
  }

  std::string memberFunctionHandlerB()
  {
    zlc::info("Received request on memberFunctionHandlerB");
    return "Response from memberFunctionHandlerB";
  }

  void memberFunctionHandlerC(const std::string &msg)
  {
    zlc::info("Received request on memberFunctionHandlerC: {}", msg);
  }

  void memberFunctionHandlerD()
  {
    zlc::info("Received request on memberFunctionHandlerD");
  }
};

int main()
{
  // initialize logger
  zlc::init("ServiceExample", "127.0.0.1");
  std::string response = "";
  zlc::registerServiceHandler("ServiceA", callbackTypeA);
  zlc::registerServiceHandler("ServiceB", callbackTypeB);
  zlc::registerServiceHandler("ServiceC", callbackTypeC);
  zlc::registerServiceHandler("ServiceD", callbackTypeD);
  zlc::request("ServiceA", "Hello Service A", response);
  zlc::info("================================================");
  zlc::request("ServiceB", zlc::Empty, response);
  zlc::info("================================================");
  zlc::request("ServiceC", "Hello Service C", zlc::Empty);
  zlc::info("================================================");
  zlc::request("ServiceD", zlc::Empty, zlc::Empty);
  zlc::info("================================================");
  ServiceHandlerClass handlerInstance;
  zlc::registerServiceHandler("ServiceE", &ServiceHandlerClass::memberFunctionHandlerA,
                              &handlerInstance);

  zlc::registerServiceHandler("ServiceF", &ServiceHandlerClass::memberFunctionHandlerB,
                              &handlerInstance);
  zlc::registerServiceHandler("ServiceG", &ServiceHandlerClass::memberFunctionHandlerC,
                              &handlerInstance);
  zlc::registerServiceHandler("ServiceH", &ServiceHandlerClass::memberFunctionHandlerD,
                              &handlerInstance);
  zlc::request("ServiceE", "Hello Service E", response);
  zlc::info("================================================");
  zlc::request("ServiceF", zlc::Empty, response);
  zlc::info("================================================");
  zlc::request("ServiceG", "Hello Service G", zlc::Empty);
  zlc::info("================================================");
  zlc::request("ServiceH", zlc::Empty, zlc::Empty);
  zlc::info("================================================");
  return 0;
}