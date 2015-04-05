namespace Engine
{
	char* ReadBinaryFile(std::u16string filename, int32& outFileLength, bool& outSuccess)
	{
		std::ifstream file;
		bool result = Platform::OpenForRead(filename, file, std::ios::binary);
		
		if(result == false)
		{
			outSuccess = false;
			outFileLength = 0;
			return new char[0];
		}

		// get length of file:
		file.seekg (0, file.end);
		outFileLength = (int32)file.tellg();
		file.seekg (0, file.beg);

		// allocate memory:
		char * buffer = new char [outFileLength];

		// read data as a block:
		file.read (buffer,outFileLength);

		file.close();

		outSuccess = true;

		return buffer;
	}
}