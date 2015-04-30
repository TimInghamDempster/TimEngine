namespace UI
{
	struct Screen
	{
		std::vector<Utils::FloatRect> rectangles;
		std::vector<UIElementHandle> parents;
		std::vector<std::vector<UIElementHandle> > children;
		std::vector<uInt32> depths;
		std::vector<UIElementType::Values> elementTypes;
		std::vector<std::u16string> textStrings;
		Renderer::UIScreenRenderHandle renderHandle;
	};

	std::vector<Screen> screens;

	void Tick()
	{
	}

	UIScreenHandle CreateScreen()
	{
		UIScreenHandle handle(screens.size());
		screens.push_back(Screen());
		return handle;
	}

	UIElementHandle AddItem(UIScreenHandle screenHandle, Utils::FloatRect screenPosition, uInt32 depth, UIElementType::Values type, std::u16string text, UIElementHandle parent = UIElementHandle::Invalid())
	{
		int32 screenId = screenHandle.GetValue();
		
		if(screenId == UIScreenHandle::Invalid().GetValue() || screenId >= screens.size())
		{
			Engine::Log(Platform::WideStringToUtf16(L"Invalid screen handle passed to UI::AddItem"));
			return UIElementHandle::Invalid();
		}

		Screen& screen = screens[screenId];

		int32 id = screen.depths.size();

		while(id >= 1 && screen.depths[id - 1] > depth)
		{
			id--;
		}

		UIElementHandle handle = UIElementHandle(screen.rectangles.size());

		// Gaurunteed to be safe, id can never be < 0 so we can never
		// be before .begin and id can never be > .size therefore 
		// .begin + id can never be after .end.  Insert is valid
		// for both those values and all values inbetween.  Can
		// only fail if we don't keep all vectors in lockstep, which
		// would be a bug anyway.
		screen.rectangles.insert(screen.rectangles.begin() + id, screenPosition);
		screen.parents.insert(screen.parents.begin() + id, parent);
		screen.children.insert(screen.children.begin() + id, std::vector<UIElementHandle>());
		screen.depths.insert(screen.depths.begin() + id, depth);
		screen.elementTypes.insert(screen.elementTypes.begin() + id, type);
		screen.textStrings.insert(screen.textStrings.begin() + id, text);

		if(parent != UIElementHandle::Invalid())
		{
			// No need for safety check.  If the parent handle
			// is valid then it's Children vector was created when
			// the handle was returned.
			screen.children[parent.GetValue()].push_back(handle);
			screen.rectangles[id].Left += screen.rectangles[parent.GetValue()].Left;
			screen.rectangles[id].Top += screen.rectangles[parent.GetValue()].Top;
		}

		return handle;
	}

	void ActivateScreen(UIScreenHandle screenHandle)
	{		
		int32 screenId = screenHandle.GetValue();
		
		if(screenId == UIScreenHandle::Invalid().GetValue() || screenId >= screens.size())
		{
			Engine::Log(Platform::WideStringToUtf16(L"Invalid screen handle passed to UI::ActivateScreen"));
			return ;
		}

		Screen& screen = screens[screenId];

		Renderer::SetActiveUIScreen(screen.renderHandle);
	}

	void ScreenUpdateComplete(UIScreenHandle screenHandle)
	{
		int32 screenId = screenHandle.GetValue();
		
		if(screenId == UIScreenHandle::Invalid().GetValue() || screenId >= screens.size())
		{
			Engine::Log(Platform::WideStringToUtf16(L"Invalid screen handle passed to UI::ScreenUpdateComplete"));
			return ;
		}

		Screen& screen = screens[screenId];

		screen.renderHandle = Renderer::CreateUIScreen(screen.rectangles, screen.elementTypes, screen.textStrings);
	}
}