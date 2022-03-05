#include <type_traits>

namespace strong
{
template <typename Tag, typename T, template <typename> typename... Ops>
class strong_type : public Ops<strong_type<Tag, T, Ops...>>...
{
   public:
    strong_type() = default;
    explicit constexpr strong_type(T const& value) : value_(value) {}
    explicit constexpr strong_type(T&& value) noexcept(
        std::is_nothrow_move_constructible_v<T>)
        : value_(std::move(value))
    {
    }
    explicit constexpr operator T&() noexcept { return value_; }
    explicit constexpr operator const T&() const noexcept { return value_; }

    T& get() { return value_; }
    T const& get() const { return value_; }

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
};

template <typename T>
inline constexpr bool is_strong_v = is_strong<T>::value;

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
        return aLhs.get() < aRhs.get();
    }

    friend constexpr bool operator==(const StrongT& aLhs, const StrongT& aRhs)
    {
        return !(aLhs < aRhs) && !(aRhs < aLhs);
    }

    friend constexpr bool operator!=(const StrongT& aLhs, const StrongT& aRhs)
    {
        return !(aLhs == aRhs);
    }

    friend constexpr bool operator>(const StrongT& aLhs, const StrongT& aRhs)
    {
        return aRhs < aLhs;
    }

    friend constexpr bool operator<=(const StrongT& aLhs, const StrongT& aRhs)
    {
        return !(aRhs < aLhs);
    }

    friend constexpr bool operator>=(const StrongT& aLhs, const StrongT& aRhs)
    {
        return !(aLhs < aRhs);
    }
};

template <typename StrongT>
struct addition
{
    template <typename T>
    friend constexpr StrongT& operator+=(StrongT& aLhs, T&& aRhs) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        aLhs.get() += getValue(std::forward<T>(aRhs));
        return aLhs;
    }

    template <typename T>
    friend constexpr StrongT operator+(const StrongT& aLhs, T&& aRhs) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return StrongT(aLhs.get() + getValue(std::forward<T>(aRhs)));
    }
};

template <typename StrongT>
struct subtraction
{
    template <typename T>
    friend constexpr StrongT& operator-=(StrongT& aLhs, T&& aRhs) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        aLhs.get() -= getValue(std::forward<T>(aRhs));
        return aLhs;
    }

    template <typename T>
    friend constexpr StrongT operator-(const StrongT& aLhs, T&& aRhs) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        return StrongT(aLhs.get() - getValue(std::forward<T>(aRhs)));
    }
};

template <typename StrongT>
struct increment
{
    friend constexpr StrongT& operator++(StrongT& aValue)
    {
        using type = underlying_type<StrongT>;
        ++static_cast<type&>(aValue);
        return aValue;
    }

    friend constexpr StrongT operator++(StrongT& aValue, int)
    {
        return ++StrongT(aValue);
    }
};

template <typename StrongT>
struct decrement
{
    friend constexpr StrongT& operator--(StrongT& aValue)
    {
        using type = underlying_type<StrongT>;
        --static_cast<type&>(aValue);
        return aValue;
    }

    friend constexpr StrongT operator--(StrongT& aValue, int)
    {
        return --StrongT(aValue);
    }
};

template <typename StrongT>
struct unary_plus
{
    friend constexpr StrongT operator+(const StrongT& aValue) { return aValue; }
};

template <typename StrongT>
struct unary_minus
{
    friend constexpr StrongT operator-(const StrongT& aValue)
    {
        using type = underlying_type<StrongT>;
        static_assert(std::is_signed_v<type>,
                      "Negation a value of unsigned type is pointless action.");
        return StrongT(-static_cast<type&>(aValue));
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

    template <typename T>
    friend constexpr StrongT& operator*=(StrongT& aValue1, T&& aValue2) noexcept
    {
        static_assert(is_strong_v<StrongT>, "Invalid StrongT.");
        aValue1.get() *= getValue(std::forward<T>(aValue2));
        return aValue1;
    }
};
}  // namespace strong