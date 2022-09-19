#pragma once
#include <windows.h>
#include "globals.hpp"
#include "./core/app.hpp"

LRESULT MainWindowCallback( HWND, UINT, WPARAM, LPARAM );

class Window {

public:
    Window(HINSTANCE hInst);
    void ProcessMessages( PlayerInput& );
    inline bool Success() { return m_success; }

    bool CreateGLContext();
    void GLSwapBuffers();

    ~Window();

private:
    HWND m_hWnd;
    HDC  m_hdc;
    bool m_success = true;
    HINSTANCE m_gl_lib;
    HGLRC m_hglrc;
};
