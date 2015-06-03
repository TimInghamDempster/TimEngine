namespace Engine
{
	struct EntityData
	{
		std::vector<std::u16string> names;
		std::vector<Maths::Vector> positions;
	};

	struct World
	{
		StableHandleList<GameEntityTag, EntityData> entityList;
		std::string worldRoot;

		World() : entityList(1000, true)
		{
		}

		void LoadWorld(std::string worldFileName)
		{
			std::ifstream file;

			bool worldOpened = Platform::OpenForRead(worldFileName, file);

			if(worldOpened)
			{
				worldRoot = worldFileName;

				Platform::ConvertToPathRoot(worldRoot);

				std::string line;

				while(std::getline(file, line))
				{
					std::stringstream stream(line);

					std::string identifier;
					std::getline(stream, identifier, ' ');
				
					if(identifier == "Entity")
					{
						std::string entityFilename;
						std::getline(stream, entityFilename, ' ');
						LoadEntity(entityFilename);
					}
				}
			}
		}

		void LoadEntity(std::string filename)
		{
		}
	};
}