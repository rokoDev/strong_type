#ifndef strong_type_h
#define strong_type_h

#include <type_traits>
#include <utility>

namespace strong
{
template <typename Tag, typename T, template <typename> typename... Ops>
class strong_type : public Ops<strong_type<Tag, T, Ops...>>...
{
   public:
    using value_type = T;
    strong_type() = default;
    explicit constexpr strong_type(T const& value) noexcept(
        std::is_nothrow_copy_constructible_v<T>)
        : value_(value)
    {
    }
    explicit constexpr strong_type(T&& value) noexcept(
        std::is_nothrow_move_constructible_v<T>)
        : value_(std::move(value))
    {
    }
    explicit constexpr operator T&() noexcept { return value_; }
    explicit constexpr operator const T&() const noexcept { return value_; }

    constexpr T& get() { return value_; }
    constexpr T const& get() const { return value_; }

   private:
    T value_;
};

template <typename Tag, typename T, template <typename> typename... Ops>
T underlying_type_impl(strong_type<Tag, T, Ops...>);

template <typename T>
using underlying_type = decltype(underlying_type_impl(std::declval<T>()));

template <typename T>
struct is_strong : std::false_type
{
};

template <typename Tag, typename T, template <typename> typename... Ops>
struct is_strong<strong_type<Tag, T, Ops...>> : std::true_type
{
    using type = T;
};

template <typename T>
inline constexpr bool is_strong_v = is_strong<T>::value;

template <typename T>
using is_strong_t = typename is_strong<T>::type;

template <typename T>
constexpr decltype(auto) getValue(T&& aValue) noexcept
{
    if constexpr (is_strong_v<std::remove_cv_t<std::remove_reference_t<T>>>)
    {
        return aValue.get();
    }
    else
    {
        return std::forward<T>(aValue);
    }
}

namespace details
{
template <typename StrongT>
inline constexpr StrongT advance(const StrongT& aLhs,
                                 std::ptrdiff_t aCount) noexcept
{
    static_assert(strong::is_strong_v<StrongT>, "Invalid StrongT.");
    using value_type = strong::underlying_type<StrongT>;
    static_assert(std::is_pointer_v<value_type>);
    return StrongT(aLhs.get() + aCount);
}
}  // namespace details

template <typename StrongT>
struct convertible_to_bool
{
    explicit constexpr operator bool() const noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return static_cast<const StrongT&>(*this).get();
    }
};

template <typename StrongT>
struct comparisons
{
    friend constexpr bool operator<(const StrongT& aLhs, const StrongT& aRhs)
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return aLhs.get() < aRhs.get();
    }

    friend constexpr bool operator==(const StrongT& aLhs, const StrongT& aRhs)
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return aLhs.get() == aRhs.get();
    }

    friend constexpr bool operator!=(const StrongT& aLhs, const StrongT& aRhs)
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return aLhs.get() != aRhs.get();
    }

    friend constexpr bool operator>(const StrongT& aLhs, const StrongT& aRhs)
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return aLhs.get() > aRhs.get();
    }

    friend constexpr bool operator<=(const StrongT& aLhs, const StrongT& aRhs)
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return aLhs.get() <= aRhs.get();
    }

    friend constexpr bool operator>=(const StrongT& aLhs, const StrongT& aRhs)
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return aLhs.get() >= aRhs.get();
    }
};

template <typename StrongT>
struct assignment
{
    constexpr StrongT& operator=(const StrongT& aValue) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        if (this == &aValue)
        {
            return *this;
        }
        StrongT& ref = static_cast<StrongT&>(*this);
        ref.get() = aValue.get();
        return ref;
    }
};

template <typename StrongT>
struct plus
{
    friend constexpr StrongT operator+(const StrongT& aLhs,
                                       const StrongT& aRhs) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return StrongT(aLhs.get() + aRhs.get());
    }
};

