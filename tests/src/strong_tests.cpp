#include <gtest/gtest.h>

#include <array>

#include "strong_type/strong_type.h"

TEST(StrongTypeTests, Constructor)
{
    using Number = strong::strong_type<struct NumberTag, uint8_t>;
    constexpr Number a{5};
    static_assert(a.get() == 5, "Invalid value.");
}

TEST(StrongTypeTests, Assignment)
{
    using Number = strong::strong_type<struct NumberTag, uint8_t,
                                       strong::assignment, strong::comparisons>;
    Number a{5}, b{10};
    a = b;
    ASSERT_EQ(a, b);
    ASSERT_EQ(a.get(), 10);
}

TEST(StrongTypeTests, Add)
{
    using Number = strong::strong_type<struct NumberTag, uint8_t, strong::plus>;
    constexpr Number a{5}, b{10};
    static_assert((a + b).get() == 15, "Invalid result of addition.");
}

TEST(StrongTypeTests, Substruct)
{
    using Number =
        strong::strong_type<struct NumberTag, int16_t, strong::minus>;
    constexpr Number a{5}, b{10};
    static_assert((a - b).get() == -5, "Invalid result of substruction.");
}

TEST(StrongTypeTests, Multiplication)
{
    using Number =
        strong::strong_type<struct NumberTag, int16_t, strong::multiplication>;
    constexpr Number a{5}, b{10};
    static_assert((a * b).get() == 50, "Invalid result of substruction.");
}

TEST(StrongTypeTests, Divide5By10)
{
    using Number =
        strong::strong_type<struct NumberTag, int16_t, strong::division>;
    constexpr Number a{5}, b{10};
    static_assert((a / b).get() == 0, "Invalid result of substruction.");
}

TEST(StrongTypeTests, Divide10By5)
{
    using Number =
        strong::strong_type<struct NumberTag, int16_t, strong::division>;
    constexpr Number a{10}, b{5};
    static_assert((a / b).get() == 2, "Invalid result of substruction.");
}

TEST(StrongTypeTests, Modulo10By5)
{
    using Number =
        strong::strong_type<struct NumberTag, int16_t, strong::modulo>;
    constexpr Number a{10}, b{5};
    static_assert((a % b).get() == 0, "Invalid result of substruction.");
}

TEST(StrongTypeTests, Modulo10By7)
{
    using Number =
        strong::strong_type<struct NumberTag, int16_t, strong::modulo>;
    constexpr Number a{10}, b{7};
    static_assert((a % b).get() == 3, "Invalid result of substruction.");
}

TEST(StrongTypeTests, UnaryMinus)
{
    using Number =
        strong::strong_type<struct NumberTag, int16_t, strong::unary_minus>;
    constexpr Number a{10};
    static_assert((-a).get() == -10, "Invalid result of substruction.");
}

TEST(StrongTypeTests, UnaryPlus)
{
    using Number =
        strong::strong_type<struct NumberTag, int16_t, strong::unary_plus>;
    constexpr Number a{-10};
    static_assert((+a).get() == -10, "Invalid result of substruction.");
}

TEST(StrongTypeTests, Compare)
{
    using Number =
        strong::strong_type<struct NumberTag, int16_t, strong::comparisons>;
    constexpr Number a{-10}, b{3}, c{0}, d{3};
    static_assert(a < b, "Invalid result of operation <");
    static_assert(a <= b, "Invalid result of operation <=");
    static_assert(c > a, "Invalid result of operation >");
    static_assert(b >= a, "Invalid result of operation >=");
    static_assert(c != b, "Invalid result of operation !=");
    static_assert(b == d, "Invalid result of operation ==");
}

TEST(StrongTypeTests, Indirection)
{
    using Indirection =
        strong::strong_type<struct IndirectionTag, uint8_t const *,
                            strong::indirection>;
    uint8_t value{10};
    Indirection valuePtr{&value};
    ASSERT_EQ(value, *valuePtr);
    ASSERT_EQ(&value, valuePtr.get());
}

TEST(StrongTypeTests, Subscription)
{
    using Subscription =
        strong::strong_type<struct SubscriptionTag, uint8_t const *,
                            strong::subscription>;
    constexpr std::size_t kCount = 5;
    std::array<uint8_t, kCount> valArray{1, 2, 3, 4, 5};
    Subscription valuePtr{valArray.data()};
    for (std::size_t i = 0; i < kCount; ++i)
    {
        ASSERT_EQ(valArray[i], valuePtr[i]);
    }
    ASSERT_EQ(valArray.data(), valuePtr.get());
}

