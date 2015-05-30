namespace Editor
{
		UI::UIScreenHandle editorMainScreen;

		Engine::World world;

		void AddEntityToWorld(UI::UIElementHandle button)
		{
		}

		void Quit(UI::UIElementHandle button)
		{
		}

		void BuildUI()
		{
			editorMainScreen = UI::CreateScreen();

			Renderer::Colours::Values background = Renderer::Colours::Grey;
			Renderer::Colours::Values highlight = Renderer::Colours::LightGrey;

			Utils::FloatRect mainMenuRect = { /*left*/ 0, /*top*/ 0, /*right*/ Engine::ScreenRect.Width, /*bottom*/ 20 };
			UI::UIElementHandle mainMenu = UI::AddItem(editorMainScreen, mainMenuRect, 0, UI::UIElementType::Label, std::u16string(), background, background, true, true);

			Utils::FloatRect fileButtonRect = {/*left*/ 0, /*top*/ 0, /*right*/ 128, /*bottom*/ 20 };
			UI::UIElementHandle fileButton = UI::AddItem(editorMainScreen, fileButtonRect, 1, UI::UIElementType::MenuButton, Platform::WideStringToUtf16(L"File"), background, highlight, false, true, mainMenu);

			Utils::FloatRect fileMenuRect = {/*left*/ 0, /*top*/ 19, /*right*/ 120, /*bottom*/ 41 };
			UI::UIElementHandle fileMenu = UI::AddItem(editorMainScreen, fileMenuRect, 1, UI::UIElementType::Label, std::u16string(), background, background, true, true, fileButton);

			Utils::FloatRect newEntityButtonRect = {/*left*/ 0, /*top*/ 0, /*right*/ 120, /*bottom*/ 20 };
			UI::UIElementHandle newEntityButton = UI::AddItem(editorMainScreen, newEntityButtonRect, 3, UI::UIElementType::Button, Platform::WideStringToUtf16(L"Add Entity"), background, highlight, true, true, fileMenu, &AddEntityToWorld);

			Utils::FloatRect quitButtonRect = {/*left*/ 0, /*top*/ 20, /*right*/ 120, /*bottom*/ 20 };
			UI::UIElementHandle quitButton = UI::AddItem(editorMainScreen, quitButtonRect, 3, UI::UIElementType::Button, Platform::WideStringToUtf16(L"Quit"), background, highlight, true, true, fileMenu, &Quit);

			UI::ScreenUpdateComplete(editorMainScreen);
			UI::ActivateScreen(editorMainScreen);
		}
		void Init()
		{
			BuildUI();
		}
}