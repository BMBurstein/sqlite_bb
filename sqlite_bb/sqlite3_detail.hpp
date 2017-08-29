#pragma once

namespace bb {
namespace sqlite {
namespace detail {

template <typename Func> struct function_traits;

template <typename Ret, typename ... Ts>
struct function_traits<Ret(Ts...)> {
  constexpr static auto arity = sizeof...(Ts);
};

template <typename Ret, typename ... Ts>
struct function_traits<Ret(*)(Ts...)> : function_traits<Ret(Ts...)> {};

template <typename C, typename Ret, typename ... Ts>
struct function_traits<Ret(C::*)(Ts...) const> : function_traits<Ret(Ts...)> {};

template <typename C>
struct function_traits : function_traits<decltype(&C::operator())> {};

template <typename F, typename Params, std::size_t ... Is>
void call(const F& f, Params&& p, std::index_sequence<Is...>) {
  f(p[Is]...);
}

} // namespace detail
} // namespace sqlite
} // namespace bb
