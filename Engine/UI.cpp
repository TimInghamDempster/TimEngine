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
		UIElementHandle rootElement;
	};

	std::vector<Screen> screens;
	UIScreenHandle activeScreen;

	void DoMouseOver(int32 item)
	{
		// TODO gaurd against invalid active screen here?
		Screen& screen = screens[activeScreen.GetValue()];
		std::vector<Renderer::Colours::Values> colours;
		colours.reserve(screen.rectangles.size());
		for(int  i = 0; i < screen.rectangles.size(); i++)
		{
			if(i == item)
			{
				colours.push_back(Renderer::Colours::LightGrey);
			}
			else
			{
				colours.push_back(Renderer::Colours::Grey);
			}
		}

		Renderer::UpdateColours(screen.renderHandle, colours);

	}

	void Tick()
	{
		if(activeScreen != UIScreenHandle::Invalid())
		{
			float cursorX = Platform::CursorPosition.x;
			float cursorY = Platform::CursorPosition.y;

			if(cursorX < 128 && cursorY < 22)
			{
				int a = 0;
				a++;
			}

			Screen& screen = screens[activeScreen.GetValue()];
			
			for(int i = screen.rectangles.size() - 1; i >= 0; i--)
			{
				float minX = screen.rectangles[i].Left;
				float minY = screen.rectangles[i].Top;
				float maxX = screen.rectangles[i].Left + screen.rectangles[i].Width;
				float maxY = screen.rectangles[i].Top + screen.rectangles[i].Height;


				if(cursorX < maxX &&
					cursorY < maxY &&
					cursorX > minX &&
					cursorY > minY)
				{
					if(screen.elementTypes[i] != UIElementType::Text)
					{
						DoMouseOver(i);
						break;
					}
				}

			}
		}
	}

	UIElementHandle AddItem(UIScreenHandle screenHandle, 
		Utils::FloatRect screenPosition, 
		uInt32 depth, 
		UIElementType::Values type, 
		std::u16string text,
		UIElementHandle parent = UIElementHandle::Invalid())
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

		if(parent == UIElementHandle::Invalid() && type != UIElementType::ScreenRoot)
		{
			parent = screen.rootElement;
		}

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

	UIScreenHandle CreateScreen(Utils::FloatRect screenRect)
	{
		UIScreenHandle handle(screens.size());
		screens.push_back(Screen());

		UIElementHandle root = AddItem(handle, screenRect, 0, UIElementType::ScreenRoot, std::u16string());
		screens[handle.GetValue()].rootElement = root;

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

		activeScreen = screenHandle;
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

		std::vector<Renderer::Colours::Values> colours;
		colours.reserve(screen.rectangles.size());
		for(int  i = 0; i < screen.rectangles.size(); i++)
		{
			colours.push_back(Renderer::Colours::Grey);
		}

		screen.renderHandle = Renderer::CreateUIScreen(screen.rectangles, screen.elementTypes, screen.textStrings, colours);
	}
}