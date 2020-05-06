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
    using DrawCallback = void(*)(int width, int height, unsigned char* buffer, int frame_count);

    window(const char* title, int width, int height, DrawCallback drawToBuffer);
    ~window();

    void PollEvents() const;
    bool ShouldClose() const;

private:
    static LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);

    void CreateBuffer(const int width, const int height);
    void CopyBufferToWindow(HDC deviceContext) const;

    void window::RenderGradient();


    HWND m_windowHandle;
    int m_windowWidth;
    int m_windowHeight;
    bool m_windowShouldClose;

    int m_windowClientWidth;
    int m_windowClientHeight;

    BITMAPINFO m_bitmapInfo;
    unsigned char* m_bitmapMemory;
    int m_bitmapWidth;
    int m_bitmapHeight;

    DrawCallback m_drawToBuffer;
};


window::window(const char* title, int width, int height, DrawCallback drawToBuffer)
    : m_windowShouldClose(false)
    , m_windowClientWidth(width)
    , m_windowClientHeight(height)
    , m_drawToBuffer(drawToBuffer)
{
    HINSTANCE hInst = GetModuleHandleA(NULL);
    assert(hInst != NULL);

    WNDCLASS winClass = {};
    winClass.hInstance = hInst;
    winClass.lpszClassName = "WindowClass";
    winClass.lpfnWndProc = window::WindowProc;
    winClass.style = CS_HREDRAW | CS_VREDRAW;
    //winClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    //winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    auto error = RegisterClassA(&winClass);
    assert(error != 0);

    RECT rect = { 0, 0, width, height };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    m_windowWidth = rect.right - rect.left;
    m_windowHeight = rect.bottom - rect.top;

    m_windowHandle = CreateWindowExA(0, "WindowClass", title,
                                     WS_VISIBLE | WS_OVERLAPPEDWINDOW,
                                     CW_USEDEFAULT, CW_USEDEFAULT,
                                     m_windowWidth, m_windowHeight,
                                     NULL, NULL,
                                     hInst, this);
    assert(m_windowHandle != NULL);
    
    CreateBuffer(width, height);
}

window::~window()
{
    m_windowShouldClose = true;
    if (m_windowHandle)
        DeleteObject(m_windowHandle);
    if (m_bitmapMemory)
        VirtualFree(m_bitmapMemory, 0, MEM_RELEASE);
}


bool window::ShouldClose() const
{
    return m_windowShouldClose;
}

void window::PollEvents() const
{
    MSG message;
    while(PeekMessageA(&message, m_windowHandle, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    InvalidateRect(m_windowHandle, NULL, FALSE);
    UpdateWindow(m_windowHandle);

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
    case WM_SIZE:
        {
            RECT rect;
            GetWindowRect(m_windowHandle, &rect);
            m_windowWidth = rect.right - rect.left;
            m_windowHeight = rect.bottom - rect.top;
            GetClientRect(m_windowHandle, &rect);
            m_windowClientWidth = rect.right - rect.left;
            m_windowClientHeight = rect.bottom - rect.top;

            OutputDebugStringA("WM_SIZE\n");
        }
        break;

    case WM_PAINT:
        {
            static int frame_count = 0;

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(m_windowHandle, &ps);

            //RenderGradient();
            m_drawToBuffer(m_bitmapWidth, m_bitmapHeight, m_bitmapMemory, frame_count++);
            CopyBufferToWindow(hdc);

            static char s_Buffer[200];
            sprintf_s(s_Buffer, sizeof(s_Buffer), "frames %i\n", frame_count);
            RECT textRect;
            textRect.left = 5;
            textRect.top = 5;
            textRect.right = 500;
            textRect.bottom = 30;
            //SetTextColor(hdc, 0x00000080);
            SetBkMode(hdc, TRANSPARENT);
            DrawTextA(hdc, s_Buffer, (int)strlen(s_Buffer), &textRect, DT_NOCLIP | DT_LEFT | DT_TOP);

            EndPaint(m_windowHandle, &ps);

            OutputDebugStringA("WM_PAINT\n");
        }
        break;

    case WM_CLOSE:
        {
            OutputDebugStringA("WM_CLOSE\n");

            m_windowShouldClose = true;
            PostQuitMessage(0);
        }
        break;

    default:
        {
            result = DefWindowProcA(m_windowHandle, message, wParam, lParam);
        }
        break;
    }

    return result;
}


void window::CreateBuffer(const int width, const int height)
{
    m_bitmapWidth = width;
    m_bitmapHeight = height;

    m_bitmapInfo.bmiHeader.biSize = sizeof(m_bitmapInfo.bmiHeader);
    m_bitmapInfo.bmiHeader.biWidth = width;
    m_bitmapInfo.bmiHeader.biHeight = height;  // top-down, meaning the first byte of the image is top-left pixel
    m_bitmapInfo.bmiHeader.biPlanes = 1;
    m_bitmapInfo.bmiHeader.biBitCount = 32;
    m_bitmapInfo.bmiHeader.biCompression = BI_RGB;
    m_bitmapInfo.bmiHeader.biSizeImage = 0;
    m_bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
    m_bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
    m_bitmapInfo.bmiHeader.biClrUsed = 0;
    m_bitmapInfo.bmiHeader.biClrImportant = 0;

    const int BytesPerPixel = 4;
    const int BitmapMemorySize = width * height * BytesPerPixel;
    m_bitmapMemory = (unsigned char*)VirtualAlloc(NULL, BitmapMemorySize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

void window::CopyBufferToWindow(HDC deviceContext) const
{
    StretchDIBits(deviceContext,
                  0, 0, m_windowClientWidth, m_windowClientHeight,
                  0, 0, m_bitmapWidth, m_bitmapHeight,
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
