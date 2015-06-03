
#pragma comment(lib, "Shlwapi.lib")

namespace Platform
{
	bool OpenFileDialog(std::string& filename)
	{
		
		 HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | 
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileOpenDialog *pFileOpen;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
                IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            // Show the Open dialog box.
            hr = pFileOpen->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem *pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                { 
					PWSTR outputName;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &outputName);
                    pItem->Release();
					filename = WideStringToUtf8(outputName);
					CoTaskMemFree(outputName);
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
	return SUCCEEDED(hr);
	}
	
	bool OpenForRead(const std::string filename, std::ifstream& stream, std::ios::openmode mode = std::ios::in)
	{
		std::wstring convertedFilename = Platform::Utf8ToWideString(filename);
		stream.open(convertedFilename, mode);

		if(stream.fail() || stream.bad() || !stream.good() || !stream.is_open())
		{
			std::string error = strerror(errno);
			Engine::Log("Error reading file: " + filename);
			return false;
		}
		else
		{
			return true;
		}
	}

	bool BrowseAndOpenFile(std::ifstream& stream)
	{
		std::string filename;

		if(!OpenFileDialog(filename))
		{
			return false;
		}

		return OpenForRead(filename, stream);
	}

	bool ConvertToPathRoot(std::string& path)
	{
		std::wstring widened = Utf8ToWideString(path);
		wchar_t* widePath = new wchar_t[widened.length()];
		std::copy(widened.begin(), widened.end(), widePath);

		HRESULT converted = PathRemoveFileSpec(widePath);

		if(SUCCEEDED(converted))
		{
			path = WideStringToUtf8(widePath);
		}

		delete[] widePath;

		return SUCCEEDED(converted);
	}
}