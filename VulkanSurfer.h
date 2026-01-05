/// Copyright(C) by Matěj Eliáš - matejelias.cz
/// This code is licensed under the MIT license(MIT) (http://opensource.org/licenses/MIT)

#ifndef VULKANSURFER_H
#define VULKANSURFER_H

#include <cstdint>
#include <functional>
#include <string>
#include <stdexcept>

// Auto-detect platform if not already defined
#if !defined(SURFER_PLATFORM_WIN32) && !defined(SURFER_PLATFORM_X11)
#if defined(_WIN32) || defined(_WIN64)
#define SURFER_PLATFORM_WIN32
#elif defined(__linux__)
#define SURFER_PLATFORM_X11
#elif defined(__APPLE__)
#define SURFER_PLATFORM_METAL
#endif
#endif

// Define Vulkan platform macros before including Vulkan
#if defined(SURFER_PLATFORM_WIN32)
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#elif defined(SURFER_PLATFORM_X11)
#ifndef VK_USE_PLATFORM_XLIB_KHR
#define VK_USE_PLATFORM_XLIB_KHR
#endif
#endif

// Include Vulkan headers
#include <vulkan/vulkan.h>


// Platform-specific includes
#if defined(SURFER_PLATFORM_WIN32)
#include <windows.h>
#include <windowsx.h>
#include <vulkan/vulkan_win32.h>
#endif

#if defined(SURFER_PLATFORM_X11)
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <vulkan/vulkan_xlib.h>
#endif

namespace Surfer {
    enum class KeyCode {
        // Alphabet
        KeyA, KeyB, KeyC, KeyD, KeyE, KeyF, KeyG, KeyH, KeyI, KeyJ, KeyK, KeyL, KeyM, KeyN, KeyO, KeyP, KeyQ, KeyR,
        KeyS, KeyT, KeyU, KeyV, KeyW, KeyX, KeyY, KeyZ,

        // Numbers
        Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

        // Numpad
        Numpad0, Numpad1, Numpad2, Numpad3, Numpad4, Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
        NumpadEnter, NumpadMultiply, NumpadSubtract, NumpadAdd, NumpadDecimal, NumpadDivide, NumpadEqual,

        // Function Keys
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

        // Modifiers
        LeftShift, RightShift, LeftControl, RightControl, LeftAlt, RightAlt, SuperLeft, SuperRight,
        CapsLock, NumLock, ScrollLock,

        // Arrows
        ArrowUp, ArrowDown, ArrowRight, ArrowLeft,

        // Navigation
        Home, End, PageUp, PageDown,

        // Editing
        BackSpace, Delete, Insert,

        // Special
        Enter, Space, Tab, Esc, PrtSc, Pause,

        // Symbols
        Minus, Equal, BracketLeft, BracketRight, Semicolon, Apostrophe, Comma, Period, Slash, Backslash, Grave,

        // Mouse Buttons
        MouseLeft, MouseRight, MouseMiddle,
        MouseWheelUp, MouseWheelDown,

        // Unspecified/Future Extensions
        UnsupportedKey
    };

    typedef std::function<void(KeyCode key)> KeyPressCallback;
    typedef std::function<void(KeyCode key)> KeyReleaseCallback;
    typedef std::function<void(uint32_t x, uint32_t y)> MouseMotionCallback;
    typedef std::function<void(uint32_t width, uint32_t height)> ResizeCallback;
    typedef std::function<void(int32_t x, int32_t y)> MoveCallback;
    typedef std::function<void()> CloseCallback;
    typedef std::function<void(bool entered)> MouseEnterExitCallback;
    typedef std::function<void(bool focused)> FocusCallback;

#if defined(SURFER_PLATFORM_WIN32)
    typedef std::function<void(WPARAM wParam)> NativeKeyPressCallback;
    typedef std::function<void(WPARAM wParam)> NativeKeyReleaseCallback;
#elif defined(SURFER_PLATFORM_X11)
    typedef std::function<void(KeySym keySym)> NativeKeyPressCallback;
    typedef std::function<void(KeySym keySym)> NativeKeyReleaseCallback;
#endif

    /**
     * Represents a single window
     */
    class Window {
    public:
        // We delete all constructors except one that is private
        Window() = delete; // No default constructor
        Window(const Window &) = delete; // No copy constructor
        Window(Window &&) = delete; // No move constructor
        Window &operator=(const Window &) = delete; // No copy assignment
        Window &operator=(Window &&) = delete; // No move assignment

        /**
         *  Creates and opens a window
         * @param title Title of the windows that will be displayed in the title bar
         * @param width Horizontal resolution of the window in pixels
         * @param height Vertical resolution of the window in pixels
         * @param x Horizontal position of the windows on the screen
         * @param y Vertical position of the window on the screen
         * @return Window pointer
         */
        static Window *createWindow(const std::string &title, const uint32_t width,
                                    const uint32_t height, const int32_t x, const int32_t y) {
            return new Window(title, width, height, x, y);
        }

        /**
         * Destroy a window
         * @param window Window pointer
         */
        static void destroyWindow(const Window *window) {
            delete window;
        }

        /**
         * Poll for events
         */
        void pollEvents() {
#if defined(SURFER_PLATFORM_WIN32)
            Win32_pollEvents();
#elif defined(SURFER_PLATFORM_X11)
            X11_pollEvents();
#endif
        }

        /**
         * Checks if window should be closed (e.g. user closed the window)
         * @return True if window should be closed, else returns false
         */
        bool shouldClose() const { return _shouldClose; }


