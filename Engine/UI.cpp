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
		std::vector<Renderer::Colours::Values> itemColours;
		std::vector<char> expanded;
		std::vector<char> visible;
		std::vector<char> isOnActivePath;
		std::vector<UIElementHandle> rootElements;
		Renderer::UIScreenRenderHandle renderHandle;
	};

	std::vector<Screen> screens;
	UIScreenHandle activeScreen;
	bool g_leftButtonClicked = false;
	bool g_rightButtonClicked = false;
	int32 mouseOverItem = -1;

	void UpdateLayout(UIScreenHandle screenHandle)
	{
		int32 screenId = screenHandle.GetValue();
		
		if(screenId == UIScreenHandle::Invalid().GetValue() || screenId >= screens.size())
		{
			Engine::Log(Platform::WideStringToUtf16(L"Invalid screen handle passed to UI::UpdateLayout"));
			return;
		}

		Screen& screen = screens[screenId];

		std::vector<UIElementHandle> active;
		std::vector<UIElementHandle> inactive;
		
		for(int i = 0; i < screen.rootElements.size(); i++)
		{
			active.push_back(screen.rootElements[i]);
		}

		bool itemsToProcess = true;

		while(itemsToProcess == true)
		{
			itemsToProcess = false;
			
			std::vector<UIElementHandle> toAddActive;
			std::vector<UIElementHandle> toAddInactive;

			for(int i = 0; i < active.size(); i++)
			{
				int32 item = active[i].GetValue();
				if(screen.children[item].size() > 0)
				{
					itemsToProcess = true;
					if(screen.expanded[item])
					{
						for(int j = 0; j < screen.children[item].size(); j++)
						{
							UIElementHandle child = screen.children[item][j];
							screen.visible[child.GetValue()] = true;
							toAddActive.push_back(child);
						}
					}
					else
					{
						for(int j = 0; j < screen.children[item].size(); j++)
						{
							UIElementHandle child = screen.children[item][j];
							screen.visible[child.GetValue()] = false;
							toAddInactive.push_back(child);
						}
					}
				}
			}

			for(int i = 0; i < inactive.size(); i++)
			{
				int32 item = inactive[i].GetValue();

				if(screen.children[item].size() > 0)
				{
					itemsToProcess = true;
					for(int j = 0; j < screen.children[item].size(); j++)
					{
						UIElementHandle child = screen.children[item][j];
						screen.visible[child.GetValue()] = false;
						toAddInactive.push_back(child);
					}
				}
			}

			active.clear();
			inactive.clear();
			
			active = toAddActive;
			inactive = toAddInactive;
		}

		if(screen.renderHandle != Renderer::UIScreenRenderHandle::Invalid())
		{
			Renderer::UIScreen& renderScreen = Renderer::uiScreens[screen.renderHandle.GetValue()];
		
			std::copy(screen.visible.begin(), screen.visible.end(), renderScreen.visible.begin());
		}
	}

	void DoMouseClick(int32 item)
	{
		Screen& screen = screens[activeScreen.GetValue()];
		switch (screen.elementTypes[item])
		{
		case UIElementType::Button:
			break;
		case UIElementType::MenuButton:
			screen.expanded[item] = screen.expanded[item] ? false : true;
			break;
		}
	}

	void DoEndMouseOver(int32 item)
	{
		// TODO gaurd against invalid active screen here?
		Screen& screen = screens[activeScreen.GetValue()];
		screen.itemColours[item] = Renderer::Colours::Grey;

		Renderer::UpdateUIColours(screen.renderHandle, screen.itemColours);

		ZeroMemory(screen.isOnActivePath.data(), sizeof(char) * screen.isOnActivePath.size());
	}

	void DoStartMouseOver(int32 item)
	{
		// TODO gaurd against invalid active screen here?
		Screen& screen = screens[activeScreen.GetValue()];
		screen.itemColours[item] = Renderer::Colours::LightGrey;

		Renderer::UpdateUIColours(screen.renderHandle, screen.itemColours);

		screen.isOnActivePath[item] = true;
		UIElementHandle parent = screen.parents[item];

		while(parent != UIElementHandle::Invalid())
		{
			screen.isOnActivePath[parent.GetValue()] = true;
			parent = screen.parents[parent.GetValue()];
		}
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
			
			bool hitItem = false;
			bool mouseOverChanged = false;

			for(int i = screen.rectangles.size() - 1; i >= 0; i--)
			{
				if(screen.visible[i] == false)
				{
					continue;
				}

				float minX = screen.rectangles[i].Left;
				float minY = screen.rectangles[i].Top;
				float maxX = screen.rectangles[i].Left + screen.rectangles[i].Width;
				float maxY = screen.rectangles[i].Top + screen.rectangles[i].Height;


				if(cursorX < maxX &&
					cursorY < maxY &&
					cursorX > minX &&
					cursorY > minY)
				{
					hitItem = true;
					if(g_leftButtonClicked)
					{
						DoMouseClick(i);
					}
					else if(mouseOverItem != i)
					{
						if(mouseOverItem != -1)
						{
							DoEndMouseOver(mouseOverItem);
						}
						DoStartMouseOver(i);
						mouseOverItem = i;
						mouseOverChanged = true;
					}
					break;
				}
			}
			int item = mouseOverItem;
			if(hitItem == false && mouseOverItem != -1)
			{
				DoEndMouseOver(mouseOverItem);
				mouseOverItem = -1;
				mouseOverChanged = true;
			}
			if(mouseOverChanged == true)
			{
				for(int i = 0; i < screen.expanded.size(); i++)
				{
					if(screen.elementTypes[i] == UIElementType::MenuButton)
					{
						bool subExpansion = false;
						for(int j = 0; j < screen.children[i].size(); j++)
						{
							int childId = screen.children[i][j].GetValue();
							if(screen.isOnActivePath[childId] == true)
							{
								subExpansion = true;
							}
						}
						screen.expanded[i] = subExpansion;
					}
				}
			}
			UpdateLayout(activeScreen);
		}
	}

	UIElementHandle AddItem(UIScreenHandle screenHandle, 
		Utils::FloatRect screenPosition, 
		uInt32 depth, 
		UIElementType::Values type, 
		std::u16string text,
		Renderer::Colours::Values colour,
		bool expanded,
		bool visible,
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
		screen.visible.insert(screen.visible.begin() + id, visible);
		screen.expanded.insert(screen.expanded.begin() + id, expanded);
		screen.itemColours.insert(screen.itemColours.begin() + id, colour);
		screen.isOnActivePath.insert(screen.isOnActivePath.begin() + id, false);

		if(parent != UIElementHandle::Invalid())
		{
			// No need for safety check.  If the parent handle
			// is valid then it's Children vector was created when
			// the handle was returned.
			screen.children[parent.GetValue()].push_back(handle);
			screen.rectangles[id].Left += screen.rectangles[parent.GetValue()].Left;
			screen.rectangles[id].Top += screen.rectangles[parent.GetValue()].Top;
		}
		else
		{
			screen.rootElements.push_back(handle);
		}

		return handle;
	}

	UIScreenHandle CreateScreen()
	{
		UIScreenHandle handle(screens.size());
		screens.push_back(Screen());
		
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
		UpdateLayout(screenHandle);

		screen.renderHandle = Renderer::CreateUIScreen(screen.rectangles, screen.elementTypes, screen.textStrings, screen.itemColours, screen.visible);
	}
}