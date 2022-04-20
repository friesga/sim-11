#ifndef _CONDDATA_H_
#define _CONDDATA_H_

#include <string>

// A template class conditionally containing data, inspired by std::optional.
template <typename T>
class CondData
{
    T value_;
    bool validValue_;

public:
    // Constructors
    CondData ();
    CondData(T value);
    CondData(T value, bool validValue);
    template <typename V>
        CondData(CondData<V> other);

    // Assignment operators
    CondData<T>& operator= (T value);
    CondData<T>& operator+= (T value);
    CondData<T>& operator-= (T value);

    // Conversion operator
    operator T() const;

    // Accessors
    bool hasValue() const;
    T value() const;
    T valueOr(T value) const;
};

// The default constructor constructs an object without valid data.
template <typename T>
inline CondData<T>::CondData()
    :
    value_{},
    validValue_{false}
{}

// Construct an object with valid data.
template <typename T>
inline CondData<T>::CondData(T value)
    :
    value_{value},
    validValue_{true}
{}

// Construct an object with data and given validity.
template <typename T>
inline CondData<T>::CondData(T value, bool validValue)
    :
    value_{value},
    validValue_{validValue}
{}

// Construct a CondData object from a CondData object with another native type
// The initialization of the data members is performed in the body of the
// constructor as initialization via the initialization list with MSVC generates
// compiler error C2397.
template <typename T>
template <typename V>
inline CondData<T>::CondData(CondData<V> other)
{
    value_ = static_cast<V> (other.valueOr(0));
    validValue_ = other.hasValue();
}

// Copy assign a native value
template <typename T>
inline CondData<T>& CondData<T>::operator= (T value)
{
    value_ = value;
    validValue_ = true;
    return *this;
}

// Add native type to this object
template <typename T>
inline CondData<T>& CondData<T>::operator+= (T value)
{
    if (validValue_)
        value_ += value;
    else
        throw (std::string("Addition on nullopt"));
    return *this;
}

// Subract native type from this object
template <typename T>
inline CondData<T>& CondData<T>::operator-= (T value)
{
    if (validValue_)
        value_ -= value;
    else
        throw (std::string("Addition on invalid CondData object"));
    return *this;
}

// Try to convert the object to the native type, throwing an exception
// if it doesn't contain a valid value.
template <typename T>
inline CondData<T>::operator T() const
{
    if (!validValue_)
        throw (std::string("Subtraction on invalid CondData object"));

    return value_;
}


// Return the valid value status to the caller
template <typename T>
inline bool CondData<T>::hasValue() const
{
    return validValue_;
}

// Return the value to the caller if it is valid
template <typename T>
inline T CondData<T>::value() const
{
    if (!validValue_)
        throw (std::string("CondData object has no valid value"));
    return value_;    
}

// Return the value to the caller if it is valid or the given value otherwise
template <typename T>
inline T CondData<T>::valueOr(T value) const
{
    if (!validValue_)
        return value;
    
    return value_;
}
#endif // _CONDDATA_H_