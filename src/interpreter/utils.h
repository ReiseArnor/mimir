#ifndef UTILS_H
#define UTILS_H
#include <algorithm>
#include <array>
#include <cstdarg>
#include <string>
#include <string_view>

template <class T> struct NameValuePair {
  using value_type = T;
  const T value;
  const std::string_view name;
};

template <class Mapping, class V>
static auto getNameForValue(Mapping map, V value) -> std::string_view
{
  auto pos = std::find_if(std::begin(map), std::end(map),
                          [&value](const typename Mapping::value_type &type) {
                            return (type.value == value);
                          });
  return pos->name;
}

template <typename Key, typename Value, std::size_t Size> struct Map {
  std::array<std::pair<Key, Value>, Size> data;

  [[nodiscard]] constexpr auto at(const Key &key) const -> Value
  {
    const auto itr =
        std::find_if(begin(data), end(data),
                     [&key](const auto &val) { return val.first == key; });
    return itr->second;
  };

  [[nodiscard]] constexpr auto find(const Key &key) const -> bool
  {
    const auto itr =
        std::find_if(begin(data), end(data),
                     [&key](const auto &val) { return val.first == key; });
    return static_cast<bool>(itr != end(data));
  };
};
#endif // UTILS_H
