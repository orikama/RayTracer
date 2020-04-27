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

    bool pollEvents();
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:

    HWND m_windowHandle;

};


window::window(const char* title, int width, int height)
{
    HINSTANCE hInst = GetModuleHandle(NULL);
    assert(hInst != NULL);

    //GetClassInfo(hInst, "Window", &winClass);

    WNDCLASS winClass = { 0 };
    winClass.hInstance = hInst;
    winClass.lpszClassName = "Window";
    winClass.lpfnWndProc = window::WindowProc;
    winClass.hbrBackground = HBRUSH(COLOR_BTNFACE + 1);
    winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    winClass.style = CS_HREDRAW | CS_VREDRAW;


    if (!RegisterClass(&winClass))
        MessageBox(NULL, "Could not register class", "Error", MB_OK);

    HWND m_windowHandle = CreateWindow("Window",
                                       title,
                                       WS_VISIBLE | WS_OVERLAPPEDWINDOW,
                                       CW_USEDEFAULT, CW_USEDEFAULT,
                                       width, height,
                                       NULL, NULL,
                                       hInst,
                                       this);

    assert(m_windowHandle != NULL);

    //ShowWindow(m_windowHandle, SW_RESTORE);
}

window::~window()
{
    if (m_windowHandle)
        DeleteObject(m_windowHandle);
}


bool window::pollEvents()
{
    MSG messages;
    // NOTE: why NULL and not m_windowHandle
    if (GetMessage(&messages, NULL, 0, 0)) {
        TranslateMessage(&messages);
        DispatchMessage(&messages);

        return true;
    }

    return false;
}


LRESULT CALLBACK window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    window* winptr = NULL;

    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        winptr = (window*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)winptr);

        winptr->m_windowHandle = hwnd;
    }
    else {
        winptr = (window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (winptr)
        return winptr->HandleMessage(uMsg, wParam, lParam);

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        //case WM_CHAR: //this is just for a program exit besides window's borders/task bar
        //    if (uMsg == VK_ESCAPE) {
        //        DestroyWindow(hwnd);
        //    }
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(m_windowHandle, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            EndPaint(m_windowHandle, &ps);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(m_windowHandle, uMsg, wParam, lParam);
    }

    return 0;
}

} // namespace rt
