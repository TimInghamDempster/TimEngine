
namespace Platform
{
	std::wstring Utf8ToWideString(const std::string &inputString)
	{
		// This isn't an actual unicode conversion function but
		// works for ascii and can be fixed later.
		std::wstring outString;
		outString.reserve(inputString.length());

		for(std::string::const_iterator itr = inputString.begin();
			itr != inputString.end();
			itr++)
		{
			// C-style cast is perfectly safe as we want the same two bytes
			// in both strings.
			outString.push_back((wchar_t)(*itr));
		}

		return outString;
	}

	std::string WideStringToUtf8(const std::wstring &inputString)
	{
		// This isn't an actual unicode conversion function but
		// works for ascii and can be fixed later.
		std::string outString;
		outString.reserve(inputString.length());

		for(std::wstring::const_iterator itr = inputString.begin();
			itr != inputString.end();
			itr++)
		{
			// C-style cast is perfectly safe as we want the same two bytes
			// in both strings.
			outString.push_back((char)(*itr));
		}

		return outString;
	}
}