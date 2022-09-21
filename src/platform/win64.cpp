#ifdef WINDOWS

// ignore compiler warning
// casting function pointers from GetProcAddress/wglGetProcAddress is the intended usage
#pragma GCC diagnostic ignored "-Wcast-function-type"
#include <windows.h>
#include "platform.hpp"
#include "globals.hpp"
#include "./core/app.hpp"
#include "./core/platform.hpp"
#include "./core/font.hpp"
#include "renderer.hpp"

#include <iostream>

const char* FONT_PATH = "./resources/HyperspaceBold.otf";

#ifdef OPENGL
HGLRC CreateGLContext();
void* LoadGL(const char *name);
#endif

bool InitWindow(HINSTANCE hInst);
FileReadResult ReadEntireFile(const char* filename);
void ProcessMessages(PlayerInput& input);
void FreeFileMemory(void* fileMemory);
f64 ElapsedTime();

HWND g_hWnd;
HDC  g_hdc;
f64 g_perfFrequency;
u64 g_perfCounterStart;

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE, PSTR, int) {
    if(!InitWindow(hInst)) {
        ErrorBox("Failed to create win64 Window!");
        return -1;
    }

#ifdef OPENGL
    HGLRC hglrc = CreateGLContext();
    if(!hglrc) {
        ErrorBox("Failed to create OpenGL context!");
        return -1;
    }
    if(!InitializeGL(LoadGL)) {
        ErrorBox("Failed to initialize OpenGL!");
        return -1;
    }
#endif

    Pong pong = Pong();
    PlayerInput input = {};

    if( !InitializeRenderer() ) {
        ErrorBox("Failed to initialize renderer!");
        return -1;
    }

    FileReadResult fontFile = ReadEntireFile(FONT_PATH);
    if(fontFile.contents) {
        Font font = LoadFontFromBytes( (u8*)fontFile.contents );
        FreeFileMemory(fontFile.contents);
        // load into renderer
        RendererLoadFont(font);
        FreeFont(font);
    } else {
        ErrorBox("Failed to load font!\nPerhaps the resources folder is not in the same directory as this program?");
        return -1;
    }

    f32 lastElapsedTime = 0.0;
    while(g_RUNNING) {
        ProcessMessages(input);
        f32 elapsedTime     = ElapsedTime();
        DeltaTime deltaTime = elapsedTime - lastElapsedTime;
        lastElapsedTime     = elapsedTime;

        switch(pong.CurrentScene()) {
            case Scene::MAIN_MENU: {
                pong.UpdateMenu(input);
            } break;
            case Scene::IN_GAME: {
                pong.UpdateGame(deltaTime, input);
            } break;
        }

        ClearScreen();
        switch(pong.CurrentScene()) {
            case Scene::MAIN_MENU: {
                RenderMenu(pong.GetSelectedMenuOption());
            } break;
            case Scene::IN_GAME: {
                RenderGame(pong.GetGameState());
            } break;
        }

#ifdef OPENGL
    SwapBuffers(g_hdc);
#endif
    }

#ifdef OPENGL
    wglMakeCurrent(nullptr, nullptr);
    if(hglrc) {
        wglDeleteContext( hglrc );
    }
#endif
    ReleaseDC(g_hWnd, g_hdc);
    return 0;
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

bool InitWindow(HINSTANCE hInst) {
    WNDCLASSEXW windowClass   = {};
    windowClass.cbSize        = sizeof(WNDCLASSEXW);
    windowClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    windowClass.hInstance     = hInst;
    windowClass.lpfnWndProc   = MainWindowCallback;
    windowClass.lpszClassName = L"WindowClass";
    windowClass.hCursor       = LoadCursor( nullptr, IDC_ARROW );

    if( RegisterClassEx(&windowClass) == FALSE ) { return false; }

    RECT windowRect   = {};
    windowRect.left   = 0;
    windowRect.top    = 0;
    windowRect.right  = 1280;
    windowRect.bottom = 720;

    if( AdjustWindowRectEx(
        &windowRect, WS_OVERLAPPEDWINDOW, 0, 0
    ) == FALSE ) { return false; }

    g_hWnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        windowClass.lpszClassName,
        L"PongGL",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right  - windowRect.left,
        windowRect.bottom - windowRect.top,
        0, 0, hInst, 0
    );

    if(!g_hWnd) { return false; }
    if( ShowWindow(g_hWnd, SW_SHOWDEFAULT) == FALSE) { return false; }

    LARGE_INTEGER lpFrequency;
    if(QueryPerformanceFrequency(&lpFrequency) == FALSE) { return false; }

    g_perfFrequency = f64( lpFrequency.QuadPart );

    LARGE_INTEGER lpPerformanceCount;
    if(QueryPerformanceCounter(&lpPerformanceCount) == FALSE) { return false; }

    g_perfCounterStart = lpPerformanceCount.QuadPart;

    return true;
}

