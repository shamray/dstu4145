#pragma once

namespace dstu4145
{
    template<typename T, typename _ = void>
    struct is_container : std::false_type {};

    template<typename... Ts>
    struct is_container_helper {};

    template<typename T>
    struct is_container<
        T,
        std::conditional_t<
        false,
        is_container_helper<
            typename T::value_type,
            typename T::iterator,
            decltype(std::declval<T>().begin()),
            decltype(std::declval<T>().end())
        >,
        void
    >
    > : public std::true_type {};
}