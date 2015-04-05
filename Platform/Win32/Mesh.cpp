namespace Renderer
{
	bool CreateAscendingBuffer(ID3D11Device* device, ID3D11Buffer** dataBuffer, uInt32 count)
	{
		bool succeeded = true;
		uInt32* data = new uInt32[count];

		for(uInt32 i = 0; i < count; i++)
		{
			data[i] = i;
		}

		D3D11_BUFFER_DESC bufferDesc;
		D3D11_SUBRESOURCE_DATA dataSubresource;
		HRESULT hr;

		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(float) * count;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		dataSubresource.pSysMem = data;
		dataSubresource.SysMemPitch = 0;
		dataSubresource.SysMemSlicePitch = 0;

		hr = device->CreateBuffer(&bufferDesc, &dataSubresource, dataBuffer);
		if(FAILED(hr))
		{
			Engine::Log(Platform::WideStringToUtf16(L"Failed to generate buffer"));
			succeeded = false;
		}
		delete[] data;
		return succeeded;
	}

	bool CreateQuadMeshBuffers(ID3D11Device* device, ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer)
	{
		struct vertex
		{
			float coord[4];
		};

		D3D11_BUFFER_DESC vertexBufferDesc;
		D3D11_BUFFER_DESC indexBufferDesc;
		D3D11_SUBRESOURCE_DATA vertexData;
		D3D11_SUBRESOURCE_DATA indexData;

		HRESULT hr;
		
		vertex vertices[] = 
		{
			{ 0.0f, 0.0f, 0.5f, 1.0f },
			{ 1.0f, 0.0f, 0.5f, 1.0f },
			{ 0.0f, 1.0f, 0.5f, 1.0f },
			{ 1.0f, 1.0f, 0.5f, 1.0f }
		};

		uInt32 indices[] = { 0, 2, 1, 1, 2, 3 };

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(vertex) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, vertexBuffer);
	if(FAILED(hr))
	{
		Engine::Log(Platform::WideStringToUtf16(L"Failed to generate vertex buffer"));
		return false;
	}

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * 6;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&indexBufferDesc, &indexData, indexBuffer);
	if(FAILED(hr))
	{
		Engine::Log(Platform::WideStringToUtf16(L"Failed to generate index buffer"));
		return false;
	}

	return true;
	}
}