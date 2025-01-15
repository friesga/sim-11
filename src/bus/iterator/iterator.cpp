#include "iterator.h"

Iterator::Iterator ()
    :
    ptr_ {nullptr}
{}

Iterator::Iterator (pointer it)
    :
    ptr_ (it)
{}

Iterator::reference Iterator::operator* ()
{
    return *ptr_;
}

Iterator::pointer Iterator::operator-> ()
{
    return &(*ptr_);
}

// Prefix increment
Iterator& Iterator::operator++ ()
{
    ++ptr_; return *this;
}

// Postfix increment
Iterator Iterator::operator++ (int)
{
    Iterator tmp = *this; ++(*this); return tmp;
}

bool operator== (const Iterator& a, const Iterator& b)
{
    return a.ptr_ == b.ptr_;
}

bool operator!= (const Iterator& a, const Iterator& b)
{
    return a.ptr_ != b.ptr_;
}
