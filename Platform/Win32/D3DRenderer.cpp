namespace Renderer
{
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d2d1.lib")
#pragma comment(lib, "DXGI.lib")

	void DrawScene();
	void DrawUI();

	IDXGISwapChain* swapChain = nullptr;
	ID3D11Device* mainDevice = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;
	ID3D11Texture2D* depthStencilBuffer = nullptr;
	ID3D11DepthStencilState* depthStencilState = nullptr;
	ID3D11DepthStencilView* depthStencilView = nullptr;
	ID3D11RasterizerState* rasterState = nullptr;

	ID3D11VertexShader* uiVertexShader = nullptr;
	ID3D11InputLayout* uiInputLayout = nullptr;
	ID3D11PixelShader* uiPixelShader = nullptr;
	ID3D11Buffer* unitQuadVertexBuffer = nullptr;
	ID3D11Buffer* unitQuadIndexBuffer = nullptr;
	ID3D11Buffer* ascendingCountBuffer = nullptr;
	ID3D11Buffer* uiTransformConstantBuffer = nullptr;

	ID2D1Factory* pD2DFactory = nullptr;
	ID2D1RenderTarget* d2dRenderTarget = nullptr;

	float clearColour[4];
	uInt32 videoCardMemory = 0;
	uInt32 numUIQuads = 0;

	std::vector<D2D1_RECT_F> uiRects;
	std::vector<ID2D1SolidColorBrush*> uiBrushes;

	bool LoadVertexShaderAndBuildInputLayout(std::u16string filename, ID3D11VertexShader** vertexShader, const D3D11_INPUT_ELEMENT_DESC *inputElementDescs, const int32 numInputElelments, ID3D11InputLayout** inputLayout)
	{
		bool overallSuccess = true;

		int32 length;
		bool fileReadSuccessful;

		char* buffer = Engine::ReadBinaryFile(filename, length, fileReadSuccessful);

		if(fileReadSuccessful)
		{

			HRESULT hr = mainDevice->CreateVertexShader(buffer, length, nullptr, vertexShader);

			if(hr != S_OK)
			{
				Engine::Log(Platform::WideStringToUtf16(L"Error creating vertex shader: ") + filename);
				overallSuccess = false;
			}
		}
		else
		{
			overallSuccess = false;
		}

		if(overallSuccess)
		{
			HRESULT hr = mainDevice->CreateInputLayout(inputElementDescs, numInputElelments, buffer, length, inputLayout);

			if(hr != S_OK)
			{
				Engine::Log(Platform::WideStringToUtf16(L"Error creating input layout for vertex shader: ") + filename);
				overallSuccess = false;
			}
		}

		delete[] buffer;

		return overallSuccess;
	}

	bool LoadPixelShader(std::u16string filename, ID3D11PixelShader** pixelShader)
	{
		bool overallSuccess = true;

		int32 length;
		bool fileReadSuccessful;

		char* buffer = Engine::ReadBinaryFile(filename, length, fileReadSuccessful);

		if(fileReadSuccessful == true)
		{

			HRESULT hr = mainDevice->CreatePixelShader(buffer, length, nullptr, pixelShader);

			if(hr != S_OK)
			{
				Engine::Log(Platform::WideStringToUtf16(L"Error creating pixel shader: ") + filename);
				overallSuccess = false;
			}
		}
		else
		{
			overallSuccess = false;
		}

		delete[] buffer;

		return overallSuccess;
	}

	bool InitD2DDevice()
	{
		HRESULT hr;

		D2D1_FACTORY_OPTIONS options;
		options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

		hr = D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,
			options,
			&pD2DFactory
			);

		if(hr != S_OK)
		{
			Engine::Log(Platform::WideStringToUtf16(L"Failed to create d2d factory"));
			return false;
		}

		float dpiX;
		float dpiY;
		pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

		D2D1_RENDER_TARGET_PROPERTIES props =
			D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			dpiX,
			dpiY
			);

		IDXGISurface* pBackBuffer;

		hr = swapChain->GetBuffer(0,
			IID_PPV_ARGS(&pBackBuffer)
			);

		if(hr != S_OK)
		{
			Engine::Log(Platform::WideStringToUtf16(L"Failed to get back buffer for d2d setup"));
			return false;
		}

		hr = pD2DFactory->CreateDxgiSurfaceRenderTarget(
			pBackBuffer,
			&props,
			&d2dRenderTarget
			);

		if(hr != S_OK)
		{
			Engine::Log(Platform::WideStringToUtf16(L"Failed to create d2d render target"));
			return false;
		}
		pBackBuffer->Release();

		uiBrushes.push_back(nullptr);
		hr = d2dRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Gray, 1.0f),
			&(uiBrushes[0])
			);

		if(hr != S_OK)
		{
			Engine::Log(Platform::WideStringToUtf16(L"Failed to create ui brush"));
			return false;
		}

		return true;
	}

	bool InitD3DDevice(HWND hwnd)
	{
		HRESULT result;
		IDXGIFactory* factory;
		IDXGIAdapter* adapter;
		IDXGIOutput* adapterOutput;
		ID3D11Texture2D* backBufferPtr;
		unsigned int numModes, i, numerator, denominator;
		DXGI_MODE_DESC* displayModeList;
		DXGI_ADAPTER_DESC adapterDesc;
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		D3D_FEATURE_LEVEL featureLevel;
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		D3D11_RASTERIZER_DESC rasterDesc;
		D3D11_VIEWPORT viewport;

		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if(FAILED(result))
		{
			return false;
		}

		result = factory->EnumAdapters(0, &adapter);
		if(FAILED(result))
		{
			return false;
		}

		result = adapter->EnumOutputs(0, &adapterOutput);
		if(FAILED(result))
		{
			return false;
		}

		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
		if(FAILED(result))
		{
			return false;
		}

		displayModeList = new DXGI_MODE_DESC[numModes];
		if(!displayModeList)
		{
			return false;
		}

		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
		if(FAILED(result))
		{
			return false;
		}

		for(i=0; i<numModes; i++)
		{
			if(displayModeList[i].Width == Engine::ScreenRect.Width)
			{
				if(displayModeList[i].Height == Engine::ScreenRect.Height)
				{
					numerator = displayModeList[i].RefreshRate.Numerator;
					denominator = displayModeList[i].RefreshRate.Denominator;
				}
			}
		}

		result = adapter->GetDesc(&adapterDesc);
		if(FAILED(result))
		{
			return false;
		}

		videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

		/*error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
		if(error != 0)
		{
		return false;
		}*/

		delete [] displayModeList;
		displayModeList = 0;

		adapterOutput->Release();
		adapterOutput = 0;

		adapter->Release();
		adapter = 0;

		factory->Release();
		factory = 0;

		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

		swapChainDesc.BufferCount = 1;

		swapChainDesc.BufferDesc.Width = Engine::ScreenRect.Width;
		swapChainDesc.BufferDesc.Height = Engine::ScreenRect.Height;

		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		/*if(m_vsync_enabled)
		{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
		}
		else*/
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		}

		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

		swapChainDesc.OutputWindow = hwnd;

		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		/*if(fullscreen)
		{
		swapChainDesc.Windowed = false;
		}
		else*/
		{
			swapChainDesc.Windowed = true;
		}

		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		swapChainDesc.Flags = 0;

		featureLevel = D3D_FEATURE_LEVEL_11_0;
		

		//result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT, &featureLevel, 1, 
		result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_BGRA_SUPPORT, &featureLevel, 1, 
			D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &mainDevice, NULL, &deviceContext);
		if(FAILED(result))
		{
			return false;
		}

		result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
		if(FAILED(result))
		{
			return false;
		}

		result = mainDevice->CreateRenderTargetView(backBufferPtr, NULL, &renderTargetView);
		if(FAILED(result))
		{
			return false;
		}

		backBufferPtr->Release();
		backBufferPtr = 0;

		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

		depthBufferDesc.Width = Engine::ScreenRect.Width;
		depthBufferDesc.Height = Engine::ScreenRect.Height;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		result = mainDevice->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);
		if(FAILED(result))
		{
			return false;
		}

		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		depthStencilDesc.DepthEnable = false;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = false;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		result = mainDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
		if(FAILED(result))
		{
			return false;
		}

		deviceContext->OMSetDepthStencilState(depthStencilState, 1);

		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = mainDevice->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
		if(FAILED(result))
		{
			return false;
		}

		deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		result = mainDevice->CreateRasterizerState(&rasterDesc, &rasterState);
		if(FAILED(result))
		{
			return false;
		}

		deviceContext->RSSetState(rasterState);

		viewport.Width = (float)Engine::ScreenRect.Width;
		viewport.Height = (float)Engine::ScreenRect.Height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		deviceContext->RSSetViewports(1, &viewport);

		bool success = InitD2DDevice();

		return success;
	}

	void Init(HWND hwnd)
	{
		clearColour[0] = 0.0f;
		clearColour[1] = 0.0f;
		clearColour[2] = 1.0f;
		clearColour[3] = 1.0f;

		InitD3DDevice(hwnd);

		CreateAscendingBuffer(mainDevice, &ascendingCountBuffer, 4096);

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 
			D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32_UINT, 1, 0, 
			D3D11_INPUT_PER_INSTANCE_DATA, 0 },
		};

		LoadVertexShaderAndBuildInputLayout(Platform::WideStringToUtf16(L"Content\\Shaders\\UIvs.cso"), &uiVertexShader, layout, 2, &uiInputLayout);
		LoadPixelShader(Platform::WideStringToUtf16(L"Content\\Shaders\\UIps.cso"), &uiPixelShader);
		CreateQuadMeshBuffers(mainDevice, &unitQuadVertexBuffer, &unitQuadIndexBuffer);
		CreateConstantBuffer(mainDevice, &uiTransformConstantBuffer, 4096*16);
	}

	void ReportLiveObjects()
	{
		ID3D11Debug* debugDevice;

		HRESULT hr = mainDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&debugDevice);

		if(hr == S_OK)
		{
			hr = debugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

			debugDevice->Release();
		}
	}

	void CleanUp()
	{
		swapChain->Release();
		deviceContext->Release();
		renderTargetView->Release();
		rasterState->Release();
		depthStencilBuffer->Release();
		depthStencilState->Release();
		depthStencilView->Release();
		uiVertexShader->Release();
		uiPixelShader->Release();
		uiInputLayout->Release();
		unitQuadIndexBuffer->Release();
		unitQuadVertexBuffer->Release();
		ascendingCountBuffer->Release();
		uiTransformConstantBuffer->Release();

		ReportLiveObjects();
		mainDevice->Release();

		pD2DFactory->Release();
		d2dRenderTarget->Release();

		for(int i = 0; i < uiBrushes.size(); i++)
		{
			uiBrushes[i]->Release();
		}
	}



	void Draw()
	{
		deviceContext->ClearRenderTargetView(renderTargetView, clearColour);
		
		DrawScene();

		DrawUI();

		swapChain->Present(0, 0);
	}

	void DrawScene()
	{
	}

	bool UpdateUITransformBuffer(std::vector<Utils::FloatRect>& newData)
	{
		// Not nearly as nasty as it looks.  The memcpy is gaurunteed to work
		// for POD types which these are.  Very fast and the only possible
		// hinkiness is the implicit cast.  As FloatRect and D2D1_RECT_F have
		// the exact same internal structure this is fine as long as neither
		// change.  Would be very surprised if the D2D rect struct changes.
		uiRects.resize(newData.size());
		memcpy(uiRects.data(), newData.data(), newData.size() * sizeof(Utils::FloatRect));

		/*
		HRESULT hr;
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		hr = deviceContext->Map(uiTransformConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if(FAILED(hr))
		{
		return false;
		}

		memcpy(mappedResource.pData, newData.data(), newData.size() * sizeof(Utils::FloatRect));

		deviceContext->Unmap(uiTransformConstantBuffer, 0);

		numUIQuads = newData.size();*/

		return true;
	}

	void DrawUI()
	{
		d2dRenderTarget->BeginDraw();

		for(int i = 0; i < uiRects.size(); i++)
		{
			d2dRenderTarget->FillRectangle(&uiRects[i], uiBrushes[0]);
		}

		d2dRenderTarget->EndDraw();

		// Leaving in for now as this is how the main rendering will work.
		/*uInt32 strides[2];
		uInt32 offsets[2];
		ID3D11Buffer* bufferPointers[2];

		strides[0] = sizeof(uInt32) * 4;
		strides[1] = sizeof(float); 

		offsets[0] = 0;
		offsets[1] = 0;

		bufferPointers[0] = unitQuadVertexBuffer;
		bufferPointers[1] = ascendingCountBuffer;

		deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);
		deviceContext->IASetIndexBuffer(unitQuadIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		deviceContext->IASetInputLayout(uiInputLayout);
		deviceContext->VSSetShader(uiVertexShader, nullptr, 0);
		deviceContext->PSSetShader(uiPixelShader, nullptr, 0);

		deviceContext->VSSetConstantBuffers(0, 1, &uiTransformConstantBuffer);

		// TODO loop over all quads
		for(uInt32 i = 0; i < numUIQuads; i++)
		{
		deviceContext->DrawIndexedInstanced(6, 1, 0, 0, i);
		}*/
	}
}