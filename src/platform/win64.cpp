#include "win64.hpp"
#include "./core/app.hpp"
#include "opengl.hpp"
#include <functional>
#include "glad/glad.h"

#include <iostream>

u64 GetTicks();

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {

    Window window = Window(hInst);
    if( !window.Success() ) {
        std::cout << "FAILED TO CREATE WINDOW!\n";
        return -1;
    }

    Pong pong = Pong();
    PlayerInput input = {};

    if(!window.CreateGLContext()) {
        std::cout << "FAILED TO CREATE OPENGL CONTEXT!\n";
        return -1;
    }

    OpenGLRenderer renderer = OpenGLRenderer();

    f32 lastElapsedTime = 0.0;

    while(g_RUNNING) {
        window.ProcessMessages(input);

        f32 elapsedTime = window.ElapsedTime();
        DeltaTime deltaTime   = elapsedTime - lastElapsedTime;
        lastElapsedTime = elapsedTime;

        pong.Update(deltaTime, input);

        renderer.ClearScreen(); {
            switch(pong.GetAppState()) {
                case AppState::START: {
                    renderer.RenderMenu(pong.SelectedMenuOption());
                } break;
                case AppState::GAME: {
                    renderer.RenderGame(pong.GetState());
                } break;
            }
        } window.GLSwapBuffers();
    }

    return 0;

}

f64 Window::ElapsedTime() {
    LARGE_INTEGER lpPerformanceCount;
    if(QueryPerformanceCounter(&lpPerformanceCount) == FALSE) {
        g_RUNNING = false;
        return 0.0;
    }
    return double( lpPerformanceCount.QuadPart - m_counterStart )/m_frequency;
}

void Window::GLSwapBuffers() {
    SwapBuffers(m_hdc);
}

void Window::ProcessMessages(PlayerInput& input) {
    MSG message = {};
    if( PeekMessage(
        &message, m_hWnd,
        0, 0,
        PM_REMOVE
    ) == FALSE) { return; }

    switch(message.message) {
        case WM_KEYDOWN: {
            if( message.wParam == VK_DOWN    || message.wParam == 'S' )  { input.down = true; }
            else if( message.wParam == VK_UP || message.wParam == 'W' ) { input.up = true; }
            else if( message.wParam == VK_RETURN || message.wParam == VK_SPACE ) { input.enter = true; }
            else if( message.wParam == VK_ESCAPE ) { g_RUNNING = false; }
        } break;
        case WM_KEYUP: {
            if( message.wParam == VK_DOWN    || message.wParam == 'S' )  { input.down = false; }
            else if( message.wParam == VK_UP || message.wParam == 'W' ) { input.up = false; }
            else if( message.wParam == VK_RETURN || message.wParam == VK_SPACE ) { input.enter = false; }
        } break;
        default: {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }break;
    }
}

LRESULT MainWindowCallback( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam ) {
    switch(Msg) {
        case WM_CLOSE: {
            g_RUNNING = false;
        } return TRUE;

        default: {
        }return DefWindowProc( hWnd, Msg, wParam, lParam );
    }
}

Window::Window(HINSTANCE hInst) {
    WNDCLASSEXW window_class   = {};
    window_class.cbSize        = sizeof(WNDCLASSEXW);
    window_class.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    window_class.hInstance     = hInst;
    window_class.lpfnWndProc   = MainWindowCallback;
    window_class.lpszClassName = L"WindowClass";
    window_class.hCursor       = LoadCursor( nullptr, IDC_ARROW );

    if( RegisterClassEx( &window_class ) == FALSE ) { m_success = false; return; }

    RECT window_rect = {};
    window_rect.left   = 0;
    window_rect.top    = 0;
    window_rect.right  = 1280;
    window_rect.bottom = 720;

    if( AdjustWindowRectEx(
        &window_rect, WS_OVERLAPPEDWINDOW, 0, 0
    ) == FALSE ) { m_success = false; return; }

    m_hWnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        window_class.lpszClassName,
        L"PongGL",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        window_rect.right  - window_rect.left,
        window_rect.bottom - window_rect.top,
        0, 0, hInst, 0
    );

    if(!m_hWnd) { m_success = false; return; }

    if( ShowWindow(m_hWnd, SW_SHOWDEFAULT) == FALSE) { m_success = false; return; }

    LARGE_INTEGER lpFrequency;
    if(QueryPerformanceFrequency(&lpFrequency) == FALSE) { m_success = false; return; }

    m_frequency = double( lpFrequency.QuadPart );

    LARGE_INTEGER lpPerformanceCount;
    if(QueryPerformanceCounter(&lpPerformanceCount) == FALSE) { m_success = false; return; }

    m_counterStart = lpPerformanceCount.QuadPart;
}

