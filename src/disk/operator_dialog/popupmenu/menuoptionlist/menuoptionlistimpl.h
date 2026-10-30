#ifndef _MENUOPTIONLISTIMPL_H_
#define _MENUOPTIONLISTIMPL_H_

#include "menuoptionlist.h"

#include <initializer_list>
#include <vector>
#include <string>

template <typename TMenuOption>
class MenuOptionListImpl : public MenuOptionList
{
public:
    MenuOptionListImpl (std::initializer_list<TMenuOption> items)
        :
        items_ {items}
    {}

    const std::vector<TMenuOption>& items () const
    {
        return items_;
    }

    size_t size () const override
    {
        return items_.size ();
    }

    std::vector<std::string> strings () const override
    {
        std::vector<std::string> result;
        result.reserve(items_.size());

        for (const auto& item : items_)
            result.push_back (item.text());

        return result;
    }

    const TMenuOption& operator[] (std::size_t index) const override
    {
        return items_.at (index);
    }

    const TMenuOption& value (std::size_t index) const
    {
        return items_.at (index);
    }

private:
    std::vector<TMenuOption> items_;
};
    
#endif // _MENUOPTIONLISTIMPL_H_ 