TEST(StrongTypeTests, ImplicitlyConvertibleTo)
{
    using ImplicitlyConvertibleToUnderlying =
        strong::strong_type<struct ImplicitlyConvertibleToUnderlyingTag,
                            uint8_t const *,
                            strong::implicitly_convertible_to_underlying>;
    constexpr std::size_t kCount = 5;
    std::array<uint8_t, kCount> valArray{1, 2, 3, 4, 5};
    ImplicitlyConvertibleToUnderlying valuePtr{valArray.data()};
    uint8_t const *ptr = valuePtr + 1;
    ASSERT_EQ(ptr, valArray.data() + 1);
}

TEST(StrongTypeTests, PointerPlusValue)
{
    using Pointer =
        strong::strong_type<struct PointerTag, uint8_t const *,
                            strong::pointer_plus_value, strong::subscription>;
    constexpr std::size_t kCount = 5;
    constexpr int kOffset = 1;
    std::array<uint8_t, kCount> valArray{1, 2, 3, 4, 5};
    Pointer ptr{valArray.data()};
    auto shifted_ptr = ptr + kOffset;
    ASSERT_EQ(shifted_ptr.get(), valArray.data() + kOffset);
}

TEST(StrongTypeTests, ValuePlusPointer)
{
    using Pointer =
        strong::strong_type<struct PointerTag, uint8_t const *,
                            strong::value_plus_pointer, strong::subscription>;
    constexpr std::size_t kCount = 5;
    constexpr int kOffset = 1;
    std::array<uint8_t, kCount> valArray{1, 2, 3, 4, 5};
    Pointer ptr{valArray.data()};
    auto shifted_ptr = kOffset + ptr;
    ASSERT_EQ(shifted_ptr.get(), valArray.data() + kOffset);
}

TEST(StrongTypeTests, PointerMinusValue)
{
    using Pointer =
        strong::strong_type<struct PointerTag, uint8_t const *,
                            strong::pointer_minus_value, strong::subscription>;
    constexpr std::size_t kCount = 5;
    constexpr int kOffset = 1;
    std::array<uint8_t, kCount> valArray{1, 2, 3, 4, 5};
    Pointer ptr{valArray.data() + kOffset};
    auto shifted_ptr = ptr - kOffset;
    ASSERT_EQ(shifted_ptr.get(), valArray.data());
}

TEST(StrongTypeTests, PointerMinusPointer)
{
    using Pointer = strong::strong_type<struct PointerTag, uint8_t const *,
                                        strong::pointer_minus_pointer,
                                        strong::subscription>;
    constexpr std::size_t kCount = 5;
    constexpr int kOffset = 2;
    std::array<uint8_t, kCount> valArray{1, 2, 3, 4, 5};
    Pointer ptr1{valArray.data()};
    Pointer ptr2{valArray.data() + kOffset};
    const auto kDiff1 = ptr2 - ptr1;
    ASSERT_EQ(kDiff1, kOffset);

    const auto kDiff2 = ptr1 - ptr2;
    ASSERT_EQ(kDiff2, -kOffset);
}

TEST(StrongTypeTests, PointerPlusAssignment)
{
    using Pointer = strong::strong_type<struct PointerTag, uint8_t const *,
                                        strong::pointer_plus_assignment,
                                        strong::subscription>;
    constexpr std::size_t kCount = 5;
    constexpr int kOffset = 3;
    std::array<uint8_t, kCount> valArray{1, 2, 3, 4, 5};
    Pointer ptr{valArray.data()};
    ptr += kOffset;
    ASSERT_EQ(ptr.get(), valArray.data() + kOffset);
}

TEST(StrongTypeTests, PointerMinusAssignment)
{
    using Pointer = strong::strong_type<struct PointerTag, uint8_t const *,
                                        strong::pointer_minus_assignment,
                                        strong::subscription>;
    constexpr std::size_t kCount = 5;
    constexpr int kOffset = 3;
    std::array<uint8_t, kCount> valArray{1, 2, 3, 4, 5};
    Pointer ptr{valArray.data() + kOffset};
    ptr -= kOffset;
    ASSERT_EQ(ptr.get(), valArray.data());
}
