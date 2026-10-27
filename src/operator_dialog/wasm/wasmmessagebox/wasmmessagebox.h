#ifndef _WASMMESSAGEBOX_H_
#define _WASMMESSAGEBOX_H_

#include <cstdint>
#include <functional>
#include <string>

namespace emscripten { class val; }

// Modal messagebox rendered as a DOM element on top of the canvas. Runs
// entirely on the browser main thread; the callback is invoked from a
// browser event handler when the user acknowledges the message.
//
// The callback may safely destroy the WASMMessageBox object or open the
// popup again.
//
// Requires wasmmessagebox.js to be loaded (see that file).
class WASMMessageBox
{
public:
    using MessageCallback = std::function<void ()>;

    explicit WASMMessageBox (MessageCallback messageCallback);
    ~WASMMessageBox ();

    // The address of this object is passed to JavaScript, so the
    // object must not be copied or moved.
    WASMMessageBox (const WASMMessageBox&) = delete;
    WASMMessageBox& operator= (const WASMMessageBox&) = delete;
    WASMMessageBox (WASMMessageBox&&) = delete;
    WASMMessageBox& operator= (WASMMessageBox&&) = delete;

    // Prints the given string in the messagebox and calls messageCallback
    // when acknowledged by the user. An already open messagebox of this
    // object is closed first without invoking the callback.
    void open (std::string message);

private:
    // Called from JavaScript (wasmmessagebox.js) as
    // Module.WASMMessageBox_onAcknowledged(key) when the user acknowledges
    // the messagebox; key is the value returned by jsKey().
    static void onAcknowledged (std::uintptr_t key);

    // The global JavaScript object defined in wasmmessagebox.js.
    static emscripten::val jsMessageBox ();

    // Key under which JavaScript administers the messagebox of this object.
    std::uintptr_t jsKey () const;

    // Invokes the callback.
    void acknowledged ();

    // Registers onAcknowledged with embind during static initialization.
    struct BindingsRegistrar
    {
        BindingsRegistrar ();
    };

    static const BindingsRegistrar bindingsRegistrar_;

    MessageCallback messageCallback_;
};

#endif
