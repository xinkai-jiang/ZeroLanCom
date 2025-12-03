#pragma once

#include <functional>
#include <string>
#include <unordered_map>

#include "lancom/types.hpp"

namespace lancom {
namespace utils {

// Basic encoders for different types
BytesMessage bytes_encoder(const BytesMessage& msg);
BytesMessage string_encoder(const std::string& msg);
BytesMessage json_encoder(const std::unordered_map<std::string, std::string>& msg);
BytesMessage msgpack_encoder(const std::unordered_map<std::string, std::string>& msg);

// Basic decoders for different types
BytesMessage bytes_decoder(const BytesMessage& msg);
std::string string_decoder(const BytesMessage& msg);
std::unordered_map<std::string, std::string> json_decoder(const BytesMessage& msg);
std::unordered_map<std::string, std::string> msgpack_decoder(const BytesMessage& msg);

// Adapter functions for service proxies
template<typename T>
std::function<BytesMessage(const T&)> get_encoder();

template<typename T>
std::function<T(const BytesMessage&)> get_decoder();

// Template specializations

template<>
inline std::function<BytesMessage(const BytesMessage&)> get_encoder<BytesMessage>() {
    return bytes_encoder;
}

template<>
inline std::function<BytesMessage(const std::string&)> get_encoder<std::string>() {
    return string_encoder;
}

template<>
inline std::function<BytesMessage(const std::unordered_map<std::string, std::string>&)> 
get_encoder<std::unordered_map<std::string, std::string>>() {
    return json_encoder;
}

template<>
inline std::function<BytesMessage(const BytesMessage&)> get_decoder<BytesMessage>() {
    return bytes_decoder;
}

template<>
inline std::function<std::string(const BytesMessage&)> get_decoder<std::string>() {
    return string_decoder;
}

template<>
inline std::function<std::unordered_map<std::string, std::string>(const BytesMessage&)> 
get_decoder<std::unordered_map<std::string, std::string>>() {
    return json_decoder;
}

} // namespace utils
} // namespace lancom
