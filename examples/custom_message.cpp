#include "zerolancom/zerolancom.hpp"
#include <iostream>

void topicCallback(const std::string &msg)
{
  zlc::info("Received message on subscribed topic: {}", msg);
}

struct CustomMessage
{
  int count;
  std::string name;
  std::vector<float> data;
  MSGPACK_DEFINE_MAP(count, name, data);
};

int main()
{
  zlc::init("CustomMessageNode", "127.0.0.1");
  zlc::Publisher<CustomMessage> pub("CustomMessage");
  try
  {
    int counter = 0;
    while (true)
    {
      pub.publish(CustomMessage{counter, "CustomMessage", {1.0f, 2.0f, 3.0f}});
      counter++;
      zlc::info("Published message to CustomMessage");
      zlc::sleep(1000);
    }
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
  }

  return 0;
}