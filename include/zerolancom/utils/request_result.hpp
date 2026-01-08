#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace zlc
{

namespace ResponseStatus
{
using namespace std::string_view_literals;

constexpr std::string_view SUCCESS = "SUCCESS"sv;
constexpr std::string_view NOSERVICE = "NOSERVICE"sv;
constexpr std::string_view INVALID_RESPONSE = "INVALID_RESPONSE"sv;
constexpr std::string_view SERVICE_FAIL = "SERVICE_FAIL"sv;
constexpr std::string_view SERVICE_TIMEOUT = "SERVICE_TIMEOUT"sv;
constexpr std::string_view INVALID_REQUEST = "INVALID_REQUEST"sv;
constexpr std::string_view UNKNOWN_ERROR = "UNKNOWN_ERROR"sv;

// Helper to validate incoming status strings
inline bool is_error(std::string_view status)
{
  return status != SUCCESS;
}
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
