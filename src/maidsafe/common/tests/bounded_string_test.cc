/*  Copyright 2012 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.maidsafe.net/licenses

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */

#include <cstdint>
#include <string>
#include <vector>

#include "maidsafe/common/bounded_string.h"

#include "maidsafe/common/convert.h"
#include "maidsafe/common/error.h"
#include "maidsafe/common/log.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/types.h"
#include "maidsafe/common/utils.h"
#include "maidsafe/common/serialisation/serialisation.h"

namespace maidsafe {

namespace detail {

namespace test {

template <typename T>
class BoundedStringTest : public testing::Test {
 protected:
  using OneOne = BoundedString<1, 1, T>;
  using OneTwo = BoundedString<1, 2, T>;
  using OneThree = BoundedString<1, 3, T>;
  using OneMax = BoundedString<1, -1, T>;
  using TwoTwo = BoundedString<2, 2, T>;
  using TwoThree = BoundedString<2, 3, T>;
  using TwoFour = BoundedString<2, 4, T>;
  using TwoMax = BoundedString<2, -1, T>;

  T RandomData(std::size_t size) const;
  T RandomData(std::uint32_t min, std::uint32_t max) const;
  std::string ToString(const T& input) const;
};

template <>
std::vector<byte> BoundedStringTest<std::vector<byte>>::RandomData(std::uint32_t min,
  std::uint32_t max) const {
  return RandomBytes(min, max);
}

template <>
std::string BoundedStringTest<std::string>::RandomData(std::uint32_t min, std::uint32_t max) const {
  return RandomString(min, max);
}

template <>
std::vector<byte> BoundedStringTest<std::vector<byte>>::RandomData(std::size_t size) const {
  return RandomBytes(size);
}

template <>
std::string BoundedStringTest<std::string>::RandomData(std::size_t size) const {
  return RandomString(size);
}

template <>
std::string BoundedStringTest<std::vector<byte>>::ToString(const std::vector<byte>& input) const {
  return convert::ToString(input);
}

template <>
std::string BoundedStringTest<std::string>::ToString(const std::string& input) const {
  return input;
}



using TestTypes = testing::Types<std::vector<byte>, std::string>;
TYPED_TEST_CASE(BoundedStringTest, TestTypes);

TYPED_TEST(BoundedStringTest, BEH_DefaultConstructor) {
  OneOne a;
  EXPECT_FALSE(a.IsInitialised());
  OneMax b;
  EXPECT_FALSE(b.IsInitialised());
}

TYPED_TEST(BoundedStringTest, BEH_Getters) {
  TwoTwo a;
  EXPECT_FALSE(a.IsInitialised());
  EXPECT_THROW(a.string(), common_error);

  for (int i(0); i < 1000; ++i) {
    auto random(RandomData(1, 1024));
    OneMax b(random);
    EXPECT_TRUE(b.IsInitialised());
    EXPECT_EQ(random, b.string());
  }
}

TYPED_TEST(BoundedStringTest, BEH_StringConstructor) {
  // Empty (invalid)
  EXPECT_THROW(OneOne a(""), common_error);
  // Valid
  std::string random(RandomString(1));
  OneOne b(random);
  EXPECT_EQ(random, ToString(b.string()));
  // Too big
  EXPECT_THROW(OneOne c(RandomString(2)), common_error);

  // Empty (invalid)
  EXPECT_THROW(OneMax d(""), common_error);
  // Valid
  random = RandomString(1, 1024);
  OneMax e(random);
  EXPECT_EQ(random, ToString(e.string()));
}

TYPED_TEST(BoundedStringTest, BEH_Swap) {
  // Swap with initialised
  auto random1(RandomData(1));
  auto random2(RandomData(2));
  OneTwo a(random1);
  OneTwo b(random2);
  swap(a, b);
  EXPECT_EQ(random2, a.string());
  EXPECT_EQ(random1, b.string());

  // Swap with uninitialised
  OneTwo c;
  swap(a, c);
  EXPECT_FALSE(a.IsInitialised());
  EXPECT_EQ(random2, c.string());
}

TYPED_TEST(BoundedStringTest, BEH_CopyConstruction) {
  // Copy from initialised
  auto random(RandomData(1, 1024));
  OneMax a(random);
  OneMax b(a);
  EXPECT_EQ(random, a.string());
  EXPECT_EQ(random, b.string());

  // Copy from uninitialised
  OneMax c;
  OneMax d(c);
  EXPECT_FALSE(d.IsInitialised());
}

TYPED_TEST(BoundedStringTest, BEH_MoveConstruction) {
  // Move from initialised
  auto random(RandomData(1, 1024));
  OneMax a(std::move(OneMax(random)));
  EXPECT_EQ(random, a.string());

  // Move from uninitialised
  OneMax b(std::move(OneMax()));
  EXPECT_FALSE(b.IsInitialised());
}

TYPED_TEST(BoundedStringTest, BEH_CopyAssignment) {
  // Assign from initialised
  auto random(RandomData(1, 1024));
  OneMax a(random);
  OneMax b("1");
  b = a;
  EXPECT_EQ(random, a.string());
  EXPECT_EQ(random, b.string());

  // Assign from self
  b = b;
  EXPECT_EQ(random, b.string());

  // Assign from uninitialised
  OneMax c;
  b = c;
  EXPECT_FALSE(b.IsInitialised());
}

TYPED_TEST(BoundedStringTest, BEH_MoveAssignment) {
  // Assign from initialised
  auto random(RandomData(1, 1024));
  OneMax a(random);
  OneMax b("1");
  b = std::move(a);
  EXPECT_EQ(random, b.string());

  // Assign from self
  b = std::move(b);
  EXPECT_EQ(random, b.string());

  // Assign from uninitialised
  OneMax c;
  b = std::move(c);
  EXPECT_FALSE(b.IsInitialised());
}

TYPED_TEST(BoundedStringTest, BEH_ConstructionFromDifferentType) {
  // Valid copy
  auto random(RandomData(2));
  TwoThree a(random);
  OneMax b(a);
  EXPECT_EQ(random, b.string());
  TwoFour c(std::move(a));
  EXPECT_EQ(random, c.string());

  // Copy from uninitialised
  TwoThree d;
  OneThree e(d);
  EXPECT_FALSE(e.IsInitialised());
  TwoFour f(std::move(d));
  EXPECT_FALSE(f.IsInitialised());
}

TYPED_TEST(BoundedStringTest, BEH_AssignmentFromDifferentType) {
  // Valid assignment
  auto random(RandomData(2));
  TwoThree a(random);
  OneMax b("1");
  b = a;
  EXPECT_EQ(random, b.string());
  TwoFour c("02");
  c = std::move(a);
  EXPECT_EQ(random, c.string());

  // Assign from uninitialised
  TwoThree d;
  OneThree e("1");
  e = d;
  EXPECT_FALSE(e.IsInitialised());
  TwoFour f("02");
  f = std::move(d);
  EXPECT_FALSE(f.IsInitialised());
}

TYPED_TEST(BoundedStringTest, BEH_Serialization) {
  // Invalid Serialisation
  OneThree a;
  EXPECT_FALSE(a.IsInitialised());
  EXPECT_THROW(Serialise(a), common_error);

  // Valid Serialisation
  OneThree b(RandomData(1));
  EXPECT_TRUE(b.IsInitialised());

  SerialisedData serialised_str;
  EXPECT_TRUE(serialised_str.empty());
  EXPECT_NO_THROW(serialised_str = Serialise(b));
  EXPECT_FALSE(serialised_str.empty());

  // Invalid Deserialisation
  TwoThree c;
  EXPECT_FALSE(c.IsInitialised());
  EXPECT_THROW(Parse(serialised_str, c), common_error);

  // Valid Deserialisation
  OneTwo d;
  EXPECT_FALSE(d.IsInitialised());
  EXPECT_NO_THROW(Parse(serialised_str, d));
  EXPECT_EQ(b.string(), d.string());
}

}  // namespace test

}  // namespace detail

}  // namespace maidsafe