template <typename StrongT>
struct plus_assignment
{
    friend constexpr StrongT& operator+=(StrongT& aLhs,
                                         const StrongT& aRhs) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        aLhs.get() += aRhs.get();
        return aLhs;
    }
};

template <typename StrongT>
struct minus
{
    friend constexpr StrongT operator-(const StrongT& aLhs,
                                       const StrongT& aRhs) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return StrongT(aLhs.get() - aRhs.get());
    }
};

template <typename StrongT>
struct minus_assignment
{
    friend constexpr StrongT& operator-=(StrongT& aLhs,
                                         const StrongT& aRhs) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        aLhs.get() -= aRhs.get();
        return aLhs;
    }
};

template <typename StrongT>
struct pre_increment
{
    friend constexpr StrongT& operator++(StrongT& aValue)
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return ++aValue.get();
    }
};

template <typename StrongT>
struct post_increment
{
    friend constexpr StrongT operator++(StrongT& aValue, int)
    {
        return ++StrongT(aValue);
    }
};

template <typename StrongT>
struct pre_decrement
{
    friend constexpr StrongT& operator--(StrongT& aValue)
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return --aValue.get();
    }
};

template <typename StrongT>
struct post_decrement
{
    friend constexpr StrongT operator--(StrongT& aValue, int)
    {
        return --StrongT(aValue);
    }
};

template <typename StrongT>
struct unary_plus
{
    friend constexpr StrongT operator+(const StrongT& aValue)
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return aValue;
    }
};

template <typename StrongT>
struct unary_minus
{
    friend constexpr StrongT operator-(const StrongT& aValue)
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        using type = underlying_type<StrongT>;
        static_assert(std::is_signed_v<type>,
                      "Negation a value of unsigned type is pointless action.");
        return StrongT(-aValue.get());
    }
};

template <typename StrongT>
struct modulo
{
    template <typename T>
    friend constexpr StrongT operator%(const StrongT& aValue,
                                       T&& aDivider) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return StrongT(aValue.get() % getValue(std::forward<T>(aDivider)));
    }
};

template <typename StrongT>
struct modulo_assignment
{
    template <typename T>
    friend constexpr StrongT& operator%=(StrongT& aValue, T&& aDivider) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        aValue.get() %= getValue(std::forward<T>(aDivider));
        return aValue;
    }
};

template <typename StrongT>
struct division
{
    template <typename T>
    friend constexpr StrongT operator/(const StrongT& aValue,
                                       T&& aDivider) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return StrongT(aValue.get() / getValue(std::forward<T>(aDivider)));
    }
};

template <typename StrongT>
struct division_assignment
{
    template <typename T>
    friend constexpr StrongT& operator/=(StrongT& aValue, T&& aDivider) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        aValue.get() /= getValue(std::forward<T>(aDivider));
        return aValue;
    }
};

template <typename StrongT>
struct multiplication
{
    template <typename T>
    friend constexpr StrongT operator*(const StrongT& aValue1,
                                       T&& aValue2) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return StrongT(aValue1.get() * getValue(std::forward<T>(aValue2)));
    }
};

template <typename StrongT>
struct multiplication_assignment
{
    template <typename T>
    friend constexpr StrongT& operator*=(StrongT& aValue1, T&& aValue2) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        aValue1.get() *= getValue(std::forward<T>(aValue2));
        return aValue1;
    }
};

template <typename StrongT>
struct bitwise_not
{
    friend constexpr StrongT operator~(const StrongT& aValue) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        StrongT result(~aValue.get());
        return result;
    }
};

template <typename StrongT>
struct bitwise_and
{
    template <typename T>
    friend constexpr StrongT operator&(const StrongT& aValue1,
                                       const T& aValue2) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return StrongT(aValue1.get() & getValue(aValue2));
    }
};

template <typename StrongT>
struct bitwise_and_assignment
{
    template <typename T>
    friend constexpr StrongT operator&=(const StrongT& aValue1,
                                        const T& aValue2) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        aValue1.get() &= getValue(aValue2);
        return aValue1;
    }
};