        /**
         * Function returns cursors position in window relative to top left corner
         * @param x Horizontal position reference
         * @param y Vertical position reference
         */
        void getCursorPosition(unsigned int &x, unsigned int &y) const {
            x = _mouse_x;
            y = _mouse_y;
        }

        /**
         * @brief Function retrieves window position on screen
         * @param x Horizontal position
         * @param y Vertical position
         */
        void getWindowPosition(unsigned int &x, unsigned int &y) const {
            x = _x;
            y = _y;
        }

        /**
         * @brief Function retrieves window size (resolution)
         * @param width Horizontal resolution in pixels
         * @param height Vertical resolution in pixels
         */
        void getWindowSize(unsigned int &width, unsigned int &height) const {
            width = _width;
            height = _height;
        }

#if defined(SURFER_PLATFORM_WIN32)
        HWND getNativeWindowPtr() const { return Win32_hWnd; }
#elif defined(SURFER_PLATFORM_X11)
        ::Window getNativeWindowPtr() const { return X11_window; }
#endif

        /**
         * Registers a callback that is triggered when a key is pressed. This can be a keyboard key or mouse key or any other.
         * @param callback KeyPressCallback function
         */
        void registerKeyPressCallback(const KeyPressCallback &callback) { this->_keyPressCallback = callback; }

        /**
         * Registers a callback that is triggered when a key is released. This can be a keyboard key or mouse key or any other.
         * @param callback KeyReleaseCallback function
         */
        void registerKeyReleaseCallback(const KeyReleaseCallback &callback) { this->_keyReleaseCallback = callback; }

        /**
         * Registers a callback that is triggered when a mouse is moved (cursor position has changed)
         * @param callback MouseMotionCallback function
         */
        void registerMouseMotionCallback(const MouseMotionCallback &callback) { this->_mouseMotionCallback = callback; }

        /**
         * Registers a callback that is triggered when the window is resized
         * @param callback ResizeCallback function
         */
        void registerResizeCallback(const ResizeCallback &callback) { this->_resizeCallback = callback; }

        /**
         * Registers a callback that is triggered when the window is moved on the screen.
         * @param callback MoveCallback function
         */
        void registerMoveCallback(const MoveCallback &callback) { this->_moveCallback = callback; }

        /**
         * Registers a callback that is triggered when the window is closed.
         * @param callback CloseCallback function
         */
        void registerCloseCallback(const CloseCallback &callback) { this->_closeCallback = callback; }

        /**
         * Registers a callback that is triggered when a mouse enter (true) or exits (false) the window
         * @param callback MouseEnterExitCallback function
         */
        void registerMouseEnterExitCallback(const MouseEnterExitCallback &callback) {
            this->_mouseEnterExitCallback = callback;
        }

        /**
         * Registers a callback that is triggered when the window becomes in focus (true) or loses focus (false)
         * @param callback FocusCallback function
         */
        void registerFocusCallback(const FocusCallback &callback) { this->_focusCallback = callback; }

        /**
         * Registers a callback that is triggered when any key is press and native KeySym is passed to the callback
         * This can be used if a needed key mapping is not supported by Surfer (un-complete mapping or unsupported key)
         * @param callback NativeKeyPressCallback function
         */
        void registerNativeKeyPressCallback(const NativeKeyPressCallback &callback) {
            this->_nativeKeyPressCallback = callback;
        }

        /**
         * Registers a callback that is triggered when any key is released and native KeySym is passed to the callback
         * This can be used if a needed key mapping is not supported by Surfer (un-complete mapping or unsupported key)
         * @param callback NativeKeyReleaseCallback function
         */
        void registerNativeKeyReleaseCallback(const NativeKeyReleaseCallback &callback) {
            this->_nativeKeyReleaseCallback = callback;
        }

        VkResult createSurface(VkInstance instance, VkSurfaceKHR *surface) {
#if defined(SURFER_PLATFORM_WIN32)
            return Win32_CreateSurface(instance, surface);
#elif defined(SURFER_PLATFORM_X11)
            return X11_CreateSurface(instance, surface);
#endif
        }

    protected:
        Window(const std::string &title, const uint32_t width, const uint32_t height,
               const int32_t x, const int32_t y) {
            if (width == 0 || height == 0) {
                throw std::runtime_error("VulkanSurfer: width == 0 || height == 0");;
            }
#if defined(SURFER_PLATFORM_WIN32)
            Win32_createWindow(title, width, height, x, y);
#elif defined(SURFER_PLATFORM_X11)
            X11_createWindow(title, width, height, x, y);
#endif
        }

        ~Window() {
#if defined(SURFER_PLATFORM_WIN32)
            Win32_destroyWindow();
#elif defined(SURFER_PLATFORM_X11)
            X11_destroyWindow();
#endif
        }


        // State
        bool _shouldClose = false;
        uint32_t _width = 0u, _height = 0u;
        int32_t _x = 0, _y = 0;
        uint32_t _mouse_x = 0u, _mouse_y = 0u;
        bool _focused = false, _mouseEntered = false;
        std::string _title{};

