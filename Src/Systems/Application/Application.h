#pragma once

#include "Systems/System.h"

class Application : public System
{
public:
    Application();
    ~Application();

    void Initialize() override;
    void Update(float deltaTime) override;

    void InitializeWindows(int&, int&);
    void ShutdownWindows();

    LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

    HINSTANCE GetHinstance();
    HWND GetHWND();
    int GetWidth();
    int GetHeight();

    bool isFullscreen;

private:
    LPCWSTR m_applicationName;
    HINSTANCE m_hinstance;
    HWND m_hwnd;
    int m_width;
    int m_height;

};
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static Application* ApplicationHandle = 0;
extern Application* mAPPLICATION;