void ProcessMessages(PlayerInput& input) {
    MSG message = {};
    if( PeekMessage(
        &message, g_hWnd,
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

#ifdef OPENGL

typedef HGLRC (*wglCreateContextAttribsARBptr) (HDC, HGLRC, const i32* );

const i32 WGL_CONTEXT_MAJOR_VERSION_ARB             = 0x2091;
const i32 WGL_CONTEXT_MINOR_VERSION_ARB             = 0x2092;
const i32 WGL_CONTEXT_FLAGS_ARB                     = 0x2094;
const i32 WGL_CONTEXT_PROFILE_MASK_ARB              = 0x9126;
const i32 WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB    = 0x0002;
const i32 WGL_CONTEXT_CORE_PROFILE_BIT_ARB          = 0x00000001;

void* LoadGL(const char *name) {
    PROC ptr = wglGetProcAddress( name );
    if(!ptr) {
        HINSTANCE glModule = LoadLibrary(L"opengl32.dll");
        ptr = GetProcAddress( glModule, name );
        FreeLibrary( glModule );
    }
    return (void*)ptr;
}

HGLRC CreateGLContext() {
    g_hdc = GetDC(g_hWnd);
    if( !g_hdc ) { return nullptr; }

    PIXELFORMATDESCRIPTOR desiredPixelFormat = {};
    u16 pixelFormatSize = sizeof( PIXELFORMATDESCRIPTOR );
    desiredPixelFormat.nSize      = pixelFormatSize;
    desiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
    desiredPixelFormat.nVersion   = 1;
    desiredPixelFormat.dwFlags    = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    desiredPixelFormat.cColorBits = 32;
    desiredPixelFormat.cAlphaBits = 8;
    desiredPixelFormat.iLayerType = PFD_MAIN_PLANE;

    i32 pixelFormatIndex = ChoosePixelFormat( g_hdc, &desiredPixelFormat );
    PIXELFORMATDESCRIPTOR suggestedPixelFormat = {};
    DescribePixelFormat(
        g_hdc, pixelFormatIndex,
        pixelFormatSize, &suggestedPixelFormat
    );

    if( SetPixelFormat(g_hdc, pixelFormatIndex, &suggestedPixelFormat) == FALSE ) {
        return nullptr;
    }

    HGLRC old_gl = wglCreateContext(g_hdc);
    if(!old_gl) { return nullptr; }

    if( wglMakeCurrent(g_hdc, old_gl) == FALSE) { return nullptr; }

    wglCreateContextAttribsARBptr wglCreateContextAttribsARB =
        (wglCreateContextAttribsARBptr)wglGetProcAddress("wglCreateContextAttribsARB");

    i32 attribs[] = {
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        0
    };

    HGLRC hglrc = wglCreateContextAttribsARB( g_hdc, nullptr, attribs );

    wglDeleteContext(old_gl);
    wglMakeCurrent(g_hdc, hglrc);

    return hglrc;

}

#endif

FileReadResult ReadEntireFile(const char* filename) {

    FileReadResult result = {};
    result.contents = nullptr;
    result.size     = 0;

    HANDLE fileHandle = CreateFileA(
        filename,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0, 0
    );

    if(fileHandle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER fileSize;
        if( GetFileSizeEx( fileHandle, &fileSize ) == TRUE ) {
            result.size = (u32)fileSize.QuadPart;
            result.contents = VirtualAlloc( 0, result.size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );

            if(result.contents) {
                DWORD bytesRead;
                if( !(ReadFile(
                    fileHandle, result.contents,
                    result.size,
                    &bytesRead, NULL
                ) && bytesRead == result.size )) {
                    FreeFileMemory(result.contents);
                    result.contents = nullptr;
                }
            }
        }

        CloseHandle( fileHandle );
    }

    return result;
}

void ErrorBox(std::string errorMessage) {
    std::wstring stemp = std::wstring(errorMessage.begin(), errorMessage.end());
    LPCWSTR sw = stemp.c_str();
    MessageBox(
        nullptr,
        sw,
        L"Fatal Error",
        MB_OK
    );
}

void FreeFileMemory(void* fileMemory) { VirtualFree( fileMemory, 0, MEM_RELEASE ); }

f64 ElapsedTime() {
    LARGE_INTEGER lpPerformanceCount;
    if(QueryPerformanceCounter(&lpPerformanceCount) == FALSE) {
        g_RUNNING = false;
        return 0.0;
    }
    return double( lpPerformanceCount.QuadPart - g_perfCounterStart )/g_perfFrequency;
}

#endif
