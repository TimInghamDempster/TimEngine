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

		World() : entityList(1000, true)
		{
		}
	};
}