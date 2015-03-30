namespace UI
{
	struct UITag{};
	typedef Utils::Handle<UITag, int32, -1> UIHandle;

	UIHandle AddItem(Utils::uRect screenPosition, UIHandle parent = UIHandle::Invalid())
	{
		return UIHandle::Invalid();
	}

	void RemoveItem(UIHandle item)
	{
	}
}