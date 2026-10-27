#ifndef WASMFILEDIALOG_H
#define WASMFILEDIALOG_H

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace emscripten { class val; }

// File dialogs for uploading files from the local PC to MEMFS and for
// downloading files from MEMFS to the local PC. Runs entirely on the browser
// main thread; the callbacks are invoked from browser event handlers or
// promise continuations, never from within the call that started the
// operation (except copyToMemfs() with a nullptr file).
//
// Browsers only open file dialogs in response to a user action, so call
// uploadFileDialog() and downloadFileDialog() while handling a mouse click
// or key press.
//
// The callbacks may safely destroy the WASMFileDialog object or start a new
// operation. Only one operation of each kind (upload, copy, download) can be
// pending per object: starting a new one discards the pending one without
// invoking its callback. Destroying the object discards all pending
// operations.
//
// A file upload has to occur in the folowing sequence:
// - The application calls uploadFileDialog with a selection callback. The
//   browser opens the file dialog and the user selects a file.
// - On selection of a file the selection callback is called with a pointer
//   to a File object.
// - The selection callback calls copyToMemfs with a copy callback. When the
//   copy is completed the copy callback is called and the upload is completed.
//
// A download operation has two steps:
// - The application calls downloadFileDialog with the file to download and
//   a download callback. The file is then downloaded and the user can select
//   the directory to download to or the file is downloaded to a Downloads
//   directory. After the download the download callback is called.
// 
// Requires wasmfiledialog.js to be loaded (see that file) and the Emscripten
// file system to be exported: -sEXPORTED_RUNTIME_METHODS=FS
//
class WASMFileDialog
{
public:
    // A file selected by the user in the upload dialog. Refers to the
    // browser's File object; its contents are read by copyToMemfs().
    class File
    {
    public:
        ~File ();

        File (const File&) = delete;
        File& operator= (const File&) = delete;

        // Name of the file on the local PC, without directory.
        const std::string& name () const { return name_; }

        // Size of the file in bytes.
        std::uint64_t size () const { return size_; }

    private:
        friend class WASMFileDialog;

        File (int id, std::string name, std::uint64_t size);

        int id_;            // key of the browser File object in JavaScript
        std::string name_;
        std::uint64_t size_;
    };

    enum class DownloadResult
    {
        Completed = 0,  // The file has been written to the local PC.
        Started = 1,    // The file has been handed to the browser's download
                        // manager; completion is not reported by the browser.
                        // Used when the browser has no save file dialog
                        // (e.g. Firefox, Safari).
        Cancelled = 2,  // The user cancelled the save file dialog.
        Failed = 3      // The file does not exist in MEMFS or could not be
                        // written; details are logged on the console.
    };

    // file is nullptr when the user cancelled the dialog.
    using SelectionCallback = std::function<void (File* file)>;
    using CopyCallback = std::function<void (bool)>;
    using DownloadCallback = std::function<void (DownloadResult)>;

    WASMFileDialog () = default;
    ~WASMFileDialog ();

    // The address of this object is passed to JavaScript, so the
    // object must not be copied or moved.
    WASMFileDialog (const WASMFileDialog&) = delete;
    WASMFileDialog& operator= (const WASMFileDialog&) = delete;
    WASMFileDialog (WASMFileDialog&&) = delete;
    WASMFileDialog& operator= (WASMFileDialog&&) = delete;

    // Opens the upload file dialog and calls the selection callback when
    // the user has selected a file or cancelled the dialog. The selection
    // callback can then call copyToMemfs to copy the file to MEMFS.
    // The File is owned by this object and stays valid until the selection
    // callback is invoked again or this object is destroyed.
    void uploadFileDialog (SelectionCallback selectionCallback);

    // Copies the given file to a file with the given path in MEMFS and calls
    // the callback when the copy is completed (true) or has failed (false).
    // Missing directories in memfsPath are created; an existing file is
    // overwritten.
    void copyToMemfs (File* file, std::string memfsPath, CopyCallback callback);

    // Full download operation: MEMFS -> browser File -> local PC.
    // Downloads the file with the given path in MEMFS and calls the
    // callback when the writing is completed. The name of the file in MEMFS
    // is proposed as name on the local PC.
    void downloadFileDialog (std::string fileToBeDownloaded,
        DownloadCallback callback);

private:
    // Called from JavaScript (wasmfiledialog.js) as
    // Module.WASMFileDialog_on...(key, ...) when an operation has finished;
    // key is the value returned by jsKey().
    // fileId < 0: the upload dialog was cancelled.
    static void onSelected (std::uintptr_t key, int fileId,
        std::string name, double size);
    static void onCopied (std::uintptr_t key, bool success);
    static void onDownloaded (std::uintptr_t key, int result);

    // The global JavaScript object defined in wasmfiledialog.js.
    static emscripten::val jsFileDialog ();

    // Key under which JavaScript administers the operations of this object.
    std::uintptr_t jsKey () const;

    // Registers the onXxx functions with embind during static initialization.
    struct BindingsRegistrar
    {
        BindingsRegistrar ();
    };

    static const BindingsRegistrar bindingsRegistrar_;

    std::unique_ptr<File> selectedFile_;
    SelectionCallback selectionCallback_;
    CopyCallback copyCallback_;
    DownloadCallback downloadCallback_;
};

#endif
