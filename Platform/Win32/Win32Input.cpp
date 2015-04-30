namespace Platform
{
	Engine::Point2d CursorPosition;

	void UpdateCursorPosition(HWND hwnd)
	{
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(hwnd, &p);
		CursorPosition.x = p.x;
		CursorPosition.y = p.y;
	}
}