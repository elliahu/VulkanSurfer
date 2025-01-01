#ifndef VULKANSURFER_H
#define VULKANSURFER_H

#include <functional>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#if defined(SURFER_PLATFORM_WIN32)
#include "windows.h"
#include <windowsx.h>
#endif
#if defined(SURFER_PLATFORM_X11)
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>
#include <X11/Xutil.h>
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
        F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, // Extended function keys

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
        Minus, Equal, BracketLeft, BracketRight, Semicolon, Quote, Comma, Period, Slash, Backslash, Grave,

        // Media Keys (Optional)
        MediaPlayPause, MediaStop, MediaNext, MediaPrevious,
        VolumeUp, VolumeDown, Mute,

        // System Keys
        Power, Sleep, WakeUp,

        // Language and Application Keys
        ContextMenu, Lang1, Lang2, // For international layouts

        // Mouse Buttons
        MouseLeft, MouseRight, MouseMiddle, MouseButton4, MouseButton5,
        MouseWheelUp, MouseWheelDown, MouseWheelLeft, MouseWheelRight,

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
         * @param instance VkInstance
         * @param width Horizontal resolution of the window in pixels
         * @param height Vertical resolution of the window in pixels
         * @param x Horizontal position of the windows on the screen
         * @param y Vertical position of the window on the screen
         * @return Window pointer
         */
        static Window *createWindow(const std::string &title, const VkInstance instance, const uint32_t width,
                                    const uint32_t height, const int32_t x, const int32_t y) {
            return new Window(title, instance, width, height, x, y);
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
         * Returns Vulkan surface
         * @return VkSurfaceKHR Vulkan surface
         */
        VkSurfaceKHR getSurface() const { return _surface; }

        void getCursorPosition(unsigned int &x, unsigned int &y) const {
            x = _mouse_x;
            y = _mouse_y;
        }

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

    protected:
        Window(const std::string &title, VkInstance instance, const uint32_t width, const uint32_t height,
               const int32_t x, const int32_t y): _instance(instance) {
            if (instance == VK_NULL_HANDLE) {
                throw std::runtime_error("VulkanSurfer: instance is null");;
            }

            if (width == 0 || height == 0) {
                throw std::runtime_error("VulkanSurfer: width == 0 || height == 0");;
            }
#if defined(SURFER_PLATFORM_WIN32)
            Win32_createWindow(title, instance, width, height, x, y);
#elif defined(SURFER_PLATFORM_X11)
            X11_createWindow(title, instance, width, height, x, y);
#endif
        }

        ~Window() {
#if defined(SURFER_PLATFORM_WIN32)
            Win32_destroyWindow();
#elif defined(SURFER_PLATFORM_X11)
            X11_destroyWindow();
#endif
        }

        // Vulkan
        VkSurfaceKHR _surface;
        VkInstance _instance;

        // State
        bool _shouldClose = false;
        uint32_t _width, _height;
        int32_t _x, _y;
        uint32_t _mouse_x, _mouse_y;
        bool _focused, _mouseEntered;
        std::string _title;

        // Callbacks
        KeyPressCallback _keyPressCallback = nullptr;
        KeyReleaseCallback _keyReleaseCallback = nullptr;
        MouseMotionCallback _mouseMotionCallback = nullptr;
        ResizeCallback _resizeCallback = nullptr;
        MoveCallback _moveCallback = nullptr;
        CloseCallback _closeCallback = nullptr;
        MouseEnterExitCallback _mouseEnterExitCallback = nullptr;
        FocusCallback _focusCallback = nullptr;


#if defined(SURFER_PLATFORM_WIN32)
        HWND Win32_hWnd;
        MSG Win32_msg{};
        bool Win32_resizing = false;


        void Win32_createWindow(const std::string &title, VkInstance instance, const uint32_t width,
                                const uint32_t height, const int32_t x, const int32_t y) {
            _width = width;
            _height = height;
            _x = x;
            _y = y;
            _title = title;

            SetProcessDPIAware();

            WNDCLASSEX wc = {};
            wc.cbSize = sizeof(WNDCLASSEX);
            wc.style = CS_HREDRAW | CS_VREDRAW;
            wc.lpfnWndProc = this->WndProc;
            wc.hInstance = GetModuleHandle(nullptr);
            wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
            wc.lpszClassName = title.c_str();

            if (!RegisterClassEx(&wc)) {
                throw std::runtime_error("VulkanSurfer: Failed to register window class");
            }

            // Calculate window rect to account for borders and title bar
            RECT windowRect = {0, 0, width, height};
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

            VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
            surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            surfaceCreateInfo.hwnd = Win32_hWnd;
            surfaceCreateInfo.hinstance = wc.hInstance;

            if (vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &_surface) != VK_SUCCESS) {
                throw std::runtime_error("VulkanSurfer: Failed to create Vulkan surface");
            }
        }

        void Win32_destroyWindow() {
            vkDestroySurfaceKHR(_instance, _surface, nullptr);
            _surface = VK_NULL_HANDLE;

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

                        if(!window->_mouseEntered) {
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
            if(_mouseEnterExitCallback != nullptr) {
                _mouseEnterExitCallback(true);
            }
        }


        void Win32_onMouseLeave() {
            _mouseEntered = false;
            if(_mouseEnterExitCallback != nullptr) {
                _mouseEnterExitCallback(false);
            }
        }

        void Win32_onKeyDown(WPARAM key) {
            if (_keyPressCallback != nullptr) {
                _keyPressCallback(Win32_translateKeyCode(key));
            }
        }

        void Win32_onKeyUp(WPARAM key) {
            if (_keyReleaseCallback != nullptr) {
                _keyReleaseCallback(Win32_translateKeyCode(key));
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
            _x = xPos;
            _y = yPos;

            if (_mouseMotionCallback != nullptr) {
                _mouseMotionCallback(_x, _y);
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
            // A - Z
            if (key >= 0x41 && key <= 0x5A) {
                return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::KeyA) + (key - 0x41));
            }

            // Numpad 0-9
            if (key >= VK_NUMPAD0 && key <= VK_NUMPAD9) {
                return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::Numpad0) + (key - VK_NUMPAD0));
            }

            return KeyCode::UnsupportedKey;
        }


