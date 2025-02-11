#ifndef _CONDDATA_H_
#define _CONDDATA_H_

#include "statuscodes.h"

#include <string>

// The CondData class contains a pair of a templated value and a StatusCode,
// to be used e.g. as return value from functions.
//
template <typename T>
class CondData
{
    T value_;
    StatusCode statusCode_;

public:
    // Constructors:
    // - The default constructor constructs an object without valid data
    //   (a default value and a status code indicating an error),
    // - The constructor with just a value argument constructs an object with
    //   valid data and status code Success,
    // - The constructor with both a value and a status code argument creates
    //   an object with data is valid or invalid depending on the given status
    //   code,
    // - The constructor with just a status code creates an object with the
    //   given status code and a default value.
    //
    CondData ();
    CondData (T value);
    CondData (T value, StatusCode statusCode);
    CondData (StatusCode statusCode);

    // Copy constructor
    template <typename V>
        CondData (CondData<V> &other);

    // Assignment operators
    CondData<T>& operator=  (T const &value);
    CondData<T>& operator+= (T const &value);
    CondData<T>& operator-= (T const &value);

    // Increment/decrement operators
    CondData<T>& operator++ ();
    CondData<T>& operator-- ();
    CondData<T> operator++ (int);
    CondData<T> operator-- (int);

    // Conversion operator
    operator T() const;

    // Accessors
    bool hasValue () const;
    T value () const;
    T valueOr (T value) const;
    StatusCode statusCode ();
};

// The default constructor constructs an object without valid data.
template <typename T>
CondData<T>::CondData ()
    :
    value_ {},
    statusCode_ {StatusCode::ArgumentError}
{}

// Construct an object with valid data.
template <typename T>
CondData<T>::CondData (T value)
    :
    value_ {value},
    statusCode_ {StatusCode::Success}
{}

// Construct an object with the given value and status code
template <typename T>
CondData<T>::CondData (T value, StatusCode statusCode)
    :
    value_ {value},
    statusCode_ {statusCode}
{}

// Construct an object with the given status code and a default value
template <typename T>
CondData<T>::CondData (StatusCode statusCode)
    :
    value_ {},
    statusCode_ {statusCode}
{}

// Construct a CondData object from a CondData object with possibly another
// native type.
// The initialization of the data members is performed in the body of the
// constructor as initialization via the initialization list with MSVC generates
// compiler error C2397.
template <typename T>
template <typename V>
CondData<T>::CondData (CondData<V> &other)
{
    value_ = static_cast<T> (other.valueOr (0));
    statusCode_ =  other.statusCode ();
}

// Copy assign a native value
template <typename T>
CondData<T>& CondData<T>::operator= (T const &value)
{
    value_ = value;
    statusCode_ = StatusCode::Success;
    return *this;
}

// Add native type to this object
template <typename T>
CondData<T>& CondData<T>::operator+= (T const &value)
{
    if (statusCode_ == StatusCode::Success)
        value_ += value;
    else
        throw (std::string("Addition on invalid CondData object"));
    return *this;
}

// Subract native type from this object
template <typename T>
CondData<T>& CondData<T>::operator-= (T const &value)
{
    if (statusCode_ == StatusCode::Success)
        value_ -= value;
    else
        throw (std::string("Subtraction on invalid CondData object"));
    return *this;
}

// Pre-increment operator
template <typename T>
CondData<T>& CondData<T>::operator++ ()
{
    if (statusCode_ == StatusCode::Success)
        ++value_;
    else
        throw (std::string ("Increment on invalid CondData object"));
    return *this;
}

// Pre-decrement operator
template <typename T>
CondData<T>& CondData<T>::operator-- ()
{
    if (statusCode_ == StatusCode::Success)
        --value_;
    else
        throw (std::string ("Decrement on invalid CondData object"));
    return *this;
}

// Post-increment operator. The int parameter is a dummy parameter used to
// differentiate between prefix and postfix versions of the operators.
// Increment the object and return the original object.
template <typename T>
CondData<T> CondData<T>::operator++ (int)
{
    if (statusCode_ == StatusCode::Success)
    {
        CondData<T> copy {*this};
        ++(*this);
        return copy;
    }
    else
        throw (std::string ("Increment on invalid CondData object"));
}

// Post-decrement oerator
template <typename T>
CondData<T> CondData<T>::operator-- (int)
{
    if (statusCode_== StatusCode::Success)
    {
        CondData<T> tmp = *this;
        --value_;
        return tmp;
    }
    else
        throw (std::string ("Increment on invalid CondData object"));
}

// Try to convert the object to the native type, throwing an exception
// if it doesn't contain a valid value.
template <typename T>
CondData<T>::operator T() const
{
    if (statusCode_ != StatusCode::Success)
        throw (std::string("Conversion on invalid CondData object"));

    return value_;
}

// Return the valid value status to the caller
template <typename T>
bool CondData<T>::hasValue() const
{
    return statusCode_ == StatusCode::Success;
}

// Return the value to the caller if it is valid
template <typename T>
T CondData<T>::value() const
{
    if (statusCode_ != StatusCode::Success)
        throw (std::string("CondData object has no valid value"));
    return value_;    
}

// Return the value to the caller if it is valid or the given value otherwise
template <typename T>
T CondData<T>::valueOr(T value) const
{
    if (statusCode_ != StatusCode::Success)
        return value;
    
    return value_;
}

// Return the status code for this object
template <typename T>
StatusCode CondData<T>::statusCode ()
{
    return statusCode_;
}

#endif // _CONDDATA_H_