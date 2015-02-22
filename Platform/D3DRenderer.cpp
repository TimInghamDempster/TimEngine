namespace Renderer
{
#include <D3D11.h>

#pragma comment (lib, "d3d11.lib")

	IDXGISwapChain* swapChain;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11RasterizerState* rasterState;

	float clearColour[4];

	void Init(HWND hwnd)
	{
		clearColour[0] = 0.0f;
		clearColour[1] = 0.0f;
		clearColour[2] = 1.0f;
		clearColour[3] = 1.0f;

		DXGI_SWAP_CHAIN_DESC scd;

		// clear out the struct for use
		ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

		// fill the swap chain description struct
		scd.BufferCount = 1;                                    // one back buffer
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
		scd.OutputWindow = hwnd;                                // the window to be used
		scd.SampleDesc.Count = 4;                               // how many multisamples
		scd.Windowed = TRUE;                                    // windowed/full-screen mode

		// create a device, device context and swap chain using the information in the scd struct
		D3D11CreateDeviceAndSwapChain(NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			NULL,
			NULL,
			NULL,
			D3D11_SDK_VERSION,
			&scd,
			&swapChain,
			&device,
			NULL,
			&deviceContext);

		// get the address of the back buffer
		ID3D11Texture2D *pBackBuffer;
		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		// use the back buffer address to create the render target
		device->CreateRenderTargetView(pBackBuffer, NULL, &renderTargetView);
		pBackBuffer->Release();

		// set the render target as the back buffer
		deviceContext->OMSetRenderTargets(1, &renderTargetView, NULL);

		 // Set the viewport
		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = 1280;
		viewport.Height = 720;

		deviceContext->RSSetViewports(1, &viewport);
	}

	void CleanUp()
	{
		swapChain->Release();
		device->Release();
		deviceContext->Release();
		renderTargetView->Release();
	}

	void Draw()
	{

		// clear the back buffer to a deep blue
		deviceContext->ClearRenderTargetView(renderTargetView, clearColour);

		// do 3D rendering on the back buffer here

		// switch the back buffer and the front buffer
		swapChain->Present(0, 0);
	}
}