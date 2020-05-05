#pragma once

#include <cassert>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace rt
{

class window
{
public:
    window(const char* title, int width, int height);
    ~window();

    void PollEvents();
    bool ShouldClose();

private:
    static LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);

    void WindowResize(int width, int height);
    void WindowUpdate(HDC deviceContext, RECT* windowRect, int x, int y, int width, int height);

    void window::RenderGradient();

    HWND m_windowHandle;
    int m_windowWidth;
    int m_windowHeight;
    bool m_windowShouldClose;

    BITMAPINFO m_bitmapInfo;
    void* m_bitmapMemory = NULL;
    int m_bitmapWidth;
    int m_bitmapHeight;
};


window::window(const char* title, int width, int height)
    : m_windowShouldClose(false)
    , m_windowWidth(width)
    , m_windowHeight(height)
{
    HINSTANCE hInst = GetModuleHandleA(NULL);
    assert(hInst != NULL);

    //GetClassInfo(hInst, "Window", &winClass);

    WNDCLASS winClass = {};
    winClass.hInstance = hInst;
    winClass.lpszClassName = "WindowClass";
    winClass.lpfnWndProc = window::WindowProc;
    winClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    //winClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    //winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    auto error = RegisterClassA(&winClass);
    assert(error != 0);

    m_windowHandle = CreateWindowExA(0,
                                     "WindowClass",
                                     title,
                                     WS_VISIBLE | WS_OVERLAPPEDWINDOW,
                                     CW_USEDEFAULT, CW_USEDEFAULT,
                                     m_windowWidth, m_windowHeight,
                                     NULL, NULL,
                                     hInst,
                                     this);
    assert(m_windowHandle != NULL);


    m_bitmapInfo.bmiHeader.biSize = sizeof(m_bitmapInfo.bmiHeader);
    m_bitmapInfo.bmiHeader.biPlanes = 1;
    m_bitmapInfo.bmiHeader.biBitCount = 32;
    m_bitmapInfo.bmiHeader.biCompression = BI_RGB;
    m_bitmapInfo.bmiHeader.biSizeImage = 0;
    m_bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
    m_bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
    m_bitmapInfo.bmiHeader.biClrUsed = 0;
    m_bitmapInfo.bmiHeader.biClrImportant = 0;
}

window::~window()
{
    m_windowShouldClose = true;
    if (m_windowHandle)
        DeleteObject(m_windowHandle);
}


bool window::ShouldClose()
{
    return m_windowShouldClose;
}

void window::PollEvents()
{
    MSG message;
    //if (GetMessageA(&message, m_windowHandle, 0, 0) > 0) {
    while(PeekMessageA(&message, m_windowHandle, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    RenderGradient();
    InvalidateRect(m_windowHandle, NULL, FALSE);
    /*HDC hdc = GetDC(m_windowHandle);
    RECT clientRect;
    GetClientRect(m_windowHandle, &clientRect);
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;
    WindowUpdate(hdc, &clientRect, 0, 0, width, height);*/
}


LRESULT CALLBACK window::WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
    window* winptr = NULL;

    if (message == WM_NCCREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        winptr = (window*)pCreate->lpCreateParams;
        SetWindowLongPtrA(windowHandle, GWLP_USERDATA, (LONG_PTR)winptr);

        winptr->m_windowHandle = windowHandle;
    }
    else {
        winptr = (window*)GetWindowLongPtrA(windowHandle, GWLP_USERDATA);
    }

    if (winptr)
        return winptr->HandleMessage(message, wParam, lParam);

    return DefWindowProcA(windowHandle, message, wParam, lParam);
}

LRESULT window::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch (message) {
    case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        }
        break;

    case WM_SIZE:
        {
            RECT clientRect;
            GetClientRect(m_windowHandle, &clientRect);
            int width = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;
            WindowResize(width, height);

            OutputDebugStringA("WM_SIZE\n");
        }
        break;

    case WM_PAINT:
        {
            OutputDebugStringA("WM_PAINT\n");

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(m_windowHandle, &ps);

            int x = ps.rcPaint.left;
            int y = ps.rcPaint.top;
            int width = ps.rcPaint.right - x;
            int height = ps.rcPaint.bottom - y;

            RECT clientRect;
            GetClientRect(m_windowHandle, &clientRect);
            WindowUpdate(hdc, &clientRect, x, y, width, height);

            EndPaint(m_windowHandle, &ps);
        }
        break;

    case WM_CLOSE:
        {
            OutputDebugStringA("WM_CLOSE\n");

            m_windowShouldClose = true;
            PostQuitMessage(0);
        }
        break;

        /*case WM_DESTROY:
            {
                OutputDebugStringA("WM_DESTROY\n");
                PostQuitMessage(0);
                m_windowShouldClose = true;
            }
            break;*/

    default:
        {
            result = DefWindowProcA(m_windowHandle, message, wParam, lParam);
        }
        break;
    }

    return result;
}


void window::WindowResize(int width, int height)
{
    if (m_bitmapMemory)
        VirtualFree(m_bitmapMemory, 0, MEM_RELEASE);

    m_bitmapWidth = width;
    m_bitmapHeight = height;

    m_bitmapInfo.bmiHeader.biWidth = width;
    m_bitmapInfo.bmiHeader.biHeight = height;

    const int BytesPerPixel = 4;
    const int BitmapMemorySize = width * height * BytesPerPixel;
    m_bitmapMemory = VirtualAlloc(NULL, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    //RenderGradient();
}

void window::WindowUpdate(HDC deviceContext, RECT* windowRect, int x, int y, int width, int height)
{
    int windowWidth = windowRect->right - windowRect->left;
    int windowHeight = windowRect->bottom - windowRect->top;
    StretchDIBits(deviceContext,
                  /*x, y, width, height,
                  x, y, width, height,*/
                  0, 0, m_bitmapWidth, m_bitmapHeight,
                  0, 0, windowWidth, windowHeight,
                  m_bitmapMemory,
                  &m_bitmapInfo,
                  DIB_RGB_COLORS, SRCCOPY);
}

void window::RenderGradient()
{
    static int xOffset = 0;
    ++xOffset;

    const int width = m_bitmapWidth;
    const int height = m_bitmapHeight;

    const int BytesPerPixel = 4;
    const int Pitch = width * BytesPerPixel;

    auto row = (unsigned char*)m_bitmapMemory;
    for (int Y = 0; Y < m_bitmapHeight; ++Y) {
        auto pixel = (unsigned int*)row;

        const unsigned char green = Y;

        for (int X = 0; X < m_bitmapWidth; ++X) {
            unsigned char blue = X + xOffset;

            *pixel = ((green << 8) | blue);
            ++pixel;
        }

        row += Pitch;
    }
}

} // namespace rt
