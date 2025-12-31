#include "zerolancom/zerolancom.hpp"

void callbackTypeA(const std::string &msg)
{
  zlc::info("Received request on CallbackTypeA: {}", msg);
}

class SubscriberHandlerClass
{
public:
  void memberFunctionHandlerA(const std::string &msg)
  {
    zlc::info("Received request on memberFunctionHandlerA: {}", msg);
  }
};

int main()
{
  // initialize logger
  zlc::init("SubscriberExample", "127.0.0.1");
  zlc::registerSubscriberHandler("TopicA", callbackTypeA);
  SubscriberHandlerClass handlerObj;
  zlc::registerSubscriberHandler(
      "TopicA", SubscriberHandlerClass::memberFunctionHandlerA, &handlerObj);
  zlc::spin();
  return 0;
}