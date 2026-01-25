#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <vector>

#include "zerolancom/serialization/msppack_codec.hpp"
#include "zerolancom/utils/message.hpp"

using namespace zlc;

// =============================================
// Primitive Type Serialization Tests
// =============================================

TEST(SerializationTest, EncodeDecodeInt)
{
  ByteBuffer buffer;
  int original = 42;
  int decoded = 0;

  encode(original, buffer);
  ASSERT_GT(buffer.size, 0);

  ByteView view{buffer.data, buffer.size};
  decode(view, decoded);

  EXPECT_EQ(decoded, original);
}

TEST(SerializationTest, EncodeDecodeNegativeInt)
{
  ByteBuffer buffer;
  int original = -12345;
  int decoded = 0;

  encode(original, buffer);
  ByteView view{buffer.data, buffer.size};
  decode(view, decoded);

  EXPECT_EQ(decoded, original);
}

TEST(SerializationTest, EncodeDecodeDouble)
{
  ByteBuffer buffer;
  double original = 3.14159265359;
  double decoded = 0.0;

  encode(original, buffer);
  ByteView view{buffer.data, buffer.size};
  decode(view, decoded);

  EXPECT_DOUBLE_EQ(decoded, original);
}

TEST(SerializationTest, EncodeDecodeString)
{
  ByteBuffer buffer;
  std::string original = "Hello, ZeroLanCom!";
  std::string decoded;

  encode(original, buffer);
  ByteView view{buffer.data, buffer.size};
  decode(view, decoded);

  EXPECT_EQ(decoded, original);
}

TEST(SerializationTest, EncodeDecodeEmptyString)
{
  ByteBuffer buffer;
  std::string original;
  std::string decoded = "not empty";

  encode(original, buffer);
  ByteView view{buffer.data, buffer.size};
  decode(view, decoded);

  EXPECT_EQ(decoded, original);
  EXPECT_TRUE(decoded.empty());
}

TEST(SerializationTest, EncodeDecodeBool)
{
  ByteBuffer buffer;

  // Test true
  bool original_true = true;
  bool decoded_true = false;
  encode(original_true, buffer);
  ByteView view_true{buffer.data, buffer.size};
  decode(view_true, decoded_true);
  EXPECT_TRUE(decoded_true);

  // Test false
  bool original_false = false;
  bool decoded_false = true;
  encode(original_false, buffer);
  ByteView view_false{buffer.data, buffer.size};
  decode(view_false, decoded_false);
  EXPECT_FALSE(decoded_false);
}

// =============================================
// Container Type Serialization Tests
// =============================================

TEST(SerializationTest, EncodeDecodeVectorInt)
{
  ByteBuffer buffer;
  std::vector<int> original = {1, 2, 3, 4, 5};
  std::vector<int> decoded;

  encode(original, buffer);
  ByteView view{buffer.data, buffer.size};
  decode(view, decoded);

  EXPECT_EQ(decoded, original);
}

TEST(SerializationTest, EncodeDecodeVectorString)
{
  ByteBuffer buffer;
  std::vector<std::string> original = {"apple", "banana", "cherry"};
  std::vector<std::string> decoded;

  encode(original, buffer);
  ByteView view{buffer.data, buffer.size};
  decode(view, decoded);

  EXPECT_EQ(decoded, original);
}

TEST(SerializationTest, EncodeDecodeEmptyVector)
{
  ByteBuffer buffer;
  std::vector<int> original;
  std::vector<int> decoded = {1, 2, 3};

  encode(original, buffer);
  ByteView view{buffer.data, buffer.size};
  decode(view, decoded);

  EXPECT_TRUE(decoded.empty());
}

// =============================================
// Empty Type Serialization Tests
// =============================================

TEST(SerializationTest, EncodeDecodeEmpty)
{
  ByteBuffer buffer;
  Empty original;
  Empty decoded;

  encode(original, buffer);
  ASSERT_GT(buffer.size, 0);

  ByteView view{buffer.data, buffer.size};
  // Should not throw
  EXPECT_NO_THROW(decode(view, decoded));
}

