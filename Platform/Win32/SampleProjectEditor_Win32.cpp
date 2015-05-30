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
#include "../../Engine/UI.cpp"
#include "../../Engine/Vector.cpp"
#include "../../Engine/World.cpp"
#include "../../Editor/Editor.cpp"

#include "WindProc.cpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
	HWND hwnd = TimEngineCreateWindow(hInstance);

	if(hwnd == NULL)
	{
		return 0;
	}

	Renderer::Init(hwnd);

	Editor::Init();

	return IndirectedWinMain(hInstance, hPrevInstance,
		lpCmdLine, nCmdShow, hwnd);
}