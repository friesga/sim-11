// JavaScript side of WASMPopUpMenu (wasmpopupmenu.h / wasmpopupmenu.cpp).
//
// Plain script that defines the global object WASMPopupMenu. The C++ code
// calls WASMPopupMenu.open() and WASMPopupMenu.close() through
// emscripten::val; the result is reported back by calling the embind
// function WASMPopUpMenu_onResult on the Emscripten module.
//
// Loading: include this file before the application opens a popup, either
//   - with a <script src="wasmpopupmenu.js"></script> tag in the HTML page, or
//   - with the emcc option --pre-js wasmpopupmenu.js.
// Only when loaded with a <script> tag in a -sMODULARIZE build, call
// WASMPopupMenu.init(module) with the module instance once it is created.

globalThis.WASMPopupMenu = {
    // Emscripten module; set by init(), otherwise the Module in scope is used.
    module: null,

    // menu key (address of the C++ object) -> state of the open popup
    menus: new Map(),

    styleInjected: false,

    init(module)
    {
        WASMPopupMenu.module = module;
    },

    getModule()
    {
        if (WASMPopupMenu.module) return WASMPopupMenu.module;
        if (typeof Module !== 'undefined') return Module;
        throw new Error('WASMPopupMenu: no Emscripten module; call WASMPopupMenu.init(module)');
    },

    injectStyle()
    {
        if (WASMPopupMenu.styleInjected) return;
        WASMPopupMenu.styleInjected = true;

        const style = document.createElement('style');
        style.textContent = `
      .wasm-popup-menu {
        position: fixed;
        z-index: 10000;
        box-sizing: border-box;
        min-width: 160px;
        max-height: calc(100vh - 16px);
        overflow-y: auto;
        margin: 0;
        padding: 4px 0;
        list-style: none;
        background: #ffffff;
        color: #1f1f1f;
        border: 1px solid rgba(0, 0, 0, 0.15);
        border-radius: 6px;
        box-shadow: 0 6px 20px rgba(0, 0, 0, 0.2);
        font: 13px/1.4 system-ui, -apple-system, "Segoe UI", sans-serif;
        outline: none;
        user-select: none;
        -webkit-user-select: none;
      }
      .wasm-popup-menu__item {
        padding: 4px 16px;
        white-space: nowrap;
        cursor: default;
      }
      .wasm-popup-menu__item.is-active {
        background: #2a6fdb;
        color: #ffffff;
      }
      @media (prefers-color-scheme: dark) {
        .wasm-popup-menu {
          background: #2b2b2b;
          color: #e8e8e8;
          border-color: rgba(255, 255, 255, 0.12);
          box-shadow: 0 6px 20px rgba(0, 0, 0, 0.5);
        }
      }`;
        document.head.appendChild(style);
    },

    // Open a popup for the C++ object identified by menuKey with its top left
    // corner at (x, y), relative to the canvas. labels: array of strings.
    // An already open popup for the same object is closed first.
    open(menuKey, x, y, labels)
    {
        WASMPopupMenu.close(menuKey);
        WASMPopupMenu.injectStyle();

        const canvas = WASMPopupMenu.getModule().canvas;
        const origin = canvas ? canvas.getBoundingClientRect() : { left: 0, top: 0 };

        const element = document.createElement('ul');
        element.className = 'wasm-popup-menu';
        element.setAttribute('role', 'menu');
        element.tabIndex = -1;

        const items = labels.map((label, index) =>
        {
            const item = document.createElement('li');
            item.className = 'wasm-popup-menu__item';
            item.setAttribute('role', 'menuitem');
            item.textContent = label;
            item.addEventListener('mousemove', () => setActive(index));
            item.addEventListener('click', (event) =>
            {
                event.preventDefault();
                event.stopPropagation();
                WASMPopupMenu.finish(menuKey, index);
            });
            element.appendChild(item);
            return item;
        });

        let active = -1;
        const setActive = (index) =>
        {
            if (index === active) return;
            if (active >= 0) items[active].classList.remove('is-active');
            active = index;
            if (active >= 0)
            {
                items[active].classList.add('is-active');
                items[active].scrollIntoView({ block: 'nearest' });
            }
        };

        // Keyboard handling. Registered as capture listener on window so
        // SDL's keyboard handlers do not see keys while the menu is open.
        const onKey = (event) =>
        {
            event.stopImmediatePropagation();
            if (event.type !== 'keydown')
            {
                event.preventDefault();
                return;
            }
            const count = items.length;
            switch (event.key)
            {
                case 'ArrowDown':
                    if (count) setActive((active + 1) % count);
                    break;
                case 'ArrowUp':
                    if (count) setActive(active <= 0 ? count - 1 : active - 1);
                    break;
                case 'Home':
                    if (count) setActive(0);
                    break;
                case 'End':
                    if (count) setActive(count - 1);
                    break;
                case 'Enter':
                case ' ':
                    if (active >= 0) WASMPopupMenu.finish(menuKey, active);
                    break;
                case 'Escape':
                case 'Tab':
                    WASMPopupMenu.finish(menuKey, -1);
                    break;
                default:
                    return; // let the browser handle other keys (no preventDefault)
            }
            event.preventDefault();
        };

        // Pointer down anywhere outside the menu dismisses it.
        const onPointerDown = (event) =>
        {
            if (!element.contains(event.target)) WASMPopupMenu.finish(menuKey, -1);
        };

        const onContextMenu = (event) =>
        {
            if (element.contains(event.target)) event.preventDefault();
        };

        const onDismiss = () => WASMPopupMenu.finish(menuKey, -1);

        window.addEventListener('keydown', onKey, true);
        window.addEventListener('keyup', onKey, true);
        window.addEventListener('keypress', onKey, true);
        document.addEventListener('pointerdown', onPointerDown, true);
        document.addEventListener('contextmenu', onContextMenu, true);
        window.addEventListener('blur', onDismiss);
        window.addEventListener('resize', onDismiss);

        // Place the menu, then keep it inside the viewport.
        element.style.left = '0px';
        element.style.top = '0px';
        element.style.visibility = 'hidden';
        document.body.appendChild(element);

        const margin = 8;
        const width = element.offsetWidth;
        const height = element.offsetHeight;
        let left = origin.left + x;
        let top = origin.top + y;
        if (left + width > window.innerWidth - margin)
            left = Math.max(margin, window.innerWidth - margin - width);
        if (top + height > window.innerHeight - margin)
            top = Math.max(margin, window.innerHeight - margin - height);

        element.style.left = left + 'px';
        element.style.top = top + 'px';
        element.style.visibility = '';
        element.focus({ preventScroll: true });

        WASMPopupMenu.menus.set(menuKey, {
            element,
            cleanup()
            {
                window.removeEventListener('keydown', onKey, true);
                window.removeEventListener('keyup', onKey, true);
                window.removeEventListener('keypress', onKey, true);
                document.removeEventListener('pointerdown', onPointerDown, true);
                document.removeEventListener('contextmenu', onContextMenu, true);
                window.removeEventListener('blur', onDismiss);
                window.removeEventListener('resize', onDismiss);
                element.remove();
                if (canvas) canvas.focus({ preventScroll: true });
            },
        });
    },

    // Remove the popup without notifying C++. Returns whether one was open.
    close(menuKey)
    {
        const state = WASMPopupMenu.menus.get(menuKey);
        if (!state) return false;
        WASMPopupMenu.menus.delete(menuKey);
        state.cleanup();
        return true;
    },

    // Remove the popup and report the result to C++.
    finish(menuKey, index)
    {
        // Clean up first: the C++ callback may reopen or destroy the menu.
        if (WASMPopupMenu.close(menuKey))
            WASMPopupMenu.getModule().WASMPopUpMenu_onResult(menuKey, index);
    },
};