#elif defined(SURFER_PLATFORM_X11)
        Display *X11_display;
        ::Window X11_window;
        ::Window X11_root;
        Atom X11_wmDeleteMessage;

        void X11_createWindow(const std::string &title, VkInstance instance, const uint32_t width,
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

            VkXlibSurfaceCreateInfoKHR surfaceInfo = {};
            surfaceInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
            surfaceInfo.dpy = X11_display;
            surfaceInfo.window = X11_window;

            VkResult result = vkCreateXlibSurfaceKHR(instance, &surfaceInfo, nullptr, &_surface);
            if (result != VK_SUCCESS) {
                throw std::runtime_error("VulkanSurfer: Failed to create Xlib surface");
            }

            this->_instance = instance;
        }

        void X11_destroyWindow() {
            vkDestroySurfaceKHR(_instance, _surface, nullptr);
            _surface = VK_NULL_HANDLE;

            if (X11_window) {
                XDestroyWindow(X11_display, X11_window);
                X11_window = 0;
            }

            if (X11_display) {
                XCloseDisplay(X11_display);
                X11_display = nullptr;
            }
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
            const KeyCode transledKeyCode = X11_translateKeyCode(x11KeyCode);

            if (_keyPressCallback != nullptr) {
                _keyPressCallback(transledKeyCode);
            }
        }

        void X11_onKeyRelease(unsigned int x11KeyCode) {
            const KeyCode transledKeyCode = X11_translateKeyCode(x11KeyCode);

            if (_keyReleaseCallback != nullptr) {
                _keyReleaseCallback(transledKeyCode);
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

        // Map raw X11 keycodes to KeyCode
        KeyCode X11_translateKeyCode(unsigned int x11KeyCode, unsigned int modifiers = 0) {
            KeySym keySym = XkbKeycodeToKeysym(X11_display, x11KeyCode, 0, 0);

            // a-z
            if (keySym >= XK_a && keySym <= XK_z) {
                return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::KeyA) + (keySym - XK_a));
            }

            // A-Z
            if (keySym >= XK_A && keySym <= XK_Z) {
                return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::KeyA) + (keySym - XK_A));
            }

            // 1-9
            if (keySym >= XK_0 && keySym <= XK_9) {
                return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::Num0) + (keySym - XK_0));
            }

            // Numpad 1-9
            if (keySym >= XK_KP_0 && keySym <= XK_KP_9) {
                return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::Numpad0) + (keySym - XK_KP_0));
            }


            return KeyCode::UnsupportedKey;
        }


#endif
    };
}

#endif //VULKANSURFER_H
