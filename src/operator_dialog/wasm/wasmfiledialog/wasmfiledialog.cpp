#include "wasmfiledialog.h"

#include <bind.h>    // link with -lembind
#include <val.h>
#include <utility>

using emscripten::val;
using std::string;
using std::move;
using std::uint64_t;
using std::uintptr_t;

// Makes the onXxx functions callable from JavaScript as
// Module.WASMFileDialog_onXxx(...). As a nested class, BindingsRegistrar may
// use the private member functions; its constructor runs during static
// initialization when bindingsRegistrar_ is created.
WASMFileDialog::BindingsRegistrar::BindingsRegistrar ()
{
    emscripten::function ("WASMFileDialog_onSelected",
                          &WASMFileDialog::onSelected);
    emscripten::function ("WASMFileDialog_onCopied",
                          &WASMFileDialog::onCopied);
    emscripten::function ("WASMFileDialog_onDownloaded",
                          &WASMFileDialog::onDownloaded);
}

const WASMFileDialog::BindingsRegistrar WASMFileDialog::bindingsRegistrar_;


WASMFileDialog::File::File (int id, string name, uint64_t size)
    :
    id_ {id},
    name_ {move (name)},
    size_ {size}
{}

WASMFileDialog::File::~File ()
{
    // Let JavaScript drop its reference to the browser File object.
    jsFileDialog ().call<void> ("releaseFile", id_);
}


WASMFileDialog::~WASMFileDialog ()
{
    // JavaScript holds our address; make sure it is forgotten and that
    // pending operations will not report back.
    jsFileDialog ().call<void> ("forget", jsKey ());
}

void WASMFileDialog::uploadFileDialog (SelectionCallback selectionCallback)
{
    selectionCallback_ = move (selectionCallback);
    jsFileDialog ().call<void> ("upload", jsKey ());
}

void WASMFileDialog::copyToMemfs (File* file, string memfsPath,
                                  CopyCallback callback)
{
    if (file == nullptr)
    {
        if (callback)
            callback (false);
        return;
    }

    copyCallback_ = move (callback);
    jsFileDialog ().call<void> ("copyToMemfs", jsKey (), file->id_, memfsPath);
}

void WASMFileDialog::downloadFileDialog (string fileToBeDownloaded,
                                         DownloadCallback callback)
{
    downloadCallback_ = move (callback);
    jsFileDialog ().call<void> ("download", jsKey (), fileToBeDownloaded);
}

void WASMFileDialog::onSelected (uintptr_t key, int fileId,
                                 string name, double size)
{
    WASMFileDialog* dialog = reinterpret_cast<WASMFileDialog*> (key);

    // Replaces (and releases) the previously selected file.
    if (fileId >= 0)
        dialog->selectedFile_.reset (
            new File (fileId, move (name),
                      static_cast<uint64_t> (size)));
    else
        dialog->selectedFile_.reset ();

    // Take the callback out: the operation is finished, and the callback
    // may destroy this object or start a new upload while running.
    SelectionCallback callback = move (dialog->selectionCallback_);
    dialog->selectionCallback_ = nullptr;

    // Must be the last use of 'dialog'.
    if (callback)
        callback (dialog->selectedFile_.get ());
}

void WASMFileDialog::onCopied (uintptr_t key, bool success)
{
    WASMFileDialog* dialog = reinterpret_cast<WASMFileDialog*> (key);

    CopyCallback callback = move (dialog->copyCallback_);
    dialog->copyCallback_ = nullptr;

    // 'dialog' must not be used anymore: the callback may delete it.
    if (callback)
        callback (success);
}

void WASMFileDialog::onDownloaded (uintptr_t key, int result)
{
    WASMFileDialog* dialog = reinterpret_cast<WASMFileDialog*> (key);

    DownloadCallback callback = move (dialog->downloadCallback_);
    dialog->downloadCallback_ = nullptr;

    // 'dialog' must not be used anymore: the callback may delete it.
    if (callback)
        callback (static_cast<DownloadResult> (result));
}

val WASMFileDialog::jsFileDialog ()
{
    return val::global ("WASMFileDialog");
}

uintptr_t WASMFileDialog::jsKey () const
{
    return reinterpret_cast<uintptr_t> (this);
}
