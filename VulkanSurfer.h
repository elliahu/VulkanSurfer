#ifndef VULKANSURFER_H
#define VULKANSURFER_H

#include <functional>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#if defined(SURFER_PLATFORM_WIN32)
#include <windows.h>
#endif
#if defined(SURFER_PLATFORM_X11)
#include <X11/Xlib.h>
#endif


namespace Surfer {

    enum class KeyCode {
        // Alphabet
        KeyA, KeyB, KeyC, KeyD, KeyE, KeyF, KeyG, KeyH, KeyI, KeyJ, KeyK, KeyL, KeyM, KeyN, KeyO, KeyP, KeyQ, KeyR, KeyS, KeyT, KeyU, KeyV, KeyW, KeyX, KeyY, KeyZ,

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
        Unknown
    };

    typedef std::function<void(KeyCode key)> KeyPressCallback;
    typedef std::function<void(KeyCode key)> KeyReleaseCallback;
    typedef std::function<void(uint32_t x, uint32_t y)> MouseMotionCallback;
    typedef std::function<void(uint32_t width, uint32_t height)> ResizeCallback;
    typedef std::function<void(int32_t x, int32_t y)> MoveCallback;
    typedef std::function<void()> CloseCallback;
    typedef std::function <void(bool entered)> MouseEnterExitCallback;
    typedef std::function <void(bool focused)> FocusCallback;

    /**
     * Represents a single window
     */
    class Window {
    public:
        // We delete all constructors except one that is private
        Window() = delete;                           // No default constructor
        Window(const Window&) = delete;                // No copy constructor
        Window(Window&&) = delete;                     // No move constructor
        Window& operator=(const Window&) = delete;     // No copy assignment
        Window& operator=(Window&&) = delete;          // No move assignment

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
        static Window * createWindow(const std::string& title, const VkInstance instance, const uint32_t width, const uint32_t height, const int32_t x, const int32_t y) {
            return new Window(title, instance, width, height, x, y);
        }

        /**
         * Destroy a window
         * @param window Window pointer
         */
        static void destroyWindow(const Window * window) {
            delete window;
        }

        /**
         * Poll for events
         */
        void pollEvents() {
#if defined(SURFER_PLATFORM_X11)
            X11_pollEvents();
#endif
        }

        /**
         * Checks if window should be closed (e.g. user closed the window)
         * @return True if window should be closed, else returns false
         */
        bool shouldClose() const {return _shouldClose;}

        /**
         * Returns Vulkan surface
         * @return VkSurfaceKHR Vulkan surface
         */
        VkSurfaceKHR getSurface() const {return _surface;}

        void getCursorPosition(unsigned int& x, unsigned int& y) const {x = _mouse_x; y = _mouse_y;}

        /**
         * Registers a callback that is triggered when a key is pressed. This can be a keyboard key or mouse key or any other.
         * @param callback KeyPressCallback function
         */
        void registerKeyPressCallback(const KeyPressCallback &callback) {this->_keyPressCallback = callback;}

        /**
         * Registers a callback that is triggered when a key is released. This can be a keyboard key or mouse key or any other.
         * @param callback KeyReleaseCallback function
         */
        void registerKeyReleaseCallback(const KeyReleaseCallback &callback) {this->_keyReleaseCallback = callback;}

        /**
         * Registers a callback that is triggered when a mouse is moved (cursor position has changed)
         * @param callback MouseMotionCallback function
         */
        void registerMouseMotionCallback(const MouseMotionCallback &callback) {this->_mouseMotionCallback = callback;}

        /**
         * Registers a callback that is triggered when the window is resized
         * @param callback ResizeCallback function
         */
        void registerResizeCallback(const ResizeCallback &callback) {this->_resizeCallback = callback;}

        /**
         * Registers a callback that is triggered when the window is moved on the screen.
         * @param callback MoveCallback function
         */
        void registerMoveCallback(const MoveCallback &callback) {this->_moveCallback = callback;}

        /**
         * Registers a callback that is triggered when the window is closed.
         * @param callback CloseCallback function
         */
        void registerCloseCallback(const CloseCallback &callback) {this->_closeCallback = callback;}

        /**
         * Registers a callback that is triggered when a mouse enter (true) or exits (false) the window
         * @param callback MouseEnterExitCallback function
         */
        void registerMouseEnterExitCallback(const MouseEnterExitCallback &callback) {this->_mouseEnterExitCallback = callback;}

        /**
         * Registers a callback that is triggered when the window becomes in focus (true) or loses focus (false)
         * @param callback FocusCallback function
         */
        void registerFocusCallback(const FocusCallback &callback) {this->_focusCallback = callback;}

    private:

