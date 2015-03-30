
namespace Platform
{
	std::wstring Utf16ToWideString(const std::u16string &inputString)
	{
		// Using the fact that wstring is two bytes on windows and will
		// contain a utf16 string so all we are really doing is a format
		// conversion, not an actual re-encoding, therefore string lengths
		// stay the same.
		std::wstring outString;
		outString.reserve(inputString.length());

		for(std::u16string::const_iterator itr = inputString.begin();
			itr != inputString.end();
			itr++)
		{
			// C-style cast is perfectly safe as we want the same two bytes
			// in both strings.
			outString.push_back((wchar_t)(*itr));
		}

		return outString;
	}

	std::u16string WideStringToUtf16(const std::wstring &inputString)
	{
		// Using the fact that wstring is two bytes on windows and will
		// contain a utf16 string so all we are really doing is a format
		// conversion, not an actual re-encoding, therefore string lengths
		// stay the same.
		std::u16string outString;
		outString.reserve(inputString.length());

		for(std::wstring::const_iterator itr = inputString.begin();
			itr != inputString.end();
			itr++)
		{
			// C-style cast is perfectly safe as we want the same two bytes
			// in both strings.
			outString.push_back((char16_t)(*itr));
		}

		return outString;
	}
}