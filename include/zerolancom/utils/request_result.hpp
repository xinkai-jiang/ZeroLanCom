#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace zlc
{

namespace ResponseStatus
{
constexpr std::string_view SUCCESS = "SUCCESS";
constexpr std::string_view FAIL = "FAIL";
constexpr std::string_view INVALID_ARG = "INVALID_ARG";
constexpr std::string_view BUSY = "BUSY";
constexpr std::string_view UNSUPPORTED = "UNSUPPORTED";
constexpr std::string_view TIMEOUT = "TIMEOUT";
constexpr std::string_view COMM_ERROR = "COMM_ERROR";
constexpr std::string_view INTERNAL_ERROR = "INTERNAL_ERROR";
} // namespace ResponseStatus

class Response
{
public:
  // flag indicates payload contains a detail string
  static constexpr uint8_t FLAG_HAS_DETAIL = 0x01;

  Response() = default;
  explicit Response(std::string code, Bytes payload = {})
      : code(std::move(code)), payload(std::move(payload))
  {
  }

  // Human-readable short description for the code
  static const char *description(const std::string &code);

  std::string code{std::string(ResponseStatus::SUCCESS)};
  Bytes payload;
};

} // namespace zlc