        Window(const std::string& title,const VkInstance instance, const uint32_t width, const uint32_t height, const int32_t x, const int32_t y) {
#if defined(SURFER_PLATFORM_X11)
            X11_createWindow(title, instance, width, height, x, y);
#endif
        }

        ~Window() {
#if defined(SURFER_PLATFORM_X11)
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

#elif defined(SURFER_PLATFORM_X11)
        Display *X11_display;
        ::Window X11_window;
        ::Window X11_root;
        Atom X11_wmDeleteMessage;

        void X11_createWindow(const std::string& title,const VkInstance instance, const uint32_t width, const uint32_t height, const int32_t x, const int32_t y) {

            _width = width;
            _height = height;
            _x = x;
            _y = y;

            X11_display = XOpenDisplay(nullptr);
            if (!X11_display)
            {
                throw std::runtime_error("Failed to open X display");
            }

            X11_root = DefaultRootWindow(X11_display);
            XSetWindowAttributes windowAttributes;
            windowAttributes.background_pixel = WhitePixel(X11_display, 0);
            windowAttributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask | FocusChangeMask | EnterWindowMask;

            X11_window = XCreateWindow(X11_display, X11_root, x, y, width, height, 0, CopyFromParent, InputOutput, CopyFromParent, CWBackPixel | CWEventMask, &windowAttributes);

            XStoreName(X11_display, X11_window, title.c_str());
            XMapWindow(X11_display, X11_window);
            XFlush(X11_display);

            X11_wmDeleteMessage = XInternAtom(X11_display, "WM_DELETE_WINDOW", False);
            XSetWMProtocols(X11_display, X11_window, &X11_wmDeleteMessage, 1);

            VkXlibSurfaceCreateInfoKHR surfaceInfo = {};
            surfaceInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
            surfaceInfo.dpy = X11_display;
            surfaceInfo.window = X11_window;

            VkResult result = vkCreateXlibSurfaceKHR(instance, &surfaceInfo, nullptr, &_surface);
            if (result != VK_SUCCESS)
            {
                std::cerr << "Failed to create Vulkan surface\n";
                exit(EXIT_FAILURE);
            }

            this->_instance = instance;
        }

        void X11_destroyWindow() {
            vkDestroySurfaceKHR(_instance, _surface, nullptr);
            _surface = VK_NULL_HANDLE;

            if (X11_window)
            {
                XDestroyWindow(X11_display, X11_window);
                X11_window = 0;
            }

            if (X11_display)
            {
                XCloseDisplay(X11_display);
                X11_display = nullptr;
            }
        }

        void X11_pollEvents() {
            XEvent event;
            while (XPending(X11_display) > 0)
            {
                XNextEvent(X11_display, &event);
                X11_processEvent(event);
            }
        }

        void X11_processEvent(XEvent event) {
            switch (event.type) {
                case ClientMessage:
                {
                    X11_onClose();
                    break;
                }
                case KeyPress:
                {
                    X11_onKeyPress(event.xkey.keycode);
                    break;
                }
                case KeyRelease:
                {
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
                case ConfigureNotify:
                {
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
            while (XPending(X11_display) > 0)
            {
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
                        _keyPressCallback(KeyCode::Unknown);
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
                        _keyReleaseCallback(KeyCode::Unknown);
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
        static KeyCode X11_translateKeyCode(unsigned int x11KeyCode, unsigned int modifiers = 0) {
            // Normalize keycode by subtracting the offset (8 for X11)
            x11KeyCode -= 8;

            // Handle modifier keys
            if (modifiers & ShiftMask) {
                // Adjust for shifted keys, e.g., 'a' -> 'A'
                // Requires XGetKeyboardMapping to differentiate
            }

            // Example mapping for alphabet keys
            if (x11KeyCode >= 38 && x11KeyCode <= 63) { // Assuming QWERTY layout
                return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::KeyA) + (x11KeyCode - 38));
            }

            // Example mapping for numbers (row, not numpad)
            if (x11KeyCode >= 10 && x11KeyCode <= 19) {
                return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::Num0) + (x11KeyCode - 10));
            }

            // Handle other keys (arrows, function keys, etc.)
            switch (x11KeyCode) {
                case 36: return KeyCode::Enter;
                case 65: return KeyCode::Space;
                case 23: return KeyCode::Tab;
                case 9:  return KeyCode::Esc;
                case 111: return KeyCode::ArrowUp;
                case 116: return KeyCode::ArrowDown;
                case 113: return KeyCode::ArrowLeft;
                case 114: return KeyCode::ArrowRight;
                // Add more mappings as needed
                default: return KeyCode::Unknown;
            }
        }


#endif
    };


}

#endif //VULKANSURFER_H
