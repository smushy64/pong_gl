#pragma once
#include <windows.h>
#include "globals.hpp"
#include "./core/app.hpp"

class Window {

public:
    Window(HINSTANCE hInst);
    void ProcessMessages( PlayerInput& );
    inline bool Success() { return m_success; }

    bool CreateGLContext();
    void GLSwapBuffers();

    ~Window();
private:
    bool      m_success = true;

    HWND      m_hWnd;
    HDC       m_hdc;
    HINSTANCE m_gl_lib;
    HGLRC     m_hglrc;
};

struct ReadResult {
    u32   size;
    void* contents;
};

f64 ElapsedTime();
ReadResult ReadEntireFile(const char* filename);
void  FreeFileMemory(void* fileMemory);
