#ifndef VULKANSURFER_H
#define VULKANSURFER_H

#include <memory>
#include <string>
#include "vulkan/vulkan.h"
#if defined(SURFER_PLATFORM_WIN32)
#include <windows.h>
#endif
#if defined(SURFER_PLATFORM_X11)
#include <X11/Xlib.h>
#endif


namespace Surfer {

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

        VkSurfaceKHR _surface;
        VkInstance _instance;
        bool _shouldClose = false;
        uint32_t _width, _height;
        int32_t _x, _y;

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
            windowAttributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;

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
                    switch (event.xbutton.button) {
                        case Button1: {
                            break;
                        }
                        case Button2: {
                            break;
                        }
                        case Button3: {
                            break;
                        }
                    }
                }
                case ButtonRelease: {
                    switch (event.xbutton.button) {
                        case Button1: {
                            break;
                        }
                        case Button2: {
                            break;
                        }
                        case Button3: {
                            break;
                        }
                    }
                }
                case MotionNotify: {
                    break;
                }
                case ConfigureNotify:
                {
                    if (event.xconfigure.width != _width || event.xconfigure.height != _height)
                    {

                    }
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
        }

        void X11_onKeyPress(KeyCode keyCode) {

        }

        void X11_onKeyRelease(KeyCode keyCode) {

        }

        void X11_onResize(uint32_t width, uint32_t height) {

        }

        void X11_onMouseMotion(uint32_t x, uint32_t y) {

        }

#endif
    };


}

#endif //VULKANSURFER_H
