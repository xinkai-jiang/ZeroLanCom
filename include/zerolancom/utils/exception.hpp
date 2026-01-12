#pragma once
#include <stdexcept>

namespace zlc
{

class NodeInfoDecodeException : public std::runtime_error
{
public:
  NodeInfoDecodeException(const std::string &msg)
      : std::runtime_error("NodeInfo Decode Error: " + msg)
  {
  }
};

class EncodeException : public std::runtime_error
{
public:
  EncodeException(const std::string &msg) : std::runtime_error("Encode Error: " + msg)
  {
  }
};

class DecodeException : public std::runtime_error
{
public:
  DecodeException(const std::string &msg) : std::runtime_error("Decode Error: " + msg)
  {
  }
};

} // namespace zlc