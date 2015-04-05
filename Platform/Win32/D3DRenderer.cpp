namespace Renderer
{
#pragma comment (lib, "d3d11.lib")
#pragma comment(lib, "DXGI.lib")
	
	void DrawScene();
	void DrawUI();

	IDXGISwapChain* swapChain = 0;
	ID3D11Device* mainDevice = 0;
	ID3D11DeviceContext* deviceContext = 0;
	ID3D11RenderTargetView* renderTargetView = 0;
	ID3D11Texture2D* depthStencilBuffer = 0;
	ID3D11DepthStencilState* depthStencilState = 0;
	ID3D11DepthStencilView* depthStencilView = 0;
	ID3D11RasterizerState* rasterState = 0;

	ID3D11VertexShader* uiVertexShader = 0;
	ID3D11InputLayout* uiInputLayout = 0;
	ID3D11PixelShader* uiPixelShader = 0;
	ID3D11Buffer* unitQuadVertexBuffer = 0;
	ID3D11Buffer* unitQuadIndexBuffer = 0;
	ID3D11Buffer* ascendingCountBuffer = 0;

	float clearColour[4];
	uInt32 videoCardMemory = 0;

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

	bool InitD3DDevice(HWND hwnd)
	{
		HRESULT result;
		IDXGIFactory* factory;
		IDXGIAdapter* adapter;
		IDXGIOutput* adapterOutput;
		unsigned int numModes, i, numerator, denominator, stringLength;
		DXGI_MODE_DESC* displayModeList;
		DXGI_ADAPTER_DESC adapterDesc;
		int error;
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		D3D_FEATURE_LEVEL featureLevel;
		ID3D11Texture2D* backBufferPtr;
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		D3D11_RASTERIZER_DESC rasterDesc;
		D3D11_VIEWPORT viewport;
		float fieldOfView, screenAspect;

		//Before we can initialize Direct3D we have to get the refresh rate from the video card/monitor. Each computer may be slightly different so we will need to query for that information. We query for the numerator and denominator values and then pass them to DirectX during the setup and it will calculate the proper refresh rate. If we don't do this and just set the refresh rate to a default value which may not exist on all computers then DirectX will respond by performing a blit instead of a buffer flip which will degrade performance and give us annoying errors in the debug output.

		// Create a DirectX graphics interface factory.
		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if(FAILED(result))
		{
			return false;
		}

		// Use the factory to create an adapter for the primary graphics interface (video card).
		result = factory->EnumAdapters(0, &adapter);
		if(FAILED(result))
		{
			return false;
		}

		// Enumerate the primary adapter output (monitor).
		result = adapter->EnumOutputs(0, &adapterOutput);
		if(FAILED(result))
		{
			return false;
		}

		// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
		if(FAILED(result))
		{
			return false;
		}

		// Create a list to hold all the possible display modes for this monitor/video card combination.
		displayModeList = new DXGI_MODE_DESC[numModes];
		if(!displayModeList)
		{
			return false;
		}

		// Now fill the display mode list structures.
		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
		if(FAILED(result))
		{
			return false;
		}

		// Now go through all the display modes and find the one that matches the screen width and height.
		// When a match is found store the numerator and denominator of the refresh rate for that monitor.
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
		//We now have the numerator and denominator for the refresh rate. The last thing we will retrieve using the adapter is the name of the video card and the amount of memory on the video card.

		// Get the adapter (video card) description.
		result = adapter->GetDesc(&adapterDesc);
		if(FAILED(result))
		{
			return false;
		}

		// Store the dedicated video card memory in megabytes.
		videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

		// Convert the name of the video card to a character array and store it.
		/*error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
		if(error != 0)
		{
			return false;
		}*/
		//Now that we have stored the numerator and denominator for the refresh rate and the video card information we can release the structures and interfaces used to get that information.

		// Release the display mode list.
		delete [] displayModeList;
		displayModeList = 0;

		// Release the adapter output.
		adapterOutput->Release();
		adapterOutput = 0;

		// Release the adapter.
		adapter->Release();
		adapter = 0;

		// Release the factory.
		factory->Release();
		factory = 0;
		//Now that we have the refresh rate from the system we can start the DirectX initialization. The first thing we'll do is fill out the description of the swap chain. The swap chain is the front and back buffer to which the graphics will be drawn. Generally you use a single back buffer, do all your drawing to it, and then swap it to the front buffer which then displays on the user's screen. That is why it is called a swap chain.

		// Initialize the swap chain description.
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

		// Set to a single back buffer.
		swapChainDesc.BufferCount = 1;

		// Set the width and height of the back buffer.
		swapChainDesc.BufferDesc.Width = Engine::ScreenRect.Width;
		swapChainDesc.BufferDesc.Height = Engine::ScreenRect.Height;

		// Set regular 32-bit surface for the back buffer.
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//The next part of the description of the swap chain is the refresh rate. The refresh rate is how many times a second it draws the back buffer to the front buffer. If vsync is set to true in our graphicsclass.h header then this will lock the refresh rate to the system settings (for example 60hz). That means it will only draw the screen 60 times a second (or higher if the system refresh rate is more than 60). However if we set vsync to false then it will draw the screen as many times a second as it can, however this can cause some visual artifacts.

		// Set the refresh rate of the back buffer.
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

		// Set the usage of the back buffer.
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

		// Set the handle for the window to render to.
		swapChainDesc.OutputWindow = hwnd;

		// Turn multisampling off.
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		// Set to full screen or windowed mode.
		/*if(fullscreen)
		{
			swapChainDesc.Windowed = false;
		}
		else*/
		{
			swapChainDesc.Windowed = true;
		}

		// Set the scan line ordering and scaling to unspecified.
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		// Discard the back buffer contents after presenting.
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		// Don't set the advanced flags.
		swapChainDesc.Flags = 0;
		//After setting up the swap chain description we also need to setup one more variable called the feature level. This variable tells DirectX what version we plan to use. Here we set the feature level to 11.0 which is DirectX 11. You can set this to 10 or 9 to use a lower level version of DirectX if you plan on supporting multiple versions or running on lower end hardware.

		// Set the feature level to DirectX 11.
		featureLevel = D3D_FEATURE_LEVEL_11_0;
		//Now that the swap chain description and feature level have been filled out we can create the swap chain, the Direct3D device, and the Direct3D device context. The Direct3D device and Direct3D device context are very important, they are the interface to all of the Direct3D functions. We will use the device and device context for almost everything from this point forward.

		//Those of you reading this who are familiar with the previous versions of DirectX will recognize the Direct3D device but will be unfamiliar with the new Direct3D device context. Basically they took the functionality of the Direct3D device and split it up into two different devices so you need to use both now.

		//Note that if the user does not have a DirectX 11 video card this function call will fail to create the device and device context. Also if you are testing DirectX 11 functionality yourself and don't have a DirectX 11 video card then you can replace D3D_DRIVER_TYPE_HARDWARE with D3D_DRIVER_TYPE_REFERENCE and DirectX will use your CPU to draw instead of the video card hardware. Note that this runs 1/1000 the speed but it is good for people who don't have DirectX 11 video cards yet on all their machines.

		// Create the swap chain, Direct3D device, and Direct3D device context.
		//result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, &featureLevel, 1, 
		result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, 
			D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &mainDevice, NULL, &deviceContext);
		if(FAILED(result))
		{
			return false;
		}
		//Sometimes this call to create the device will fail if the primary video card is not compatible with DirectX 11. Some machines may have the primary card as a DirectX 10 video card and the secondary card as a DirectX 11 video card. Also some hybrid graphics cards work that way with the primary being the low power Intel card and the secondary being the high power Nvidia card. To get around this you will need to not use the default device and instead enumerate all the video cards in the machine and have the user choose which one to use and then specify that card when creating the device.

		//Now that we have a swap chain we need to get a pointer to the back buffer and then attach it to the swap chain. We'll use the CreateRenderTargetView function to attach the back buffer to our swap chain.

		// Get the pointer to the back buffer.
		result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
		if(FAILED(result))
		{
			return false;
		}

		// Create the render target view with the back buffer pointer.
		result = mainDevice->CreateRenderTargetView(backBufferPtr, NULL, &renderTargetView);
		if(FAILED(result))
		{
			return false;
		}

		// Release pointer to the back buffer as we no longer need it.
		backBufferPtr->Release();
		backBufferPtr = 0;
		//We will also need to set up a depth buffer description. We'll use this to create a depth buffer so that our polygons can be rendered properly in 3D space. At the same time we will attach a stencil buffer to our depth buffer. The stencil buffer can be used to achieve effects such as motion blur, volumetric shadows, and other things.

		// Initialize the description of the depth buffer.
		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

		// Set up the description of the depth buffer.
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
		//Now we create the depth/stencil buffer using that description. You will notice we use the CreateTexture2D function to make the buffers, hence the buffer is just a 2D texture. The reason for this is that once your polygons are sorted and then rasterized they just end up being colored pixels in this 2D buffer. Then this 2D buffer is drawn to the screen.

		// Create the texture for the depth buffer using the filled out description.
		result = mainDevice->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);
		if(FAILED(result))
		{
			return false;
		}
		//Now we need to setup the depth stencil description. This allows us to control what type of depth test Direct3D will do for each pixel.

		// Initialize the description of the stencil state.
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		// Set up the description of the stencil state.
		depthStencilDesc.DepthEnable = false;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = false;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing.
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing.
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		//With the description filled out we can now create a depth stencil state.

		// Create the depth stencil state.
		result = mainDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
		if(FAILED(result))
		{
			return false;
		}
		//With the created depth stencil state we can now set it so that it takes effect. Notice we use the device context to set it.

		// Set the depth stencil state.
		deviceContext->OMSetDepthStencilState(depthStencilState, 1);
		//The next thing we need to create is the description of the view of the depth stencil buffer. We do this so that Direct3D knows to use the depth buffer as a depth stencil texture. After filling out the description we then call the function CreateDepthStencilView to create it.

		// Initailze the depth stencil view.
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

		// Set up the depth stencil view description.
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		// Create the depth stencil view.
		result = mainDevice->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
		if(FAILED(result))
		{
			return false;
		}
		//With that created we can now call OMSetRenderTargets. This will bind the render target view and the depth stencil buffer to the output render pipeline. This way the graphics that the pipeline renders will get drawn to our back buffer that we previously created. With the graphics written to the back buffer we can then swap it to the front and display our graphics on the user's screen.

		// Bind the render target view and depth stencil buffer to the output render pipeline.
		deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
		//Now that the render targets are setup we can continue on to some extra functions that will give us more control over our scenes for future tutorials. First thing is we'll create is a rasterizer state. This will give us control over how polygons are rendered. We can do things like make our scenes render in wireframe mode or have DirectX draw both the front and back faces of polygons. By default DirectX already has a rasterizer state set up and working the exact same as the one below but you have no control to change it unless you set up one yourself.

		// Setup the raster description which will determine how and what polygons will be drawn.
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

		// Create the rasterizer state from the description we just filled out.
		result = mainDevice->CreateRasterizerState(&rasterDesc, &rasterState);
		if(FAILED(result))
		{
			return false;
		}

		// Now set the rasterizer state.
		deviceContext->RSSetState(rasterState);
		//The viewport also needs to be setup so that Direct3D can map clip space coordinates to the render target space. Set this to be the entire size of the window.

		// Setup the viewport for rendering.
		viewport.Width = (float)Engine::ScreenRect.Width;
		viewport.Height = (float)Engine::ScreenRect.Height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		// Create the viewport.
		deviceContext->RSSetViewports(1, &viewport);
		//Now we will create the projection matrix. The projection matrix is used to translate the 3D scene into the 2D viewport space that we previously created. We will need to keep a copy of this matrix so that we can pass it to our shaders that will be used to render our scenes.

		return true;
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

		ReportLiveObjects();
		mainDevice->Release();
	}



	void Draw()
	{

		// clear the back buffer to a deep blue
		deviceContext->ClearRenderTargetView(renderTargetView, clearColour);

		// do 3D rendering on the back buffer here

		DrawScene();

		DrawUI();

		// switch the back buffer and the front buffer
		swapChain->Present(0, 0);
	}

	void DrawScene()
	{
	}

	void DrawUI()
	{
		uInt32 strides[2];
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
		
		// TODO loop over all quads
		for(uInt32 i = 0; i < 1; i++)
		{
			deviceContext->DrawIndexedInstanced(6, 1, 0, 0, i);
		}
	}
}