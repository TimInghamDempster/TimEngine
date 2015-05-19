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
		case WM_LBUTTONDOWN:
			UI::g_leftButtonClicked = true;
		case WM_RBUTTONDOWN:
			UI::g_rightButtonClicked = true;
			break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

HWND TimEngineCreateWindow(HINSTANCE hInstance)
{
    WNDCLASSEX wc;
    HWND hwnd;

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

	RECT  wrect;
	DWORD style = WS_BORDER;

	 // Define desired client size.
	wrect.left   = 0;
	wrect.top    = 0;
	wrect.right  = 1280;
	wrect.bottom = 720;
  
	AdjustWindowRect(&wrect,style,false);

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_className,
        L"TimEngine Sample Project",
        style,
		CW_USEDEFAULT, CW_USEDEFAULT, wrect.right, wrect.bottom,
        NULL, NULL, hInstance, NULL);

	Engine::ScreenRect.Top = 0;
	Engine::ScreenRect.Left = 0;
	Engine::ScreenRect.Height = 720;
	Engine::ScreenRect.Width = 1280;

    if(hwnd == NULL)
    {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
    }
	return hwnd;
}

int WINAPI IndirectedWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow, HWND hwnd)
{
    MSG Msg;

	g_running = true;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

	while (g_running)
	{
		UI::g_leftButtonClicked = false;
		UI::g_rightButtonClicked = false;

		while(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}

		Platform::UpdateCursorPosition(hwnd);
		UI::Tick();
		Renderer::Draw();
	}

    return 0;
}