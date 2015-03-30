namespace World
{
	using std::u16string;
	using std::ifstream;

	void Tick()	{}

	void LoadLevel(u16string filename)
	{
		ifstream levelFile;
		bool success = Platform::OpenForRead(filename, levelFile);

		if(!success)
		{
			return;
		}
	}
}