typedef HGLRC (*wglCreateContextAttribsARBptr) (HDC, HGLRC, const i32* );

const i32 WGL_CONTEXT_MAJOR_VERSION_ARB             = 0x2091;
const i32 WGL_CONTEXT_MINOR_VERSION_ARB             = 0x2092;
const i32 WGL_CONTEXT_LAYER_PLANE_ARB               = 0x2093;
const i32 WGL_CONTEXT_FLAGS_ARB                     = 0x2094;
const i32 WGL_CONTEXT_PROFILE_MASK_ARB              = 0x9126;
const i32 WGL_CONTEXT_DEBUG_BIT_ARB                 = 0x0001;
const i32 WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB    = 0x0002;
const i32 WGL_CONTEXT_CORE_PROFILE_BIT_ARB          = 0x00000001;
const i32 WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB = 0x00000002;
const i32 WGL_ERROR_INVALID_VERSION_ARB             = 0x2095;
const i32 WGL_ERROR_INVALID_PROFILE_ARB             = 0x2096;

void* LoadGL(const char *name) {
    PROC ptr = wglGetProcAddress( name );
    if(!ptr) {
        HINSTANCE glModule = LoadLibrary(L"opengl32.dll");
        ptr = GetProcAddress( glModule, name );
        FreeLibrary( glModule );
    }
    return (void*)ptr;
}

bool Window::CreateGLContext() {
    m_hdc = GetDC(m_hWnd);
    if( !m_hdc ) { return false; }

    PIXELFORMATDESCRIPTOR desiredPixelFormat = {};
    u16 pixelFormatSize = sizeof( PIXELFORMATDESCRIPTOR );
    desiredPixelFormat.nSize      = pixelFormatSize;
    desiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
    desiredPixelFormat.nVersion   = 1;
    desiredPixelFormat.dwFlags    = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    desiredPixelFormat.cColorBits = 32;
    desiredPixelFormat.cAlphaBits = 8;
    desiredPixelFormat.iLayerType = PFD_MAIN_PLANE;

    i32 pixelFormatIndex = ChoosePixelFormat( m_hdc, &desiredPixelFormat );
    PIXELFORMATDESCRIPTOR suggestedPixelFormat = {};
    DescribePixelFormat(
        m_hdc, pixelFormatIndex,
        pixelFormatSize, &suggestedPixelFormat
    );

    if( SetPixelFormat(m_hdc, pixelFormatIndex, &suggestedPixelFormat) == FALSE ) {
        return false;
    }

    HGLRC old_gl = wglCreateContext(m_hdc);
    if(!old_gl) { return false; }

    if( wglMakeCurrent(m_hdc, old_gl) == FALSE) { return false; }

    wglCreateContextAttribsARBptr wglCreateContextAttribsARB =
        (wglCreateContextAttribsARBptr)wglGetProcAddress("wglCreateContextAttribsARB");

    i32 attribs[] = {
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        0
    };

    m_hglrc = wglCreateContextAttribsARB( m_hdc, nullptr, attribs );

    wglDeleteContext(old_gl);
    wglMakeCurrent(m_hdc, m_hglrc);

    if(!gladLoadGLLoader((GLADloadproc)LoadGL)) { return false; }

    return true;

}

Window::~Window() {
    wglMakeCurrent(nullptr, nullptr);
    if(m_hglrc) {
        wglDeleteContext( m_hglrc );
    }
    ReleaseDC(m_hWnd, m_hdc);
}
