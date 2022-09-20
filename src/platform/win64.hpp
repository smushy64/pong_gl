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

    f64 ElapsedTime();

    ~Window();
private:
    bool      m_success = true;

    HWND      m_hWnd;
    HDC       m_hdc;
    HINSTANCE m_gl_lib;
    HGLRC     m_hglrc;

    f64   m_frequency;
    i64   m_counterStart;
};
