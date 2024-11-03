#ifndef LOX_DEFS_H
#define LOX_DEFS_H

#include <string_view>
#include <variant>
#include <type_traits>
#include <cassert>
#ifndef NDEBUG
#include <iostream>
#endif

#ifdef __cpp_exceptions
#define LOX_EXCEPTION_ENABLED
#endif

#ifdef NDEBUG
#define LOX_NOEXCEPT noexcept
#else
#define LOX_NOEXCEPT
#endif

#define LOX_UNUSED(x) (void)(x)

namespace lox {

// FIXME: std::string_view may cause problems in the future.
using object = std::variant<std::monostate,
  std::string,
  std::string_view,
  double,
  bool,
  std::nullptr_t>;

template<typename T, typename E>
class expected {
public:
    explicit expected(E&& error) LOX_NOEXCEPT
      : m_error{ std::make_optional(std::forward<E>(error)) }
    {
    }

    explicit expected(T&& value) LOX_NOEXCEPT
      : m_value{ std::make_optional(std::forward<T>(value)) }
    {
    }

    ~expected() LOX_NOEXCEPT
    {
        const bool ok = !m_value.has_value() && !m_error.has_value();
#ifndef NDEBUG
        if (!ok) {
            // TODO: Use a logger.
            std::cerr
              << "[lox::expected] An unexpected value was not handled.\n";
        }

        assert(ok);
#endif
#ifdef NDEBUG
        if (!ok) {
            std::abort();
        }
#endif
    }

    expected(expected&&) = delete;
    expected& operator=(expected&&) = delete;

    expected(const expected&) = delete;
    expected& operator=(const expected&) = delete;

    template<typename Callback>
    [[nodiscard]]
    expected& and_then(Callback&& callback)
    {
        if (m_value.has_value()) {
            callback(m_value.value());
            m_value.reset();
        }

        return *this;
    }

    template<typename Callback>
    [[maybe_unused]]
    void or_else(Callback&& callback)
    {
        if (m_error.has_value()) {
            callback(m_error.value());
            m_error.reset();
        }
    }

private:
    std::optional<T> m_value{};
    std::optional<E> m_error{};
};

template<class T>
class copyable {
private:
    using NoPtr_T = typename std::remove_pointer<T>::type;

private:
    template<typename... Args>
    [[nodiscard]] constexpr T constrcut(Args&&... args) LOX_NOEXCEPT
    {
        if constexpr (std::is_pointer<T>::value) {
            constexpr std::size_t argc = sizeof...(Args);
            if constexpr (argc > 0) {
                return new NoPtr_T{ std::forward<Args>(args)... };
            }

            return nullptr;
        }
        else {
            return T{ std::forward<Args>(args)... };
        }
    }

public:
    template<typename... Args>
    explicit constexpr copyable(Args&&... args) LOX_NOEXCEPT
      : m_value{ constrcut(std::forward<Args>(args)...) }
    {
    }

    copyable() LOX_NOEXCEPT : m_value{ constrcut() }
    {
    }

    ~copyable()
    {
        if constexpr (std::is_pointer<T>::value) {
            if (m_value) {
                delete m_value;
            }
        }
    }

    copyable(const copyable& other) LOX_NOEXCEPT
    {
        if constexpr (std::is_pointer<T>::value) {
            if (other.m_value) {
                m_value = new NoPtr_T{ *other.m_value };
            }
            else {
                m_value = nullptr;
            }
        }
        else {
            m_value = other.m_value;
        }
    }

    copyable(copyable&& other) LOX_NOEXCEPT
    {
        m_value = std::move(other.m_value);
        if constexpr (std::is_pointer<T>::value) {
            other.m_value = nullptr;
        }
    }

    template<typename = std::enable_if<std::is_pointer<T>::value>>
    explicit copyable(std::nullptr_t)
      : m_value{ nullptr }
    {
    }

    copyable& operator=(const copyable& other) LOX_NOEXCEPT
    {
        if constexpr (std::is_pointer<T>::value) {
            if (m_value) {
                delete m_value;
            }

            if (other.m_value) {
                m_value = new NoPtr_T{ *other.m_value };
            }
            else {
                m_value = nullptr;
            }
        }
        else {
            m_value = other.m_value;
        }

        return *this;
    }

    copyable& operator=(copyable&& other) LOX_NOEXCEPT
    {
        if constexpr (std::is_pointer<T>::value) {
            if (m_value) {
                delete m_value;
            }
        }

        m_value = std::move(other.m_value);
        if constexpr (std::is_pointer<T>::value) {
            other.m_value = nullptr;
        }

        return *this;
    }

    [[nodiscard]] operator bool() const LOX_NOEXCEPT
    {
        if constexpr (std::is_pointer<T>::value) {
            return m_value != nullptr;
        }
        else {
            return !!m_value;
        }
    }

    [[nodiscard]] operator T() const LOX_NOEXCEPT
    {
        return m_value;
    }

    [[nodiscard]] operator const NoPtr_T&() const LOX_NOEXCEPT
    {
        if constexpr (std::is_pointer<T>::value) {
            assert(m_value);
            return *m_value;
        }
        else {
            return &m_value;
        }
    }

    [[nodiscard]] typename std::remove_pointer<T>::type* operator->() const
      LOX_NOEXCEPT
    {
        if constexpr (std::is_pointer<T>::value) {
            return m_value;
        }
        else {
            return &m_value;
        }
    }

    [[nodiscard]] typename std::remove_pointer<T>::type& operator*() const
      LOX_NOEXCEPT
    {
        if constexpr (std::is_pointer<T>::value) {
            assert(m_value);
            return *m_value;
        }
        else {
            return &m_value;
        }
    }

private:
    T m_value;
};

static_assert(std::is_copy_assignable_v<copyable<int>>, "Requirement error.");
static_assert(std::is_copy_constructible_v<copyable<int>>,
  "Requirement error.");
static_assert(std::is_move_assignable_v<copyable<int>>, "Requirement error.");
static_assert(std::is_move_constructible_v<copyable<int>>,
  "Requirement error.");
}

#endif