        // Callbacks
        KeyPressCallback _keyPressCallback = nullptr;
        KeyReleaseCallback _keyReleaseCallback = nullptr;
        MouseMotionCallback _mouseMotionCallback = nullptr;
        ResizeCallback _resizeCallback = nullptr;
        MoveCallback _moveCallback = nullptr;
        CloseCallback _closeCallback = nullptr;
        MouseEnterExitCallback _mouseEnterExitCallback = nullptr;
        FocusCallback _focusCallback = nullptr;
        NativeKeyPressCallback _nativeKeyPressCallback = nullptr;
        NativeKeyReleaseCallback _nativeKeyReleaseCallback = nullptr;


#if defined(SURFER_PLATFORM_WIN32)
        HWND Win32_hWnd;
        HINSTANCE Win32_hInstance;
        MSG Win32_msg{};
        bool Win32_resizing = false;


        void Win32_createWindow(const std::string &title, const uint32_t width,
                                const uint32_t height, const int32_t x, const int32_t y) {
            _width = width;
            _height = height;
            _x = x;
            _y = y;
            _title = title;

            SetProcessDPIAware();

            Win32_hInstance = GetModuleHandle(nullptr);

            WNDCLASSEX wc = {};
            wc.cbSize = sizeof(WNDCLASSEX);
            wc.style = CS_HREDRAW | CS_VREDRAW;
            wc.lpfnWndProc = this->WndProc;
            wc.hInstance = Win32_hInstance;
            wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
            wc.lpszClassName = title.c_str();

            if (!RegisterClassEx(&wc)) {
                throw std::runtime_error("VulkanSurfer: Failed to register window class");
            }

            // Calculate window rect to account for borders and title bar
            RECT windowRect = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
            AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, 0);
            int adjustedWidth = windowRect.right - windowRect.left;
            int adjustedHeight = windowRect.bottom - windowRect.top;

            Win32_hWnd = CreateWindowEx(
                0,
                wc.lpszClassName,
                title.c_str(),
                WS_OVERLAPPEDWINDOW,
                x, y,
                adjustedWidth, adjustedHeight, // Use adjusted dimensions
                nullptr,
                nullptr,
                wc.hInstance,
                this);

            if (!Win32_hWnd) {
                throw std::runtime_error("VulkanSurfer: Failed to create window");
            }

            SetWindowLongPtr(Win32_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
            ShowWindow(Win32_hWnd, SW_SHOW);
            UpdateWindow(Win32_hWnd);
        }

