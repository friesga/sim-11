#ifndef _OVERLOADED_H_
#define _OVERLOADED_H_

// Definition of a helper class to combine multiple lambdas into one callable
// object. This allows std::visit to dispatch calls to the correct lambda based
// on the variant's type. Refer to C++17 - The Complete Guide, page 161.
// 
// ”Inherit” all function call operators of passed base types
template<typename... Ts> struct overloaded : Ts... { using Ts::operator()...; };

// explicit deduction guide (not needed as of C++20)
// template<typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;


#endif // _OVERLOADED_H_