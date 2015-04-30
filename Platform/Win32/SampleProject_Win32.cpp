
#include <windows.h>
#include <string>
#include <fstream>
#include <vector>

#include <D3D11.h>
#include <d2d1.h>
#include <dwrite.h>

#include "../../Engine/Handle.cpp"
#include "TypesWin32.cpp"
#include "../../Engine/Rect.cpp"
#include "../../Engine/Config.cpp"
#include "StringUtilVCpp.cpp"
#include "../../Engine/TypesCrossPlatform.cpp"
#include "Win32Input.cpp"
#include "../../Engine/Logging.cpp"
#include "Win32IO.cpp"
#include "../../Engine/FileHelpers.cpp"
#include "Mesh.cpp"
#include "D3DRenderer.cpp"
#include "../../Engine/GameWorld.cpp"

const wchar_t g_className[] = L"myWindowClass";
bool g_running;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CLOSE:
            DestroyWindow(hwnd);
			g_running = false;
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

	g_running = true;

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_className;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, L"Window Registration Failed!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_className,
        L"TimEngine Sample Project",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
        NULL, NULL, hInstance, NULL);

	Engine::ScreenRect.Top = 0;
	Engine::ScreenRect.Left = 0;
	Engine::ScreenRect.Height = 720;
	Engine::ScreenRect.Width = 1280;

    if(hwnd == NULL)
    {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

	Renderer::Init(hwnd);

	World::LoadLevel(Platform::WideStringToUtf16(L"Start.lvl"));

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

	while (g_running)
	{
		while(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		Platform::UpdateCursorPosition(hwnd);
		UI::Tick();
		World::Tick();
		Renderer::Draw();
	}

	Renderer::CleanUp();

    return 0;
}