        void Win32_destroyWindow() {
            MSG msg;
            while (PeekMessage(&msg, Win32_hWnd, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            if (Win32_hWnd) {
                // Don't destroy if already being destroyed
                if (IsWindow(Win32_hWnd)) {
                    // This triggers WM_DESTROY
                    DestroyWindow(Win32_hWnd);
                }
                Win32_hWnd = nullptr;
            }

            // Only unregister if we're the last window using this class
            WNDCLASSEX wc = {};
            wc.cbSize = sizeof(WNDCLASSEX);
            if (GetClassInfoEx(GetModuleHandle(nullptr), _title.c_str(), &wc)) {
                UnregisterClass(_title.c_str(), GetModuleHandle(nullptr));
            }
        }

        VkResult Win32_CreateSurface(VkInstance instance, VkSurfaceKHR *surface) const {
            VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
            surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            surfaceCreateInfo.hwnd = Win32_hWnd;
            surfaceCreateInfo.hinstance = Win32_hInstance;

            return vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, surface);
        }

        void Win32_pollEvents() {
            while (PeekMessage(&Win32_msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&Win32_msg);
                DispatchMessage(&Win32_msg);
            }
        }

        /// internal window procedure function for Win32 API
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
            Window *window = reinterpret_cast<Window *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

            switch (uMsg) {
                case WM_CLOSE:
                    if (window) {
                        window->Win32_onClose();
                    }
                    return 0;
                case WM_DESTROY: {
                    SetWindowLongPtr(hWnd, GWLP_USERDATA, 0); // Clear user data
                    PostQuitMessage(0);
                    if (window) {
                        window->Win32_onDestroy();
                    }
                    return 0;
                }
                case WM_PAINT:
                    ValidateRect(hWnd, NULL);
                    return 0;
                case WM_KEYDOWN:
                    if (window) {
                        window->Win32_onKeyDown(wParam);
                    }
                    return 0;
                case WM_KEYUP:
                    if (window) {
                        window->Win32_onKeyUp(wParam);
                    }
                    return 0;
                case WM_LBUTTONDOWN: {
                    if (window) {
                        window->Win32_onButtonPress(WM_LBUTTONDOWN);
                    }
                    return 0;
                }
                case WM_RBUTTONDOWN: {
                    if (window) {
                        window->Win32_onButtonPress(WM_RBUTTONDOWN);
                    }
                    return 0;
                }
                case WM_MBUTTONDOWN: {
                    if (window) {
                        window->Win32_onButtonPress(WM_MBUTTONDOWN);
                    }
                    return 0;
                }
                case WM_LBUTTONUP: {
                    if (window) {
                        window->Win32_onButtonRelease(WM_LBUTTONUP);
                    }
                    return 0;
                }
                case WM_RBUTTONUP: {
                    if (window) {
                        window->Win32_onButtonRelease(WM_RBUTTONUP);
                    }
                    return 0;
                }
                case WM_MBUTTONUP: {
                    if (window) {
                        window->Win32_onButtonRelease(WM_MBUTTONUP);
                    }
                    return 0;
                }
                case WM_MOUSEMOVE: {
                    int xPos = GET_X_LPARAM(lParam);
                    int yPos = GET_Y_LPARAM(lParam);
                    if (window) {
                        window->Win32_onMouseMove(xPos, yPos);

                        TRACKMOUSEEVENT tme = {};
                        tme.cbSize = sizeof(tme);
                        tme.dwFlags = TME_LEAVE;
                        tme.hwndTrack = window->Win32_hWnd;
                        TrackMouseEvent(&tme);

                        if (!window->_mouseEntered) {
                            window->Win32_onMouseEnter();
                        }
                    }
                    return 0;
                }
                case WM_MOUSELEAVE: {
                    if (window) {
                        window->Win32_onMouseLeave();
                    }
                    return 0;
                }
                case WM_SIZE: // size changed
                    if (window && wParam != SIZE_MINIMIZED) {
                        if ((window->Win32_resizing) || ((wParam == SIZE_MAXIMIZED) || (wParam == SIZE_RESTORED))) {
                            uint32_t width = LOWORD(lParam);
                            uint32_t height = HIWORD(lParam);
                            window->Win32_onResize(width, height);
                        }
                    }
                    return 0;
                case WM_ENTERSIZEMOVE: // resizing started
                    window->Win32_resizing = true;
                    return 0;
                case WM_EXITSIZEMOVE: // resizing stopped
                    window->Win32_resizing = false;
                    return 0;
                case WM_DPICHANGED:
                    if (window) {
                        window->Win32_onDpiChange(hWnd, wParam, lParam);
                    }
                    return 0;
                case WM_MOVE: {
                    if (window) {
                        window->Win32_onMove(lParam);
                    }
                    return 0;
                }
                case WM_SETFOCUS: {
                    if (window) {
                        window->Win32_onFocusIn();
                    }
                }
                case WM_KILLFOCUS: {
                    if (window && !window->shouldClose()) {
                        window->Win32_onFocusOut();
                    }
                }
                default:
                    return DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
        }

        void Win32_onMouseEnter() {
            _mouseEntered = true;
            if (_mouseEnterExitCallback != nullptr) {
                _mouseEnterExitCallback(true);
            }
        }


        void Win32_onMouseLeave() {
            _mouseEntered = false;
            if (_mouseEnterExitCallback != nullptr) {
                _mouseEnterExitCallback(false);
            }
        }

        void Win32_onKeyDown(WPARAM key) {
            if (_keyPressCallback != nullptr) {
                _keyPressCallback(Win32_translateKeyCode(key));
            }

            if (_nativeKeyPressCallback != nullptr) {
                _nativeKeyPressCallback(key);
            }
        }

        void Win32_onKeyUp(WPARAM key) {
            if (_keyReleaseCallback != nullptr) {
                _keyReleaseCallback(Win32_translateKeyCode(key));
            }

            if (_nativeKeyReleaseCallback != nullptr) {
                _nativeKeyReleaseCallback(key);
            }
        }

        void Win32_onClose() {
            // Set should close first
            _shouldClose = true;

            // Wait for any pending messages
            MSG msg;
            while (PeekMessage(&msg, Win32_hWnd, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        void Win32_onDestroy() {
            if (_closeCallback != nullptr)
                _closeCallback();
        }

        void Win32_onMouseMove(int xPos, int yPos) {
            if (_mouse_x != xPos || _mouse_y != yPos) {
                // save the internal state
                _mouse_x = xPos;
                _mouse_y = yPos;

                if (_mouseMotionCallback != nullptr) {
                    _mouseMotionCallback(xPos, yPos);
                }
            }
        }

        void Win32_onButtonPress(UINT button) {
            switch (button) {
                case WM_LBUTTONDOWN: {
                    if (_keyPressCallback != nullptr) {
                        _keyPressCallback(KeyCode::MouseLeft);
                    }
                    break;
                }
                case WM_RBUTTONDOWN: {
                    if (_keyPressCallback != nullptr) {
                        _keyPressCallback(KeyCode::MouseRight);
                    }
                    break;
                }
                case WM_MBUTTONDOWN: {
                    if (_keyPressCallback != nullptr) {
                        _keyPressCallback(KeyCode::MouseMiddle);
                    }
                    break;
                }
            }
        }

        void Win32_onButtonRelease(UINT button) {
            switch (button) {
                case WM_LBUTTONUP: {
                    if (_keyReleaseCallback != nullptr) {
                        _keyReleaseCallback(KeyCode::MouseLeft);
                    }
                    break;
                }
                case WM_RBUTTONUP: {
                    if (_keyReleaseCallback != nullptr) {
                        _keyReleaseCallback(KeyCode::MouseRight);
                    }
                    break;
                }
                case WM_MBUTTONUP: {
                    if (_keyReleaseCallback != nullptr) {
                        _keyReleaseCallback(KeyCode::MouseMiddle);
                    }
                    break;
                }
            }
        }

        void Win32_onResize(uint32_t width, uint32_t height) {
            _width = width;
            _height = height;

            if (_resizeCallback != nullptr) {
                _resizeCallback(width, height);
            }
        }

        void Win32_onMove(LPARAM lParam) {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);

            if (x != _x || y != _y) {
                _x = x;
                _y = y;

                if (_moveCallback != nullptr) {
                    _moveCallback(x, y);
                }
            }
        }

        void Win32_onDpiChange(HWND hWnd, WPARAM wParam, LPARAM lParam) {
            UINT dpi = HIWORD(wParam);
            RECT *const pRect = reinterpret_cast<RECT *>(lParam);

            if (pRect) {
                SetWindowPos(hWnd,
                             nullptr,
                             pRect->left,
                             pRect->top,
                             pRect->right - pRect->left,
                             pRect->bottom - pRect->top,
                             SWP_NOZORDER | SWP_NOACTIVATE);
            }
        }

        void Win32_onFocusIn() {
            _focused = true;
            if (_focusCallback != nullptr) {
                _focusCallback(true);
            }
        }

        void Win32_onFocusOut() {
            _focused = false;
            if (_focusCallback != nullptr) {
                _focusCallback(false);
            }
        }

        KeyCode Win32_translateKeyCode(WPARAM key) {
            // Contiguous Ranges (Alphabet, Numbers, F-Keys, Numpad 0-9)

            // Alphabet (A - Z)
            if (key >= 0x41 && key <= 0x5A) {
                return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::KeyA) + (key - 0x41));
            }

            // Numbers (0 - 9)
            if (key >= 0x30 && key <= 0x39) {
                return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::Num0) + (key - 0x30));
            }

