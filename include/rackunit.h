#ifndef _RACK_UNIT_H_
#define _RACK_UNIT_H_

// Definition of a Rack Unit to avoid confusion with other height definitions.
class RackUnit
{
public:
    operator int () const { return static_cast<int> (value_); }

private:
    // The constructor is defined private so it can't be called from outside
    // the class. The literal operator _ru is declared as friend and therefore
    // is able to access the constructor.
    explicit RackUnit (size_t value) : value_(value) {}

    friend RackUnit operator"" _ru (size_t value);

    size_t value_;
};

inline RackUnit operator"" _ru (size_t value)
{
    return RackUnit (value);
}


#endif // _RACK_UNIT_H_