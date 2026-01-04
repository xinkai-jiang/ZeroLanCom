#include <stdexcept>

namespace zlc
{

class NodeInfoDecodeException : public std::runtime_error {
public:
    NodeInfoDecodeException(const std::string& msg) 
        : std::runtime_error("NodeInfo Decode Error: " + msg) {}
};
} // namespace zlc