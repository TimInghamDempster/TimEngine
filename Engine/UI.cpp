namespace UI
{
	struct UITag{};
	typedef Utils::Handle<UITag, int32, -1> UIHandle;

	std::vector<Utils::uRect> Rectangles;
	std::vector<UIHandle> Parents;
	std::vector<std::vector<UIHandle> > Children;

	UIHandle AddItem(Utils::uRect screenPosition, UIHandle parent = UIHandle::Invalid())
	{
		UIHandle handle = UIHandle(Rectangles.size());

		Rectangles.push_back(screenPosition);
		Parents.push_back(parent);
		Children.push_back(std::vector<UIHandle>());

		if(parent != UIHandle::Invalid())
		{
			Children[parent.GetValue()].push_back(handle);
		}

		return handle;
	}
}