            // Numpad (0 - 9)
            if (key >= VK_NUMPAD0 && key <= VK_NUMPAD9) {
                return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::Numpad0) + (key - VK_NUMPAD0));
            }

            // Function Keys (F1 - F12)
            if (key >= VK_F1 && key <= VK_F12) {
                return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::F1) + (key - VK_F1));
            }

            // Specific Keys
            switch (key) {
                // Arrows
                case VK_UP: return KeyCode::ArrowUp;
                case VK_DOWN: return KeyCode::ArrowDown;
                case VK_LEFT: return KeyCode::ArrowLeft;
                case VK_RIGHT: return KeyCode::ArrowRight;

                // Navigation
                case VK_HOME: return KeyCode::Home;
                case VK_END: return KeyCode::End;
                case VK_PRIOR: return KeyCode::PageUp;
                case VK_NEXT: return KeyCode::PageDown;

                // Editing
                case VK_BACK: return KeyCode::BackSpace;
                case VK_DELETE: return KeyCode::Delete;
                case VK_INSERT: return KeyCode::Insert;

                // Special
                case VK_ESCAPE: return KeyCode::Esc;
                case VK_SPACE: return KeyCode::Space;
                case VK_TAB: return KeyCode::Tab;
                case VK_RETURN: return KeyCode::Enter; // NumpadEnter requires checking LPARAM (Extended Bit)
                case VK_SNAPSHOT: return KeyCode::PrtSc;
                case VK_PAUSE: return KeyCode::Pause;

                // Numpad Math
                case VK_MULTIPLY: return KeyCode::NumpadMultiply;
                case VK_ADD: return KeyCode::NumpadAdd;
                case VK_SUBTRACT: return KeyCode::NumpadSubtract;
                case VK_DECIMAL: return KeyCode::NumpadDecimal;
                case VK_DIVIDE: return KeyCode::NumpadDivide;

                // Symbols (US Keyboard Layout Mappings)
                case VK_OEM_PLUS: return KeyCode::Equal; // Shift + '=' is '+'
                case VK_OEM_MINUS: return KeyCode::Minus;
                case VK_OEM_COMMA: return KeyCode::Comma;
                case VK_OEM_PERIOD: return KeyCode::Period;
                case VK_OEM_1: return KeyCode::Semicolon; // ;:
                case VK_OEM_2: return KeyCode::Slash; // /?
                case VK_OEM_3: return KeyCode::Grave; // `~
                case VK_OEM_4: return KeyCode::BracketLeft; // [{
                case VK_OEM_5: return KeyCode::Backslash; // \|
                case VK_OEM_6: return KeyCode::BracketRight; // ]}
                case VK_OEM_7: return KeyCode::Apostrophe; // '"

                // Locks
                case VK_CAPITAL: return KeyCode::CapsLock;
                case VK_NUMLOCK: return KeyCode::NumLock;
                case VK_SCROLL: return KeyCode::ScrollLock;

                // Modifiers
                // Note: WM_KEYDOWN usually sends generic VK_SHIFT/CONTROL/MENU.
                // We check GetKeyState to distinguish Left vs Right.

                case VK_SHIFT:
                    if (GetKeyState(VK_LSHIFT) & 0x8000) return KeyCode::LeftShift;
                    if (GetKeyState(VK_RSHIFT) & 0x8000) return KeyCode::RightShift;
                    return KeyCode::LeftShift; // Fallback

                case VK_CONTROL:
                    if (GetKeyState(VK_LCONTROL) & 0x8000) return KeyCode::LeftControl;
                    if (GetKeyState(VK_RCONTROL) & 0x8000) return KeyCode::RightControl;
                    return KeyCode::LeftControl;

                case VK_MENU: // Alt Key
                    if (GetKeyState(VK_LMENU) & 0x8000) return KeyCode::LeftAlt;
                    if (GetKeyState(VK_RMENU) & 0x8000) return KeyCode::RightAlt;
                    return KeyCode::LeftAlt;

                case VK_LWIN: return KeyCode::SuperLeft;
                case VK_RWIN: return KeyCode::SuperRight;

                // Direct mappings if Windows sends specific VKs
                case VK_LSHIFT: return KeyCode::LeftShift;
                case VK_RSHIFT: return KeyCode::RightShift;
                case VK_LCONTROL: return KeyCode::LeftControl;
                case VK_RCONTROL: return KeyCode::RightControl;
                case VK_LMENU: return KeyCode::LeftAlt;
                case VK_RMENU: return KeyCode::RightAlt;

                default: return KeyCode::UnsupportedKey;
            }
        }


