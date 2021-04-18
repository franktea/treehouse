#include <functional>
#include <type_traits>
#include <tuple>
#include <new>
#include <optional>
#include <exception>
#include <concepts>
#include <cstdio>

using std::puts;
using std::printf;
using std::invoke_result_t;

using std::constructible_from;
using std::move_constructible;
using std::copy_constructible;
using std::derived_from;
using std::invocable;

using std::move;

inline namespace _receiver_cpos_ {

inline constexpr struct _set_value_fn_ {
    template<class R, class... As>
        requires requires(R&& r, As&&... as) {
            move(r).set_value(move(as)...);
        }
    void operator()(R&& r, As&&... as) const
        noexcept(noexcept(move(r).set_value(move(as)...))) 
    {
        move(r).set_value(move(as)...);
    }
} set_value {};

inline constexpr struct _set_error_fn_ {
    template<class R, class E>
        requires requires(R&& r, E&& e) {
            { std::move(r).set_error(std::move(e)) } noexcept;
        }
    void operator()(R&& r, E&& e) const noexcept
    {
        std::move(r).set_error(std::move(e));
    } 
} set_error{};

inline constexpr struct _set_done_fn_ {
    template<class R>
        requires requires(R&& r) {
            { std::move(r).set_done() } noexcept;
        }
    void operator()(R&& r) const noexcept
    {
        std::move(r).set_done();
    }
} set_done {};

} // namespace _receiver_cpos_

// receiver是被拆成了两个，set_value是被放在了receiver_of中
template<class R, class E = std::exception_ptr>
concept receiver = move_constructible<std::remove_cvref_t<R>> &&
    requires(R&& r, E&& e) {
        ::set_error(move(r), move(e));
        ::set_done(move(r));
    };

template<class R, class... As>
concept receiver_of = 
    receiver<R> &&
    requires(R&& r, As&&... as) {
        ::set_value(move(r), move(as)...);
    };

template<class R, class... As>
inline constexpr bool is_nothrow_receiver_of_v = 
    noexcept(::set_value(std::declval<R>(), std::declval<As>()...));

struct sender_base {};

template<class S>
struct sender_traits;

template<class S>
    requires derived_from<S, sender_base>
struct sender_traits<S> {};

template<class S>
concept sender = 
    move_constructible<std::remove_cvref_t<S>> &&
    requires {
        sizeof(sender_traits<std::remove_cvref_t<S>>);
    };

static_assert(!sender<int>);

inline namespace _sender_cpos_ {
inline constexpr struct _start_fn_ {
    template<class O>
        requires requires(O&& o) {
            { move(o).start() } noexcept;
        }
    void operator()(O&& o) const noexcept
    {
        move(o).start();
    }
} start {};
} // namespace _sender_cpos_

template<class O>
concept operation_state = 
    std::is_object_v<O> &&
    requires (O&& o) {
        ::start(move(o));
    };

inline namespace _sender_cpos_ {
inline constexpr struct _connect_fn_ {
    template<sender S, receiver R>
        requires requires(S&& s, R&& r) {
            {move(s).connect(move(r))} -> operation_state;
        }
    auto operator()(S&& s, R&& r) const
        noexcept(noexcept(move(s).connect(move(r))))
    {
        return move(s).connect(move(r));
    }
} connect {};
} // namespace _sender_cpos_

template<class S, class R>
concept sender_to = 
    sender<S> &&
    receiver<R> &&
    requires(S&& s, R&& r) {
        ::connect(move(s), move(r));
    };

template<class S, class R>
    requires sender_to<S, R>
using state_t = decltype(::connect(std::declval<S>(), std::declval<R>()));

// "then" algorithm implemention begin
template<receiver R, class F>
struct _then_receiver {
    R r_;
    F f_;
    _then_receiver(R r, F f):r_(move(r)), f_(move(f)) {}

    // 针对F返回值未非void的重载
    template<class... As, class Ret = invoke_result_t<F, As...>>
        requires receiver_of<R, Ret>
    void set_value(As&&... as) &&
        noexcept(std::is_nothrow_invocable_v<F, As...> &&
        is_nothrow_receiver_of_v<R, Ret>)
    {
        ::set_value(move(r_), std::invoke(move(f_), move(as)...));
    }

    // 针对F返回值未void类型的重载
    template<class... As, class Ret=invoke_result_t<F, As...>>
        requires receiver_of<R> // 这里不能是receiver_of<R, Ret>，否则编不过
        && std::is_void_v<Ret>
    void set_value(As&&... as) &&
        noexcept(std::is_nothrow_invocable_v<F, As...> &&
        is_nothrow_receiver_of_v<R>)
    {
        std::invoke(move(f_), move(as)...);
        ::set_value(move(r_));
    }

    template<class E>
        requires receiver<R, E>
    void set_error(E&& e) noexcept
    {
        ::set_error(move(r_), move(e));
    }

    void set_done() && noexcept
    {
        ::set_done(move(r_));
    }
};

template<sender S, class F>
struct _then_sender : sender_base {
    S s_;
    F f_;
    _then_sender(S s, F f): s_(move(s)), f_(move(f)) {}

    template<receiver R>
        requires sender_to<S, _then_receiver<R, F>>
    auto connect(R r) && -> state_t<S, _then_receiver<R, F>>
    {
        return ::connect(move(s_), _then_receiver{move(r), move(f_)});
    }
};

template<sender S, class F>
sender auto then(S s, F f) {
    return _then_sender{move(s), move(f)};
}
// "then" algorithm implemention end

///////////////////////////////////////////////////////////////////
// sender "just" begin
template<move_constructible... Ts>
struct just: sender_base {
    std::tuple<Ts...> ts_;
    explicit(sizeof...(Ts)==1) constexpr just(Ts... ts) : 
        ts_{move(ts)...} {}
    
    template<receiver_of<Ts...> R>
    struct _op {
        R r_;
        std::tuple<Ts...> ts_;
        void start() && noexcept try {
            std::apply([this](Ts&&... ts) mutable {
                ::set_value(move(r_), move(ts)...);
            }, move(ts_));
        } catch(...) {
            ::set_error(move(r_), std::current_exception());
        }
    };

    template<receiver_of<Ts...> R>
        requires (copy_constructible<Ts>&& ...)
    auto connect(R r) const & -> _op<R> {
        return _op<R>{move(r), ts_};
    }

    template<receiver_of<Ts...> R>
    auto connect(R r) && -> _op<R> {
        return _op<R>{move(r), move(ts_)};
    }
};
// sender "just" end
///////////////////////////////////////////////////////////////////

inline constexpr struct _sink {
    void set_value(auto&& ...) const noexcept {}
    [[noreturn]] void set_error(auto&&) const noexcept {
        std::terminate();
    }

    [[noreturn]] void set_done() const noexcept {
        std::terminate();
    }
} sink{};

int main() {
    sender auto j = just(1, 2, 3);
    auto f = [](int i, int j, int k) noexcept {
        printf("i=%d, j=%d, k=%d\n", i, j, k);
    };

    sender auto t = ::then(j, f);

    operation_state auto s = ::connect(t, sink);

    ::start(s);

    ::start(::connect(::then(j, f), sink));
}