TEST(SerializationTest, EmptyDecodesFromNil)
{
  ByteBuffer buffer;
  Empty original;
  Empty decoded;

  encode(original, buffer);
  ByteView view{buffer.data, buffer.size};

  // Decoding Empty from nil should succeed
  EXPECT_NO_THROW(decode(view, decoded));
}

// =============================================
// Custom Struct Serialization Tests
// =============================================

struct SimpleMessage
{
  int id;
  std::string name;
  double value;

  MSGPACK_DEFINE(id, name, value)

  bool operator==(const SimpleMessage &other) const
  {
    return id == other.id && name == other.name && value == other.value;
  }
};

struct NestedMessage
{
  std::string header;
  SimpleMessage payload;
  std::vector<int> tags;

  MSGPACK_DEFINE(header, payload, tags)

  bool operator==(const NestedMessage &other) const
  {
    return header == other.header && payload == other.payload && tags == other.tags;
  }
};

TEST(SerializationTest, EncodeDecodeSimpleStruct)
{
  ByteBuffer buffer;
  SimpleMessage original{42, "test_message", 3.14};
  SimpleMessage decoded;

  encode(original, buffer);
  ByteView view{buffer.data, buffer.size};
  decode(view, decoded);

  EXPECT_EQ(decoded, original);
}

TEST(SerializationTest, EncodeDecodeNestedStruct)
{
  ByteBuffer buffer;
  NestedMessage original{"Header", {1, "nested", 2.71}, {10, 20, 30}};
  NestedMessage decoded;

  encode(original, buffer);
  ByteView view{buffer.data, buffer.size};
  decode(view, decoded);

  EXPECT_EQ(decoded, original);
}

// =============================================
// Error Handling Tests
// =============================================

TEST(SerializationTest, DecodeInvalidDataThrows)
{
  // Create truncated msgpack data (incomplete array header)
  // 0x93 indicates a 3-element array, but we only provide 1 byte
  uint8_t invalid_data[] = {0x93};
  ByteView view{invalid_data, sizeof(invalid_data)};

  std::vector<int> decoded;
  EXPECT_THROW(decode(view, decoded), DecodeException);
}

TEST(SerializationTest, DecodeTypeMismatchThrows)
{
  ByteBuffer buffer;
  std::string original = "not an integer";
  encode(original, buffer);

  ByteView view{buffer.data, buffer.size};
  int decoded;

  // String cannot be decoded as int
  EXPECT_THROW(decode(view, decoded), DecodeException);
}

TEST(SerializationTest, DecodeEmptyFromNonNilThrows)
{
  ByteBuffer buffer;
  int original = 42;
  encode(original, buffer);

  ByteView view{buffer.data, buffer.size};
  Empty decoded;

  // Empty can only be decoded from nil
  EXPECT_THROW(decode(view, decoded), DecodeException);
}

// =============================================
// Buffer Reuse Tests
// =============================================

TEST(SerializationTest, BufferReuseWorks)
{
  ByteBuffer buffer;

  // First encode
  int int_val = 100;
  encode(int_val, buffer);
  size_t first_size = buffer.size;

  // Second encode should reset buffer
  std::string str_val = "hello";
  encode(str_val, buffer);

  // Decode the string (not the int)
  ByteView view{buffer.data, buffer.size};
  std::string decoded;
  decode(view, decoded);

  EXPECT_EQ(decoded, str_val);
  EXPECT_NE(buffer.size, first_size); // Size should be different
}

// =============================================
// Large Data Tests
// =============================================

TEST(SerializationTest, LargeStringRoundTrip)
{
  ByteBuffer buffer;
  std::string original(10000, 'x'); // 10KB string
  std::string decoded;

  encode(original, buffer);
  ByteView view{buffer.data, buffer.size};
  decode(view, decoded);

  EXPECT_EQ(decoded, original);
}

TEST(SerializationTest, LargeVectorRoundTrip)
{
  ByteBuffer buffer;
  std::vector<int> original(1000);
  for (int i = 0; i < 1000; ++i)
  {
    original[i] = i * i;
  }

  std::vector<int> decoded;
  encode(original, buffer);
  ByteView view{buffer.data, buffer.size};
  decode(view, decoded);

  EXPECT_EQ(decoded, original);
}