template <typename StrongT>
struct bitwise_or
{
    template <typename T>
    friend constexpr StrongT operator|(const StrongT& aValue1,
                                       const T& aValue2) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return StrongT(aValue1.get() | getValue(aValue2));
    }
};

template <typename StrongT>
struct bitwise_or_assignment
{
    template <typename T>
    friend constexpr StrongT operator|=(const StrongT& aValue1,
                                        const T& aValue2) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        aValue1.get() |= getValue(aValue2);
        return aValue1;
    }
};

template <typename StrongT>
struct bitwise_xor
{
    template <typename T>
    friend constexpr StrongT operator^(const StrongT& aValue1,
                                       const T& aValue2) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return StrongT(aValue1.get() ^ getValue(aValue2));
    }
};

template <typename StrongT>
struct bitwise_xor_assignment
{
    template <typename T>
    friend constexpr StrongT operator^=(const StrongT& aValue1,
                                        const T& aValue2) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        aValue1.get() ^= getValue(aValue2);
        return aValue1;
    }
};

template <typename StrongT>
struct bitwise_left_shift
{
    template <typename T>
    friend constexpr StrongT operator<<(const StrongT& aValue1,
                                        const T& aValue2) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return StrongT(aValue1.get() << getValue(aValue2));
    }
};

template <typename StrongT>
struct bitwise_left_shift_assignment
{
    template <typename T>
    friend constexpr StrongT operator<<=(const StrongT& aValue1,
                                         const T& aValue2) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        aValue1.get() <<= getValue(aValue2);
        return aValue1;
    }
};

template <typename StrongT>
struct bitwise_right_shift
{
    template <typename T>
    friend constexpr StrongT operator>>(const StrongT& aValue1,
                                        const T& aValue2) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return StrongT(aValue1.get() >> getValue(aValue2));
    }
};

template <typename StrongT>
struct bitwise_right_shift_assignment
{
    template <typename T>
    friend constexpr StrongT operator>>=(const StrongT& aValue1,
                                         const T& aValue2) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        aValue1.get() >>= getValue(aValue2);
        return aValue1;
    }
};

template <typename StrongT>
struct indirection
{
    friend constexpr auto& operator*(StrongT aValue) { return *(aValue.get()); }
};

template <typename StrongT>
struct subscription
{
    constexpr auto& operator[](std::size_t aIndex)
    {
        StrongT& ref = static_cast<StrongT&>(*this);
        return *(ref.get() + aIndex);
    }
};

template <typename StrongT>
struct implicitly_convertible_to_underlying
{
    using Destination = is_strong_t<StrongT>;
    constexpr operator Destination() const
    {
        const StrongT& ref = static_cast<const StrongT&>(*this);
        return ref.get();
    }
};

template <typename StrongT>
struct pointer_plus_value
{
    friend constexpr StrongT operator+(const StrongT& aLhs,
                                       std::ptrdiff_t aValue) noexcept
    {
        return details::advance(aLhs, aValue);
    }
};

template <typename StrongT>
struct value_plus_pointer
{
    friend constexpr StrongT operator+(std::ptrdiff_t aValue,
                                       const StrongT& aLhs) noexcept
    {
        return details::advance(aLhs, aValue);
    }
};

template <typename StrongT>
struct pointer_minus_value
{
    friend constexpr StrongT operator-(const StrongT& aLhs,
                                       std::ptrdiff_t aValue) noexcept
    {
        return details::advance(aLhs, -aValue);
    }
};

template <typename StrongT>
struct pointer_minus_pointer
{
    friend constexpr std::ptrdiff_t operator-(const StrongT& aLhs,
                                              const StrongT& aRhs) noexcept
    {
        static_assert(strong::is_strong_v<StrongT>, "Invalid StrongT.");
        using value_type = strong::underlying_type<StrongT>;
        static_assert(std::is_pointer_v<value_type>);
        return aLhs.get() - aRhs.get();
    }
};
}  // namespace strong

#endif /* strong_type_h */
