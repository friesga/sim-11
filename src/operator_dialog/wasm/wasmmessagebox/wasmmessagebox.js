// JavaScript side of WASMMessageBox (wasmmessagebox.h / wasmmessagebox.cpp).
//
// Plain script that defines the global object WASMMessageBox. The C++ code
// calls WASMMessageBox.open() and WASMMessageBox.close() through
// emscripten::val; acknowledgement is reported back by calling the embind
// function WASMMessageBox_onAcknowledged on the Emscripten module.
//
// Loading: include this file before the application opens a messagebox,
// either
//   - with a <script src="wasmmessagebox.js"></script> tag in the HTML page, or
//   - with the emcc option --pre-js wasmmessagebox.js.
// Only when loaded with a <script> tag in a -sMODULARIZE build, call
// WASMMessageBox.init(module) with the module instance once it is created.

globalThis.WASMMessageBox = {
    // Emscripten module; set by init(), otherwise the Module in scope is used.
    module: null,

    // box key (address of the C++ object) -> state of the open messagebox.
    // Insertion order is stacking order: the last entry is on top.
    boxes: new Map(),

    styleInjected: false,

    init(module)
    {
        WASMMessageBox.module = module;
    },

    getModule()
    {
        if (WASMMessageBox.module) return WASMMessageBox.module;
        if (typeof Module !== 'undefined') return Module;
        throw new Error('WASMMessageBox: no Emscripten module; call WASMMessageBox.init(module)');
    },

    injectStyle()
    {
        if (WASMMessageBox.styleInjected) return;
        WASMMessageBox.styleInjected = true;

        const style = document.createElement('style');
        style.textContent = `
      .wasm-message-box-overlay {
        position: fixed;
        inset: 0;
        z-index: 10001;
        display: flex;
        align-items: center;
        justify-content: center;
        background: rgba(0, 0, 0, 0.35);
      }
      .wasm-message-box {
        box-sizing: border-box;
        min-width: 260px;
        max-width: min(480px, calc(100vw - 32px));
        max-height: calc(100vh - 32px);
        overflow-y: auto;
        padding: 20px 24px 16px;
        background: #ffffff;
        color: #1f1f1f;
        border: 1px solid rgba(0, 0, 0, 0.15);
        border-radius: 8px;
        box-shadow: 0 8px 28px rgba(0, 0, 0, 0.25);
        font: 13px/1.4 system-ui, -apple-system, "Segoe UI", sans-serif;
        outline: none;
      }
      .wasm-message-box__text {
        margin: 0 0 16px;
        white-space: pre-wrap;
        overflow-wrap: anywhere;
      }
      .wasm-message-box__buttons {
        display: flex;
        justify-content: flex-end;
      }
      .wasm-message-box__button {
        min-width: 80px;
        padding: 5px 16px;
        font: inherit;
        color: #ffffff;
        background: #2a6fdb;
        border: 1px solid #2a6fdb;
        border-radius: 4px;
        cursor: default;
      }
      .wasm-message-box__button:focus-visible {
        outline: 2px solid #2a6fdb;
        outline-offset: 2px;
      }
      @media (prefers-color-scheme: dark) {
        .wasm-message-box {
          background: #2b2b2b;
          color: #e8e8e8;
          border-color: rgba(255, 255, 255, 0.12);
          box-shadow: 0 8px 28px rgba(0, 0, 0, 0.5);
        }
      }`;
        document.head.appendChild(style);
    },

    // Whether the messagebox identified by boxKey is the topmost one.
    isTopmost(boxKey)
    {
        let last;
        for (const key of WASMMessageBox.boxes.keys()) last = key;
        return last === boxKey;
    },

    // Open a modal messagebox for the C++ object identified by boxKey,
    // showing the string message. An already open messagebox for the same
    // object is closed first.
    open(boxKey, message)
    {
        WASMMessageBox.close(boxKey);
        WASMMessageBox.injectStyle();

        const canvas = WASMMessageBox.getModule().canvas;

        const overlay = document.createElement('div');
        overlay.className = 'wasm-message-box-overlay';

        const element = document.createElement('div');
        element.className = 'wasm-message-box';
        element.setAttribute('role', 'alertdialog');
        element.setAttribute('aria-modal', 'true');
        element.tabIndex = -1;

        const text = document.createElement('p');
        text.className = 'wasm-message-box__text';
        text.textContent = message;     // textContent: no HTML interpretation
        text.id = 'wasm-message-box-text-' + boxKey;
        element.setAttribute('aria-describedby', text.id);

        const buttons = document.createElement('div');
        buttons.className = 'wasm-message-box__buttons';

        const button = document.createElement('button');
        button.className = 'wasm-message-box__button';
        button.type = 'button';
        button.textContent = 'OK';
        button.addEventListener('click', (event) =>
        {
            event.preventDefault();
            event.stopPropagation();
            WASMMessageBox.finish(boxKey);
        });

        buttons.appendChild(button);
        element.appendChild(text);
        element.appendChild(buttons);
        overlay.appendChild(element);

        // Keyboard handling. Registered as capture listener on window so
        // SDL's keyboard handlers do not see keys while the box is open.
        // Only the topmost messagebox handles keys.
        const onKey = (event) =>
        {
            if (!WASMMessageBox.isTopmost(boxKey)) return;
            event.stopImmediatePropagation();
            if (event.type !== 'keydown')
            {
                event.preventDefault();
                return;
            }
            switch (event.key)
            {
                case 'Enter':
                case ' ':
                case 'Escape':
                    WASMMessageBox.finish(boxKey);
                    break;
                case 'Tab':
                    // Keep the focus on the OK button.
                    button.focus({ preventScroll: true });
                    break;
                default:
                    return; // let the browser handle other keys (no preventDefault)
            }
            event.preventDefault();
        };

        // Pointer down outside the button must not move the focus away
        // (the box is modal; clicking the backdrop does not dismiss it).
        const onPointerDown = (event) =>
        {
            if (event.target !== button) event.preventDefault();
        };

        const onContextMenu = (event) => event.preventDefault();

        window.addEventListener('keydown', onKey, true);
        window.addEventListener('keyup', onKey, true);
        window.addEventListener('keypress', onKey, true);
        overlay.addEventListener('pointerdown', onPointerDown);
        overlay.addEventListener('contextmenu', onContextMenu);

        document.body.appendChild(overlay);
        button.focus({ preventScroll: true });

        WASMMessageBox.boxes.set(boxKey, {
            overlay,
            cleanup()
            {
                window.removeEventListener('keydown', onKey, true);
                window.removeEventListener('keyup', onKey, true);
                window.removeEventListener('keypress', onKey, true);
                overlay.remove();

                // Return the focus to the box below, or else to the canvas.
                let below;
                for (const state of WASMMessageBox.boxes.values()) below = state;
                if (below)
                    below.overlay.querySelector('button').focus({ preventScroll: true });
                else if (canvas)
                    canvas.focus({ preventScroll: true });
            },
        });
    },

    // Remove the messagebox without notifying C++. Returns whether one was open.
    close(boxKey)
    {
        const state = WASMMessageBox.boxes.get(boxKey);
        if (!state) return false;
        WASMMessageBox.boxes.delete(boxKey);
        state.cleanup();
        return true;
    },

    // Remove the messagebox and report the acknowledgement to C++.
    finish(boxKey)
    {
        // Clean up first: the C++ callback may reopen or destroy the box.
        if (WASMMessageBox.close(boxKey))
            WASMMessageBox.getModule().WASMMessageBox_onAcknowledged(boxKey);
    },
};
