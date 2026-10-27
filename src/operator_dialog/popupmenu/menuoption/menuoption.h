#ifndef _MENUOPTION_H_
#define _MENUOPTION_H_

#include <string>

// A MenuOption defines a single option in a popup menu. A MenuOption
// consists of the text that is displayed in the menu and the value that
// is returned when the option is selected. The value is the integer
// representation of an enum class that defines the possible values for
// the menu.
//
class MenuOption
{
public:
    virtual ~MenuOption () = default;
    virtual const std::string& text () const = 0;
    virtual int value () const = 0;
};

#endif // _MENUOPTION_H_