#ifndef _MENUOPTIONIMPL_H_
#define _MENUOPTIONIMPL_H_

#include "menuoption.h"

#include <string>

template<typename Enum>
class MenuOptionImpl : public MenuOption
{
public:
    MenuOptionImpl (Enum value, std::string text)
        :
        value_(value),
        text_(std::move(text))
    {}

    const std::string& text() const override
    {
        return text_;
    }

    int value () const override
    {
        return static_cast<int> (value_);
    }

private:
    Enum value_;
    std::string text_;
};

#endif // _MENUOPTIONIMPL_H_