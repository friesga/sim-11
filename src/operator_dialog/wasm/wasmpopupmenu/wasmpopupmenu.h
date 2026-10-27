#ifndef _WASMPOPMENU_H_
#define _WASMPOPMENU_H_

#include "operator_dialog/popupmenu/menuoptionlist/menuoptionlist.h"

#include <cstdint>
#include <functional>

namespace emscripten { class val; }

// Popup menu rendered as a DOM element on top of the canvas. Runs
// entirely on the browser main thread; the callback is invoked from
// a browser event handler when the user selects an option (with a
// pointer to that option) or dismisses the menu (with nullptr).
//
// The callback may safely destroy the WASMPopupMenu object or open the
// popup again. The option pointer points into the MenuOptionList, so it
// stays valid as long as that list exists.
//
// Requires wasmpopupmenu.js to be loaded (see that file).
class WASMPopupMenu
{
public:
    using PopupCallback = std::function<void (const MenuOption*)>;

    // The menu keeps a reference to 'options'; the list must outlive
    // the WASMPopupMenu object and must not change while the popup is open.
    WASMPopupMenu (const MenuOptionList& options, PopupCallback callback);
    ~WASMPopupMenu ();

    // The address of this object is passed to JavaScript, so the
    // object must not be copied or moved.
    WASMPopupMenu (const WASMPopupMenu&) = delete;
    WASMPopupMenu& operator= (const WASMPopupMenu&) = delete;
    WASMPopupMenu (WASMPopupMenu&&) = delete;
    WASMPopupMenu& operator= (WASMPopupMenu&&) = delete;

    // Open the popup with its top left corner at (x, y), in CSS pixels
    // relative to the top left corner of the canvas (i.e. SDL window
    // coordinates). An already open popup is closed first without
    // invoking the callback.
    void open (int x, int y);

    // Invokes the callback with the given result.
    void selected (const MenuOption* option);

private:
    // Called from JavaScript (wasmpopupmenu.js) as
    // Module.WASMPopUpMenu_onResult(key, index) when the popup is closed
    // by the user; key is the value returned by jsKey().
    // index >= 0: selected option, index < 0: dismissed.
    static void onResult (std::uintptr_t key, int index);

    // The global JavaScript object defined in wasmpopupmenu.js.
    static emscripten::val jsPopupMenu ();

    // Key under which JavaScript administers the popup of this object.
    std::uintptr_t jsKey () const;

    // Registers onResult with embind during static initialization.
    struct BindingsRegistrar
    {
        BindingsRegistrar ();
    };

    static const BindingsRegistrar bindingsRegistrar_;

    const MenuOptionList& options_;
    PopupCallback callback_;
};

#endif // _WASMPOPMENU_H_
