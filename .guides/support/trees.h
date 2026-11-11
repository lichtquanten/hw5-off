#pragma once

#include <boost/container/map.hpp>
#include <boost/container/options.hpp>
#include <boost/container/set.hpp>

#include <functional>
#include <memory>
#include <utility>

namespace shipwreck_tree_detail
{

    using AVLOptions = boost::container::tree_assoc_options<
        boost::container::tree_type<boost::container::avl_tree>>::type;

    using SplayOptions = boost::container::tree_assoc_options<
        boost::container::tree_type<boost::container::splay_tree>>::type;

}

template <typename T, typename Compare = std::less<T>>
using AVLSet = boost::container::set<
    T, Compare, std::allocator<T>, shipwreck_tree_detail::AVLOptions>;

template <typename T, typename Compare = std::less<T>>
using SplaySet = boost::container::set<
    T, Compare, std::allocator<T>, shipwreck_tree_detail::SplayOptions>;

template <typename Key, typename Value, typename Compare = std::less<Key>>
using AVLMap = boost::container::map<
    Key, Value, Compare, std::allocator<std::pair<const Key, Value>>,
    shipwreck_tree_detail::AVLOptions>;

template <typename Key, typename Value, typename Compare = std::less<Key>>
using SplayMap = boost::container::map<
    Key, Value, Compare, std::allocator<std::pair<const Key, Value>>,
    shipwreck_tree_detail::SplayOptions>;
