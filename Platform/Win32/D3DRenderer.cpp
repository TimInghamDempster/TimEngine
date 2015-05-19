namespace Renderer
{
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d2d1.lib")
#pragma comment (lib, "dwrite.lib")
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
	IDWriteFactory* pDWriteFactory = nullptr;
	IDWriteTextFormat* pTextFormat = nullptr;

	float clearColour[4];
	uInt32 videoCardMemory = 0;
	uInt32 numUIQuads = 0;
	
	namespace Colours
	{
		enum Values
		{
			Black,
			Grey,
			LightGrey,
			Transparent,
			Count
		};
	}

	struct UIScreen
	{
		std::vector<D2D1_RECT_F> rects;
		std::vector<UI::UIElementType::Values> elementTypes;
		std::vector<std::wstring> text;
		std::vector<Colours::Values> colours;
		std::vector<char> visible;
	};
	
	std::vector<ID2D1SolidColorBrush*> uiBrushes;
	std::vector<UIScreen> uiScreens;

	UIScreenRenderHandle activeScreenHandle;


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

	bool InitDWrite()
	{
		HRESULT hr;
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&pDWriteFactory)
			);

		if(hr != S_OK)
		{
			Engine::Log(Platform::WideStringToUtf16(L"Error creating DWrite factory."));
			return false;
		}

		hr = pDWriteFactory->CreateTextFormat(
			L"Arial",                // Font family name.
			NULL,                       // Font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			12.0f,
			L"en-us",
			&pTextFormat
			);

		if(hr != S_OK)
		{
			Engine::Log(Platform::WideStringToUtf16(L"Error creating DWrite text format."));
			return false;
		}
		
		hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		
		if(hr != S_OK)
		{
			Engine::Log(Platform::WideStringToUtf16(L"Error text aligning text format"));
			return false;
		}

		hr = pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		if(hr != S_OK)
		{
			Engine::Log(Platform::WideStringToUtf16(L"Error paragraph aligning text format."));
			return false;
		}
		
		return true;
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

		uiBrushes.resize(Colours::Values::Count);

		hr = d2dRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Gray, 1.0f),
			&(uiBrushes[Colours::Values::Grey])
			);

		hr = d2dRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
			&(uiBrushes[Colours::Values::Black])
			);

		hr = d2dRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::LightGray, 1.0f),
			&(uiBrushes[Colours::Values::LightGrey])
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

		swapChainDesc.BufferDesc.Width = (UINT)Engine::ScreenRect.Width;
		swapChainDesc.BufferDesc.Height = (UINT)Engine::ScreenRect.Height;

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

		depthBufferDesc.Width = (UINT)Engine::ScreenRect.Width;
		depthBufferDesc.Height = (UINT)Engine::ScreenRect.Height;
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

		return true;
	}

	void Init(HWND hwnd)
	{
		clearColour[0] = 0.0f;
		clearColour[1] = 0.0f;
		clearColour[2] = 1.0f;
		clearColour[3] = 1.0f;

		// There is an argument to be made for rolling all of these functions into this main
		// Init.  It would gauruntee that they are only called here and in the correct order.
		// On the other hand it would make jumping to the specific piece of code much harder.
		InitD3DDevice(hwnd);
		InitD2DDevice();
		InitDWrite();

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

		for(uInt32 i = 0; i < uiBrushes.size(); i++)
		{
			uiBrushes[i]->Release();
		}

		d2dRenderTarget->Release();
		pTextFormat->Release();
		pD2DFactory->Release();
		pDWriteFactory->Release();
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

	void UpdateUIColours(UIScreenRenderHandle screen, std::vector<Colours::Values>& colours)
	{
		int32 screenId = screen.GetValue();
		if(screen == UIScreenRenderHandle::Invalid() || screenId >= (int32)uiScreens.size())
		{
			Engine::Log(Platform::WideStringToUtf16(L"Invalid screen handle passed to Renderer::UpdateColours"));
			return;
		}

		UIScreen& uiScreen = uiScreens[screenId];

		if(colours.size() != uiScreen.colours.size())
		{
			Engine::Log(Platform::WideStringToUtf16(L"New colour array wrong size in Renderer::UpdateUIColours"));
			return;
		}

		memcpy(uiScreen.colours.data(), colours.data(), sizeof(Colours::Values) * colours.size());
	}

	void UpdateUIVisibility(UIScreenRenderHandle screen, std::vector<char>& visible)
	{
		int32 screenId = screen.GetValue();
		if(screen == UIScreenRenderHandle::Invalid() || screenId >= (int32)uiScreens.size())
		{
			Engine::Log(Platform::WideStringToUtf16(L"Invalid screen handle passed to Renderer::UpdateUIVisibility"));
			return;
		}

		UIScreen& uiScreen = uiScreens[screenId];

		if(visible.size() != uiScreen.visible.size())
		{
			Engine::Log(Platform::WideStringToUtf16(L"New colour array wrong size in Renderer::UpdateUIVisibility"));
			return;
		}

		memcpy(uiScreen.visible.data(), visible.data(), sizeof(char) * visible.size());
	}

	UIScreenRenderHandle CreateUIScreen(std::vector<Utils::FloatRect>& newData,
		std::vector<UI::UIElementType::Values>& types,
		std::vector<std::u16string>& text,
		std::vector<Colours::Values>& colours,
		std::vector<char>& visible)
	{
		UIScreen uiScreen;
		UIScreenRenderHandle handle(uiScreens.size());

		uiScreen.rects.reserve(newData.size());
		for(uInt32 i = 0; i < newData.size(); i++)
		{
			D2D1_RECT_F rect = {newData[i].Left, newData[i].Top, newData[i].Left + newData[i].Width, newData[i].Top + newData[i].Height};
			uiScreen.rects.push_back(rect);
		}

		uiScreen.elementTypes.resize(types.size());
		memcpy(uiScreen.elementTypes.data(), types.data(), types.size() * sizeof(UI::UIElementType::Values));

		uiScreen.colours.resize(colours.size());
		memcpy(uiScreen.colours.data(), colours.data(), sizeof(Colours::Values) * colours.size());

		uiScreen.visible.resize(visible.size());
		memcpy(uiScreen.visible.data(), visible.data(), sizeof(bool) * visible.size());

		uiScreen.text.resize(text.size());
		for(uInt32 i = 0; i < text.size(); i++)
		{
			uiScreen.text[i] = Platform::Utf16ToWideString(text[i]);
		}

		uiScreens.push_back(uiScreen);

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

		return handle;
	}

	void SetActiveUIScreen(UIScreenRenderHandle screenHandle)
	{
		if(screenHandle != UIScreenRenderHandle::Invalid() && screenHandle.GetValue() <= (int32)uiScreens.size())
		{
			activeScreenHandle = screenHandle;
		}
		else
		{
			Engine::Log(Platform::WideStringToUtf16(L"Invalid handle passed to Renderer::SetActiveUIScreen"));
		}
	}

	void DrawUI()
	{
		if(activeScreenHandle != UIScreenRenderHandle::Invalid() && activeScreenHandle.GetValue() <= (int32)uiScreens.size())
		{
			UIScreen& uiScreen = uiScreens[activeScreenHandle.GetValue()];
			d2dRenderTarget->BeginDraw();

			for(uInt32 i = 0; i < uiScreen.rects.size(); i++)
			{
				if(uiScreen.visible[i])
				{
					if(uiScreen.colours[i] != Colours::Transparent)
					{
						d2dRenderTarget->FillRectangle(uiScreen.rects[i], uiBrushes[uiScreen.colours[i]]);
					}
					d2dRenderTarget->DrawTextW(uiScreen.text[i].c_str(), uiScreen.text[i].size(), pTextFormat, &uiScreen.rects[i], uiBrushes[Colours::Black]);
				}
			}

			d2dRenderTarget->EndDraw();
		}
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