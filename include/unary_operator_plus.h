#ifndef _UNARY_OPERATOR_PLUS_H_
#define _UNARY_OPERATOR_PLUS_H_

// Definition of operator+ for scoped enums to convert it to the underlying
// type.
// Source: https://stackoverflow.com/questions/8357240/how-to-automatically-convert-strongly-typed-enum-into-int
//
template <typename T>
constexpr auto operator+ (T e) noexcept
-> std::enable_if_t<std::is_enum<T>::value, std::underlying_type_t<T>>
{
    return static_cast<std::underlying_type_t<T>> (e);
}


#endif // _UNARY_OPERATOR_PLUS_H_