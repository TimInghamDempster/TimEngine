#include <iostream>
#include <fstream>
#include <string>
namespace Platform
{
	bool OpenForRead(const std::u16string filename, std::ifstream& stream)
	{
		stream.open(Platform::Utf16ToWideString(filename), std::ios::in);

		if(stream.fail() || stream.bad() || !stream.good() || !stream.is_open())
		{
			Engine::Log(Platform::WideStringToUtf16(L"Error reading file: ") + filename);
			return false;
		}
		else
		{
			return true;
		}
	}
}