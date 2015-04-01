namespace UI
{
	struct UITag{};
	typedef Utils::Handle<UITag, int32, -1> UIHandle;

	std::vector<Utils::Rect> Rectangles;
	std::vector<UIHandle> Parents;
	std::vector<std::vector<UIHandle> > Children;
	std::vector<uInt32> Depths;

	UIHandle AddItem(Utils::Rect screenPosition, uInt32 depth, UIHandle parent = UIHandle::Invalid())
	{
		int32 id = Depths.size();

		while(id >= 1 && Depths[id - 1] > depth)
		{
			id--;
		}

		UIHandle handle = UIHandle(Rectangles.size());

		// Gaurunteed to be safe, id can never be < 0 so we can never
		// be before .begin and id can never be > .size therefore 
		// .begin + id can never be after .end.  Insert is valid
		// for both those values and all values inbetween.  Can
		// only fail if we don't keep all vectors in lockstep, which
		// would be a bug anyway.
		Rectangles.insert(Rectangles.begin() + id, screenPosition);
		Parents.insert(Parents.begin() + id, parent);
		Children.insert(Children.begin() + id, std::vector<UIHandle>());
		Depths.insert(Depths.begin() + id, depth);

		if(parent != UIHandle::Invalid())
		{
			// No need for safety check.  If the parent handle
			// is valid then it's Children vector was created when
			// the handle was returned.
			Children[parent.GetValue()].push_back(handle);
		}

		return handle;
	}
}