#elif defined(SURFER_PLATFORM_X11)
        Display *X11_display;
        ::Window X11_window;
        ::Window X11_root;
        Atom X11_wmDeleteMessage;

        void X11_createWindow(const std::string &title, const uint32_t width,
                              const uint32_t height, const int32_t x, const int32_t y) {
            _width = width;
            _height = height;
            _x = x;
            _y = y;
            _title = title;

            X11_display = XOpenDisplay(nullptr);
            if (!X11_display) {
                throw std::runtime_error("VulkanSurfer: Failed to open X display");
            }

            X11_root = DefaultRootWindow(X11_display);
            XSetWindowAttributes windowAttributes;
            windowAttributes.background_pixel = WhitePixel(X11_display, 0);
            windowAttributes.event_mask =
                    ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
                    PointerMotionMask | StructureNotifyMask | FocusChangeMask | EnterWindowMask |
                    LeaveWindowMask | ButtonMotionMask | FocusChangeMask | KeymapStateMask |
                    ExposureMask;

            X11_window = XCreateWindow(X11_display, X11_root, x, y, width, height, 0, CopyFromParent, InputOutput,
                                       CopyFromParent, CWBackPixel | CWEventMask, &windowAttributes);

            XStoreName(X11_display, X11_window, title.c_str());
            XMapWindow(X11_display, X11_window);
            XFlush(X11_display);

            X11_wmDeleteMessage = XInternAtom(X11_display, "WM_DELETE_WINDOW", False);
            XSetWMProtocols(X11_display, X11_window, &X11_wmDeleteMessage, 1);

            // Enable drag-and-drop support for the window (file drop handling)
            Atom XdndAware = XInternAtom(X11_display, "XdndAware", False);
            XChangeProperty(X11_display, X11_window, XdndAware, XA_ATOM, 32, PropModeReplace,
                            (unsigned char *) &XdndAware, 1);
        }

        void X11_destroyWindow() {
            if (X11_window) {
                XDestroyWindow(X11_display, X11_window);
                X11_window = 0;
            }

            if (X11_display) {
                XCloseDisplay(X11_display);
                X11_display = nullptr;
            }
        }

        VkResult X11_CreateSurface(VkInstance instance, VkSurfaceKHR *surface) {
            VkXlibSurfaceCreateInfoKHR surfaceInfo = {};
            surfaceInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
            surfaceInfo.dpy = X11_display;
            surfaceInfo.window = X11_window;

            return vkCreateXlibSurfaceKHR(instance, &surfaceInfo, nullptr, surface);
        }

        void X11_pollEvents() {
            XEvent event;
            while (XPending(X11_display) > 0) {
                XNextEvent(X11_display, &event);
                X11_processEvent(event);
            }
        }

        void X11_processEvent(XEvent event) {
            switch (event.type) {
                case ClientMessage: {
                    X11_onClose();
                    break;
                }
                case KeyPress: {
                    X11_onKeyPress(event.xkey.keycode);
                    break;
                }
                case KeyRelease: {
                    X11_onKeyRelease(event.xkey.keycode);
                    break;
                }
                case ButtonPress: {
                    X11_onButtonPress(event.xbutton.button);
                    break;
                }
                case ButtonRelease: {
                    X11_onButtonRelease(event.xbutton.button);
                    break;
                }
                case MotionNotify: {
                    X11_onMouseMotion(event.xmotion.x, event.xmotion.y);
                    break;
                }
                case ConfigureNotify: {
                    const XConfigureEvent xce = event.xconfigure;

                    const bool moved = (xce.x != _x || xce.y != _y);
                    const bool resized = (xce.width != _width || xce.height != _height);

                    if (moved) {
                        X11_onMove(xce.x, xce.y);
                    }

                    if (resized) {
                        X11_onResize(xce.width, xce.height);
                    }

                    break;
                }
                case FocusIn: {
                    X11_onFocusIn();
                    break;
                }
                case FocusOut: {
                    X11_onFocusOut();
                    break;
                }
                case EnterNotify: {
                    X11_onMouseEnter();
                    break;
                }
                case LeaveNotify: {
                    X11_onMouseLeave();
                    break;
                }
            }
        }

        void X11_onClose() {
            _shouldClose = true;

            XEvent event;
            while (XPending(X11_display) > 0) {
                XNextEvent(X11_display, &event);
            }

            if (_closeCallback != nullptr) {
                _closeCallback();
            }
        }

        void X11_onKeyPress(unsigned int x11KeyCode) {
            KeySym keySym = XkbKeycodeToKeysym(X11_display, x11KeyCode, 0, 0);
            const KeyCode transledKeyCode = X11_translateKeyCode(keySym);

            if (_keyPressCallback != nullptr) {
                _keyPressCallback(transledKeyCode);
            }

            if (_nativeKeyPressCallback != nullptr) {
                _nativeKeyPressCallback(keySym);
            }
        }

        void X11_onKeyRelease(unsigned int x11KeyCode) {
            KeySym keySym = XkbKeycodeToKeysym(X11_display, x11KeyCode, 0, 0);
            const KeyCode transledKeyCode = X11_translateKeyCode(keySym);

            if (_keyReleaseCallback != nullptr) {
                _keyReleaseCallback(transledKeyCode);
            }

            if (_nativeKeyReleaseCallback != nullptr) {
                _nativeKeyReleaseCallback(keySym);
            }
        }

        void X11_onButtonPress(unsigned int x11button) {
            switch (x11button) {
                // directly calls the callback as the translation function cannot translate mouse
                case Button1: {
                    if (_keyPressCallback != nullptr) {
                        _keyPressCallback(KeyCode::MouseLeft);
                    }
                    break;
                }
                case Button2: {
                    if (_keyPressCallback != nullptr) {
                        _keyPressCallback(KeyCode::MouseRight);
                    }
                    break;
                }
                case Button3: {
                    if (_keyPressCallback != nullptr) {
                        _keyPressCallback(KeyCode::MouseMiddle);
                    }
                    break;
                }
                case Button4: {
                    if (_keyPressCallback != nullptr) {
                        _keyPressCallback(KeyCode::MouseWheelUp);
                    }
                    break;
                }
                case Button5: {
                    if (_keyPressCallback != nullptr) {
                        _keyPressCallback(KeyCode::MouseWheelDown);
                    }
                    break;
                }
                default: {
                    if (_keyPressCallback != nullptr) {
                        _keyPressCallback(KeyCode::UnsupportedKey);
                    }
                }
            }
        }

        void X11_onButtonRelease(unsigned int x11button) {
            switch (x11button) {
                // directly calls the callback as the translation function cannot translate mouse
                case Button1: {
                    if (_keyReleaseCallback != nullptr) {
                        _keyReleaseCallback(KeyCode::MouseLeft);
                    }
                    break;
                }
                case Button2: {
                    if (_keyReleaseCallback != nullptr) {
                        _keyReleaseCallback(KeyCode::MouseRight);
                    }
                    break;
                }
                case Button3: {
                    if (_keyReleaseCallback != nullptr) {
                        _keyReleaseCallback(KeyCode::MouseMiddle);
                    }
                    break;
                }
                case Button4: {
                    if (_keyReleaseCallback != nullptr) {
                        _keyReleaseCallback(KeyCode::MouseWheelUp);
                    }
                    break;
                }
                case Button5: {
                    if (_keyReleaseCallback != nullptr) {
                        _keyReleaseCallback(KeyCode::MouseWheelDown);
                    }
                    break;
                }
                default: {
                    if (_keyReleaseCallback != nullptr) {
                        _keyReleaseCallback(KeyCode::UnsupportedKey);
                    }
                }
            }
        }

        void X11_onMove(int32_t x, int32_t y) {
            _x = x;
            _y = y;

            if (_moveCallback != nullptr) {
                _moveCallback(x, y);
            }
        }

        void X11_onResize(const uint32_t width, const uint32_t height) {
            // save the internal state
            _width = width;
            _height = height;

            if (_resizeCallback != nullptr) {
                _resizeCallback(width, height);
            }
        }

        void X11_onMouseMotion(uint32_t x, uint32_t y) {
            if (_mouse_x != x || _mouse_y != y) {
                // save the internal state
                _mouse_x = x;
                _mouse_y = y;

                if (_mouseMotionCallback != nullptr) {
                    _mouseMotionCallback(x, y);
                }
            }
        }

        void X11_onFocusIn() {
            _focused = true;
            if (_focusCallback != nullptr) {
                _focusCallback(true);
            }
        }

        void X11_onFocusOut() {
            _focused = false;
            if (_focusCallback != nullptr) {
                _focusCallback(false);
            }
        }

        void X11_onMouseEnter() {
            _mouseEntered = true;
            if (_mouseEnterExitCallback != nullptr) {
                _mouseEnterExitCallback(true);
            }
        }

        void X11_onMouseLeave() {
            _mouseEntered = false;
            if (_mouseEnterExitCallback != nullptr) {
                _mouseEnterExitCallback(false);
            }
        }

        KeyCode X11_translateKeyCode(KeySym keySym) {
            // Contiguous Ranges

            // Alphabet (a-z, A-Z)
            if (keySym >= XK_a && keySym <= XK_z) {
                return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::KeyA) + (keySym - XK_a));
            }
            if (keySym >= XK_A && keySym <= XK_Z) {
                return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::KeyA) + (keySym - XK_A));
            }

            // Numbers (0-9)
            if (keySym >= XK_0 && keySym <= XK_9) {
                return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::Num0) + (keySym - XK_0));
            }

            // Numpad Numbers (0-9)
            if (keySym >= XK_KP_0 && keySym <= XK_KP_9) {
                return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::Numpad0) + (keySym - XK_KP_0));
            }

            // Function Keys (F1 - F12)
            // Note: XK_F1 to XK_F12 are contiguous in X11 (0xFFBE to 0xFFC9)
            if (keySym >= XK_F1 && keySym <= XK_F12) {
                return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::F1) + (keySym - XK_F1));
            }

            // Specific Key Mappings
            switch (keySym) {
                // Arrows
                case XK_Up: return KeyCode::ArrowUp;
                case XK_Down: return KeyCode::ArrowDown;
                case XK_Left: return KeyCode::ArrowLeft;
                case XK_Right: return KeyCode::ArrowRight;

                // Navigation
                case XK_Home: return KeyCode::Home;
                case XK_End: return KeyCode::End;
                case XK_Page_Up: return KeyCode::PageUp;
                case XK_Page_Down: return KeyCode::PageDown;

                // Editing
                case XK_BackSpace: return KeyCode::BackSpace;
                case XK_Delete: return KeyCode::Delete;
                case XK_Insert: return KeyCode::Insert;

                // Special
                case XK_Return: return KeyCode::Enter;
                case XK_space: return KeyCode::Space;
                case XK_Tab: return KeyCode::Tab;
                case XK_Escape: return KeyCode::Esc;
                case XK_Print: return KeyCode::PrtSc;
                case XK_Pause: return KeyCode::Pause;
                case XK_Break: return KeyCode::Pause;

                // Modifiers
                case XK_Shift_L: return KeyCode::LeftShift;
                case XK_Shift_R: return KeyCode::RightShift;
                case XK_Control_L: return KeyCode::LeftControl;
                case XK_Control_R: return KeyCode::RightControl;
                case XK_Alt_L: return KeyCode::LeftAlt;
                case XK_Alt_R: return KeyCode::RightAlt;
                case XK_Super_L: return KeyCode::SuperLeft;
                case XK_Super_R: return KeyCode::SuperRight;
                case XK_Caps_Lock: return KeyCode::CapsLock;
                case XK_Num_Lock: return KeyCode::NumLock;
                case XK_Scroll_Lock: return KeyCode::ScrollLock;
                case XK_ISO_Level3_Shift: return KeyCode::RightAlt; // AltGr

                // Numpad (Non-Digits)
                case XK_KP_Enter: return KeyCode::NumpadEnter;
                case XK_KP_Multiply: return KeyCode::NumpadMultiply;
                case XK_KP_Add: return KeyCode::NumpadAdd;
                case XK_KP_Subtract: return KeyCode::NumpadSubtract;
                case XK_KP_Decimal: return KeyCode::NumpadDecimal;
                case XK_KP_Divide: return KeyCode::NumpadDivide;
                case XK_KP_Equal: return KeyCode::NumpadEqual;

                // Numpad Navigation (When NumLock is off)
                case XK_KP_Insert: return KeyCode::Numpad0;
                case XK_KP_End: return KeyCode::Numpad1;
                case XK_KP_Down: return KeyCode::Numpad2;
                case XK_KP_Page_Down: return KeyCode::Numpad3;
                case XK_KP_Left: return KeyCode::Numpad4;
                case XK_KP_Begin: return KeyCode::Numpad5;
                case XK_KP_Right: return KeyCode::Numpad6;
                case XK_KP_Home: return KeyCode::Numpad7;
                case XK_KP_Up: return KeyCode::Numpad8;
                case XK_KP_Page_Up: return KeyCode::Numpad9;
                case XK_KP_Delete: return KeyCode::NumpadDecimal;

                // Symbols (Standard US Layout & Shifted Variants)

                // Key: ` ~
                case XK_grave: return KeyCode::Grave;
                case XK_asciitilde: return KeyCode::Grave;

                // Key: - _
                case XK_minus: return KeyCode::Minus;
                case XK_underscore: return KeyCode::Minus;

                // Key: = +
                case XK_equal: return KeyCode::Equal;
                case XK_plus: return KeyCode::Equal;

                // Key: [ {
                case XK_bracketleft: return KeyCode::BracketLeft;
                case XK_braceleft: return KeyCode::BracketLeft;

                // Key: ] }
                case XK_bracketright: return KeyCode::BracketRight;
                case XK_braceright: return KeyCode::BracketRight;

                // Key: ; :
                case XK_semicolon: return KeyCode::Semicolon;
                case XK_colon: return KeyCode::Semicolon;

                // Key: ' "
                case XK_apostrophe: return KeyCode::Apostrophe;
                case XK_quotedbl: return KeyCode::Apostrophe;

                // Key: , <
                case XK_comma: return KeyCode::Comma;
                case XK_less: return KeyCode::Comma;

                // Key: . >
                case XK_period: return KeyCode::Period;
                case XK_greater: return KeyCode::Period;

                // Key: / ?
                case XK_slash: return KeyCode::Slash;
                case XK_question: return KeyCode::Slash;

                // Key: \ |
                case XK_backslash: return KeyCode::Backslash;
                case XK_bar: return KeyCode::Backslash;

                // Shifted Numbers (US Layout)
                case XK_exclam: return KeyCode::Num1;
                case XK_at: return KeyCode::Num2;
                case XK_numbersign: return KeyCode::Num3;
                case XK_dollar: return KeyCode::Num4;
                case XK_percent: return KeyCode::Num5;
                case XK_asciicircum: return KeyCode::Num6;
                case XK_ampersand: return KeyCode::Num7;
                case XK_asterisk: return KeyCode::Num8;
                case XK_parenleft: return KeyCode::Num9;
                case XK_parenright: return KeyCode::Num0;

                // Custom/International Mappings
                case XK_ecaron: return KeyCode::Num2;
                case XK_scaron: return KeyCode::Num3;
                case XK_ccaron: return KeyCode::Num4;
                case XK_rcaron: return KeyCode::Num5;
                case XK_zcaron: return KeyCode::Num6;
                case XK_yacute: return KeyCode::Num7;
                case XK_aacute: return KeyCode::Num8;
                case XK_iacute: return KeyCode::Num9;
                case XK_eacute: return KeyCode::Num0;
                case XK_dead_acute: return KeyCode::Equal;
                case XK_dead_diaeresis: return KeyCode::Backslash;
                case XK_uring: return KeyCode::Semicolon;
                case XK_section: return KeyCode::Apostrophe;

                default: return KeyCode::UnsupportedKey;
            }
        }


#endif
    };
}

#endif //VULKANSURFER_H
