#pragma once

#include <type_traits>

template <typename T>
std::enable_if_t<std::is_enum_v<T>, T> operator |(T const& lhs, T const& rhs)
{
	using t = std::underlying_type_t<T>;
	return static_cast<T>(static_cast<t>(lhs) | static_cast<t>(rhs));
}

template <typename T>
std::enable_if_t<std::is_enum_v<T>, T> operator ^(T const& lhs, T const& rhs)
{
	using t = std::underlying_type_t<T>;
	return static_cast<T>(static_cast<t>(lhs) ^ static_cast<t>(rhs));
}

template <typename T>
std::enable_if_t<std::is_enum_v<T>, T> operator &(T const& lhs, T const& rhs)
{
	using t = std::underlying_type_t<T>;
	return static_cast<T>(static_cast<t>(lhs) & static_cast<t>(rhs));
}

template <typename T>
std::enable_if_t<std::is_enum_v<T>, T> operator ~(T const& v)
{
	using t = std::underlying_type_t<T>;
	return static_cast<T>(~static_cast<t>(v));
}