// JavaScript side of WASMFileDialog (wasmfiledialog.h / wasmfiledialog.cpp).
//
// Plain script that defines the global object WASMFileDialog. The C++ code
// calls its functions through emscripten::val; results are reported back by
// calling the embind functions WASMFileDialog_onSelected,
// WASMFileDialog_onCopied and WASMFileDialog_onDownloaded on the Emscripten
// module.
//
// Requires the file system to be exported: -sEXPORTED_RUNTIME_METHODS=FS
//
// Loading: include this file before the application opens a file dialog,
// either
//   - with a <script src="wasmfiledialog.js"></script> tag in the HTML page, or
//   - with the emcc option --pre-js wasmfiledialog.js.
// Only when loaded with a <script> tag in a -sMODULARIZE build, call
// WASMFileDialog.init(module) with the module instance once it is created.

globalThis.WASMFileDialog = {
    // Emscripten module; set by init(), otherwise the Module in scope is used.
    module: null,

    // dialog key (address of the C++ object) ->
    //   { upload, copy, download }: token of the pending operation per kind
    dialogs: new Map(),

    // file id -> browser File object selected in an upload dialog
    files: new Map(),
    nextFileId: 1,

    // Values of WASMFileDialog::DownloadResult.
    DownloadResult: { Completed: 0, Started: 1, Cancelled: 2, Failed: 3 },

    init(module)
    {
        WASMFileDialog.module = module;
    },

    getModule()
    {
        if (WASMFileDialog.module) return WASMFileDialog.module;
        if (typeof Module !== 'undefined') return Module;
        throw new Error('WASMFileDialog: no Emscripten module; call WASMFileDialog.init(module)');
    },

    getFS()
    {
        const fs = WASMFileDialog.getModule().FS;
        if (!fs) throw new Error('WASMFileDialog: FS not available; link with -sEXPORTED_RUNTIME_METHODS=FS');
        return fs;
    },

    // ------------------------------------------------ operation administration

    // Start an operation of the given kind for the dialog identified by key.
    // A pending operation of the same kind is discarded. Returns the token
    // of the new operation; token.cleanup (optional) is called when the
    // operation ends in any way.
    begin(key, kind)
    {
        let state = WASMFileDialog.dialogs.get(key);
        if (!state)
        {
            state = {};
            WASMFileDialog.dialogs.set(key, state);
        }
        const previous = state[kind];
        if (previous && previous.cleanup) previous.cleanup();

        const token = { cleanup: null };
        state[kind] = token;
        return token;
    },

    // End the operation identified by token and call report() to notify C++,
    // unless the operation has been discarded in the meantime.
    finish(key, kind, token, report)
    {
        const state = WASMFileDialog.dialogs.get(key);
        if (!state || state[kind] !== token) return;
        delete state[kind];
        if (token.cleanup) token.cleanup();
        report();
    },

    // Discard all pending operations of a dialog (C++ object destroyed).
    forget(key)
    {
        const state = WASMFileDialog.dialogs.get(key);
        if (!state) return;
        WASMFileDialog.dialogs.delete(key);
        for (const token of Object.values(state))
            if (token && token.cleanup) token.cleanup();
    },

    // Drop the reference to a browser File (C++ File object destroyed).
    releaseFile(fileId)
    {
        WASMFileDialog.files.delete(fileId);
    },

    // ------------------------------------------------------------------ upload

    // Open the browser's file selection dialog.
    upload(key)
    {
        const token = WASMFileDialog.begin(key, 'upload');

        const input = document.createElement('input');
        input.type = 'file';
        input.style.display = 'none';
        token.cleanup = () => input.remove();

        const report = (file) => WASMFileDialog.finish(key, 'upload', token, () =>
        {
            const module = WASMFileDialog.getModule();
            if (file)
            {
                const fileId = WASMFileDialog.nextFileId++;
                WASMFileDialog.files.set(fileId, file);
                module.WASMFileDialog_onSelected(key, fileId, file.name, file.size);
            }
            else
            {
                module.WASMFileDialog_onSelected(key, -1, '', 0);
            }
        });

        input.addEventListener('change', () =>
            report(input.files && input.files.length > 0 ? input.files[0] : null));
        // Fired when the user closes the dialog without selecting a file.
        input.addEventListener('cancel', () => report(null));

        document.body.appendChild(input);
        input.click();
    },

    // ------------------------------------------------------------ copy to MEMFS

    // Copy the contents of a selected file to path in MEMFS.
    copyToMemfs(key, fileId, path)
    {
        const token = WASMFileDialog.begin(key, 'copy');
        const file = WASMFileDialog.files.get(fileId);

        const work = file
            ? file.arrayBuffer()
            : Promise.reject(new Error('unknown file id ' + fileId));

        work.then((buffer) =>
        {
            const FS = WASMFileDialog.getFS();
            const dir = path.substring(0, path.lastIndexOf('/'));
            if (dir) FS.mkdirTree(dir);
            FS.writeFile(path, new Uint8Array(buffer));
            return true;
        })
        .catch((error) =>
        {
            console.error('WASMFileDialog: copy to ' + path + ' failed:', error);
            return false;
        })
        .then((success) => WASMFileDialog.finish(key, 'copy', token, () =>
            WASMFileDialog.getModule().WASMFileDialog_onCopied(key, success)));
    },

    // ---------------------------------------------------------------- download

    // Save the file at path in MEMFS on the local PC.
    download(key, path)
    {
        const token = WASMFileDialog.begin(key, 'download');
        const Result = WASMFileDialog.DownloadResult;
        const name = path.substring(path.lastIndexOf('/') + 1) || 'download';

        let data = null;
        try
        {
            data = WASMFileDialog.getFS().readFile(path);   // Uint8Array copy
        }
        catch (error)
        {
            console.error('WASMFileDialog: cannot read ' + path + ' from MEMFS:', error);
        }

        // Hand the file to the browser's download manager. The browser does
        // not report whether or when the download completes.
        const downloadViaLink = () =>
        {
            const blob = new Blob([data], { type: 'application/octet-stream' });
            const url = URL.createObjectURL(blob);
            const link = document.createElement('a');
            link.href = url;
            link.download = name;
            link.style.display = 'none';
            document.body.appendChild(link);
            link.click();
            link.remove();
            setTimeout(() => URL.revokeObjectURL(url), 60000);
            return Result.Started;
        };

        let work;
        if (data === null)
        {
            work = Promise.resolve(Result.Failed);
        }
        else if (typeof window.showSaveFilePicker === 'function')
        {
            // File System Access API (Chromium browsers): real save dialog
            // with reliable completion and cancellation.
            work = window.showSaveFilePicker({ suggestedName: name }).then(
                async (handle) =>
                {
                    const writable = await handle.createWritable();
                    await writable.write(data);
                    await writable.close();
                    return Result.Completed;
                },
                (error) =>
                {
                    if (error.name === 'AbortError') return Result.Cancelled;
                    // No user activation, or not allowed in this (i)frame.
                    if (error.name === 'SecurityError' || error.name === 'NotAllowedError')
                        return downloadViaLink();
                    throw error;
                });
        }
        else
        {
            work = Promise.resolve().then(downloadViaLink);
        }

        work.catch((error) =>
        {
            console.error('WASMFileDialog: download of ' + path + ' failed:', error);
            return Result.Failed;
        })
        .then((result) => WASMFileDialog.finish(key, 'download', token, () =>
            WASMFileDialog.getModule().WASMFileDialog_onDownloaded(key, result)));
    },
};
