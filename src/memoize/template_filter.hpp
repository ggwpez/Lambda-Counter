#ifndef TEMPLATE_LIST_FILTER_HPP
#define TEMPLATE_LIST_FILTER_HPP

#include <type_traits>
#include <tuple>

template <template <typename> typename, typename...>
struct filter;

template <template <typename> typename Pred>
struct filter<Pred>
{
    using type = std::tuple<>;
};

template <template <typename> typename Pred, typename X, typename... XS>
struct filter<Pred, X, XS...>
{
    template <typename, typename...>
    struct Cons;

    template <typename Y, typename... YS>
    struct Cons<Y, std::tuple<YS...>>
    {
        using type = std::tuple<Y, YS...>;
    };

    using type = typename std::conditional<Pred<X>::type::value,
                               typename Cons<X, typename filter<Pred, XS...>::type>::type,
                               typename filter<Pred, XS...>::type
                          >::type;
};

#endif // TEMPLATE_LIST_FILTER_HPP































