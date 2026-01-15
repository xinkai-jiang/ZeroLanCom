# ZeroLanCom Codebase Guide for AI Agents

## Project Overview

ZeroLanCom is a lightweight C++17 distributed messaging framework built on **ZeroMQ** and **MessagePack** that provides:
- Topic-based Pub/Sub messaging
- Synchronous RPC Service calls
- Automatic message serialization
- Dynamic node discovery via multicast
- Header-only core with minimal compilation overhead

**Key Goal**: Enable simple inter-process and distributed communication without complex broker infrastructure.

## Architecture

### Core Component: ZeroLanComNode (Singleton)

Located in [include/zerolancom/nodes/zerolancom_node.hpp](../../include/zerolancom/nodes/zerolancom_node.hpp), this singleton manages:

1. **ServiceManager**: Handles RPC service registration and invocation via ZMQ REP/REQ sockets on a dedicated port
2. **SubscriberManager**: Manages topic subscriptions with ZMQ SUB sockets
3. **NodeInfoManager**: Tracks remote nodes discovered via multicast heartbeats
4. **LocalNodeInfo**: Registers local services/topics, generates periodic heartbeats (36-char UUID-based node ID)
5. **MulticastSender/Receiver**: UDP multicast on configurable group/port for node discovery

### Data Flow Patterns

**Service (RPC) Call**:
```
Client.request() → ServiceManager resolves address → ZMQ REQ → 
Handler invoked → Serialized response → ZMQ REP → Client receives
```

**Pub/Sub**:
```
Publisher binds ephemeral TCP port → Registers in LocalNodeInfo → 
Multicast heartbeat announces topic → Remote Subscriber connects via NodeInfoManager
```

### Critical Design Decisions

1. **ZMQ Context is Global**: [ZmqContext::instance()](../../include/zerolancom/utils/zmq_utils.hpp) used everywhere—thread-safe singleton wrapper around `zmq::context_t`
2. **Template Classes Must Be Header-Only**: `Publisher<T>`, `Client` template methods, and serialization in [include/zerolancom/serialization/](../../include/zerolancom/serialization/) to enable instantiation
3. **Non-Template Logic in `.cpp`**: `ServiceManager`, `SubscriberManager`, `NodeInfoManager` have declarations in `.hpp` and implementations in [src/](../../src/)
4. **Serialization via MessagePack**: [msppack_codec.hpp](../../include/zerolancom/serialization/msppack_codec.hpp) provides `encode<T>()` and `decode<T>()` templates; `zlc::Empty` maps to `msgpack::nil` for `void` semantics
5. **Namespacing**: All code in `zlc::` namespace; examples and tests may have local helpers

## Common Development Tasks

### Build
```bash
cd build && rm -rf * && cmake .. -DCMAKE_BUILD_TYPE=Debug && make -j$(nproc)
```
See [scripts/compile.sh](../../scripts/compile.sh). Debug build enables `-Wall -Wextra -Wpedantic`. Install with `make install`.

### Test
```bash
cd build/tests && ./test_single_node  # or ctest
```
Tests link [src/](../../src/) implementations and use [tests/include/test_utils.hpp](../../tests/include/test_utils.hpp) for helpers.

### Run Examples
```bash
cd build/examples && ./zerolancom_example  # or custom_message_example, service_example
```
See [examples/](../../examples/) for patterns:
- [zerolancom_example.cpp](../../examples/zerolancom_example.cpp): Pub/Sub + RPC basics
- [service_example.cpp](../../examples/service_example.cpp): Handler variations (free functions, member methods, empty payloads)

### Code Formatting
```bash
./scripts/format.sh
```

## Key Code Patterns

### Initialization
```cpp
zlc::init("NodeName", "127.0.0.1");  // Non-member in zerolancom.hpp wrapper
// Internally calls ZeroLanComNode::init() → starts multicast threads
```

### Service Registration (All in [zerolancom.hpp](../../include/zerolancom/zerolancom.hpp) API)
```cpp
// Free function handler
zlc::registerServiceHandler("MyService", [](const Request &r) { return Response; });

// Member function handler
MyClass inst;
zlc::registerServiceHandler("MyService", &MyClass::method, &inst);

// Handler signature: `ResponseType handler(const RequestType &)` 
// Use zlc::Empty for void semantics
```

### Service Request
```cpp
std::string response;
zlc::waitForService("MyService", 1000);  // Blocks until available
zlc::request<std::string, std::string>("MyService", "hello", response);
```

### Pub/Sub
```cpp
// Publisher (header-only template)
zlc::Publisher<MyMessageType> pub("topic_name");
pub.publish(msg_instance);  // Binds ephemeral port, announces via multicast

// Subscriber (callback registered with SubscriberManager)
zlc::registerSubscriberHandler("topic_name", [](const MyMessageType &m) { 
  // Handle message
});
```

### Custom Message Types
Types must be **MessagePack-serializable**:
```cpp
struct MyMsg {
  int id;
  std::string name;
  
  // For binary_codec or msgpack: implement pack/unpack or MSGPACK_DEFINE macro
  MSGPACK_DEFINE(id, name)
};
```
See [examples/custom_message.cpp](../../examples/custom_message.cpp) for details.

## File Organization Reference

| Directory | Purpose |
|-----------|---------|
| [include/zerolancom/](../../include/zerolancom/) | Public headers (API contract) |
| [src/](../../src/) | Core implementations (non-template logic) |
| [include/zerolancom/nodes/](../../include/zerolancom/nodes/) | Node discovery & multicast (ZeroLanComNode, node info structures) |
| [include/zerolancom/sockets/](../../include/zerolancom/sockets/) | ZMQ wrappers (Client, Publisher, ServiceManager, SubscriberManager) |
| [include/zerolancom/serialization/](../../include/zerolancom/serialization/) | MessagePack codec (encode/decode templates, Empty type) |
| [include/zerolancom/utils/](../../include/zerolancom/utils/) | Logging (spdlog), exceptions, singleton, message buffers, ZMQ utils |
| [examples/](../../examples/) | Executable demos with full workflows |
| [tests/](../../tests/) | Unit tests; link against shared lib in build/ |

## Debugging Tips

1. **Logger**: Use `zlc::info()`, `zlc::warn()`, `zlc::error()` (spdlog wrappers in [include/zerolancom/utils/logger.hpp](../../include/zerolancom/utils/logger.hpp))
2. **Multicast Issues**: Check firewall & group address (default often `224.0.1.123:5555` but configurable in `init()`)
3. **Service Not Found**: `waitForService()` blocks; verify remote node heartbeat received by NodeInfoManager
4. **Serialization Errors**: Ensure type implements `MSGPACK_DEFINE` or custom pack/convert adaptor
5. **Thread Safety**: All public APIs (registerService*, registerSubscriber*, publish*) are thread-safe via internal mutexes; ZMQ context is thread-safe

## Dependencies

- **libzmq3-dev**: ZeroMQ C++ bindings (via pkg-config in CMakeLists.txt)
- **spdlog**: Logging (header-only)
- **msgpack-c++**: Serialization (header-only)
- **fmt**: Formatting library (spdlog dependency)

Install on Ubuntu: `sudo apt install libzmq3-dev libspdlog-dev`

## Python Bindings

[python/lancom_pybind.cpp](../../python/lancom_pybind.cpp) provides PyBind11 wrapper. Build via `cmake` with Python detection enabled; generates shared library importable as `import zerolancom`.
