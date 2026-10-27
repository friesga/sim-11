#include "wasmmessagebox.h"

#include <bind.h>    // link with -lembind
#include <val.h>

#include <utility>

using emscripten::val;
using std::move;
using std::string;
using std::uintptr_t;

// Makes onAcknowledged callable from JavaScript as
// Module.WASMMessageBox_onAcknowledged(key). As a nested class,
// BindingsRegistrar may use the private member function; its constructor
// runs during static initialization when bindingsRegistrar_ is created.
WASMMessageBox::BindingsRegistrar::BindingsRegistrar ()
{
    emscripten::function ("WASMMessageBox_onAcknowledged",
                          &WASMMessageBox::onAcknowledged);
}

const WASMMessageBox::BindingsRegistrar WASMMessageBox::bindingsRegistrar_;

WASMMessageBox::WASMMessageBox (MessageCallback messageCallback)
    :
    messageCallback_ {move (messageCallback)}
{}

WASMMessageBox::~WASMMessageBox ()
{
    // JavaScript holds our address; make sure it is forgotten.
    jsMessageBox ().call<void> ("close", jsKey ());
}

void WASMMessageBox::open (string message)
{
    jsMessageBox ().call<void> ("open", jsKey (), message);
}

void WASMMessageBox::onAcknowledged (uintptr_t key)
{
    // Must be the last use of the object: the callback may delete it.
    reinterpret_cast<WASMMessageBox*> (key)->acknowledged ();
}

val WASMMessageBox::jsMessageBox ()
{
    return val::global ("WASMMessageBox");
}

uintptr_t WASMMessageBox::jsKey () const
{
    return reinterpret_cast<uintptr_t> (this);
}

void WASMMessageBox::acknowledged ()
{
    // Copy the callback: it may destroy this object while running.
    MessageCallback callback = messageCallback_;
    if (callback)
        callback ();
}
