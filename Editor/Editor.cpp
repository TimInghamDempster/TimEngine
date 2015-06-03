namespace Editor
{
		UI::UIScreenHandle editorMainScreen;

		Engine::World world;

		void LoadWorld(UI::UIElementHandle button)
		{
			std::string filename;

			bool fileSelected = Platform::OpenFileDialog(filename);

			if(fileSelected)
			{
				world.LoadWorld(filename);
			}
		}

		void Quit(UI::UIElementHandle button)
		{
			Engine::shouldQuit = true;
		}

		void BuildUI()
		{
			editorMainScreen = UI::CreateScreen();

			Renderer::Colours::Values background = Renderer::Colours::Grey;
			Renderer::Colours::Values highlight = Renderer::Colours::LightGrey;

			Utils::FloatRect mainMenuRect = { /*left*/ 0, /*top*/ 0, /*right*/ Engine::ScreenRect.Width, /*bottom*/ 20 };
			UI::UIElementHandle mainMenu = UI::AddItem(editorMainScreen, mainMenuRect, 0, UI::UIElementType::Label, std::string(), background, background, true, true);

			Utils::FloatRect fileButtonRect = {/*left*/ 0, /*top*/ 0, /*right*/ 128, /*bottom*/ 20 };
			UI::UIElementHandle fileButton = UI::AddItem(editorMainScreen, fileButtonRect, 1, UI::UIElementType::MenuButton, "File", background, highlight, false, true, mainMenu);

			Utils::FloatRect fileMenuRect = {/*left*/ 0, /*top*/ 19, /*right*/ 120, /*bottom*/ 41 };
			UI::UIElementHandle fileMenu = UI::AddItem(editorMainScreen, fileMenuRect, 1, UI::UIElementType::Label, std::string(), background, background, true, true, fileButton);

			Utils::FloatRect loadWorldButtonRect = {/*left*/ 0, /*top*/ 0, /*right*/ 120, /*bottom*/ 20 };
			UI::UIElementHandle loadWorldButton = UI::AddItem(editorMainScreen, loadWorldButtonRect, 3, UI::UIElementType::Button, "Load World", background, highlight, true, true, fileMenu, &LoadWorld);

			Utils::FloatRect quitButtonRect = {/*left*/ 0, /*top*/ 20, /*right*/ 120, /*bottom*/ 20 };
			UI::UIElementHandle quitButton = UI::AddItem(editorMainScreen, quitButtonRect, 3, UI::UIElementType::Button, "Quit", background, highlight, true, true, fileMenu, &Quit);

			UI::ScreenUpdateComplete(editorMainScreen);
			UI::ActivateScreen(editorMainScreen);
		}
		void Init()
		{
			BuildUI();
		}
}