#include <iostream>
#include <fstream>
#include <string>
namespace Platform
{
	bool OpenForRead(const std::u16string filename, std::ifstream& stream, std::ios::openmode mode = std::ios::in)
	{
		std::wstring convertedFilename = Platform::Utf16ToWideString(filename);
		stream.open(convertedFilename, mode);

		if(stream.fail() || stream.bad() || !stream.good() || !stream.is_open())
		{
			std::string error = strerror(errno);
			Engine::Log(Platform::WideStringToUtf16(L"Error reading file: ") + filename);
			return false;
		}
		else
		{
			return true;
		}
	}
}