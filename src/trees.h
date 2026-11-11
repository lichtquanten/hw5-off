#pragma once

// Ready-made AVL and splay trees with a std::set/std::map-style interface.
// Boost headers are bundled in .guides/third_party; no installation or linking
// against a separate Boost library is needed with the provided build.
//
// Examples:
//   AVLSet<int> positions;
//   SplaySet<int> visited;
//   AVLMap<int, double> scores;
//   SplayMap<int, double> signals;
//
// Use insert, find, erase, lower_bound, and ordered iteration. Maps also support
// operator[]. Keys are unique and ordered by Compare. For a custom ordering,
// use e.g. AVLSet<int, std::greater<int>>.

#include <boost/container/map.hpp>
#include <boost/container/options.hpp>
#include <boost/container/set.hpp>

#include <functional>
#include <memory>
#include <utility>

namespace shipwreck_tree_detail {

using AVLOptions = boost::container::tree_assoc_options<
    boost::container::tree_type<boost::container::avl_tree>>::type;

using SplayOptions = boost::container::tree_assoc_options<
    boost::container::tree_type<boost::container::splay_tree>>::type;

}  // namespace shipwreck_tree_detail

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

