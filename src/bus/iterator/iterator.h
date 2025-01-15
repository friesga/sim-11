#ifndef _ITERATOR_H_
#define _ITERATOR_H_

#include <iterator>

class BusDevice;

// A Bus contains a number of Busdevice's. This class implements an iterator
// over BusDevice pointers so it can be used to iterate over a Bus'
// BusDevices.
class Iterator
{
public:
	using iterator_category = std::forward_iterator_tag;
	using difference_type   = std::ptrdiff_t;
	using value_type        = BusDevice*;
	using pointer           = BusDevice**;  // or also value_type*
	using reference         = const value_type&;

	// In the default constructor initialize the iterator with a default
	// null pointer.
	Iterator ();
	Iterator (pointer it);

	// Dereference operator
	reference operator* ();
	pointer operator-> ();

	// Prefix increment
	Iterator& operator++ ();

	// Postfix increment
	Iterator operator++ (int);

	friend bool operator== (const Iterator& a, const Iterator& b);
	friend bool operator!= (const Iterator& a, const Iterator& b);

private:
	pointer ptr_;
};



#endif // _ITERATOR_H_