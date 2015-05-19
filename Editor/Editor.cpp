namespace Editor
{
	UI::UIScreenHandle editorMainScreen;
	UI::UIElementHandle mainMenu;
	UI::UIElementHandle fileButton;
	UI::UIElementHandle fileButtonText;
	UI::UIElementHandle fileMenu;
	UI::UIElementHandle importButton;
	UI::UIElementHandle importMeshButton;

	/* Forward Declarations */
	void BuildUI();

	void Init()
	{
		BuildUI();
	}

	void ImportMesh(UI::UIElementHandle button)
	{
		//Do mesh import stuff here.  Or put this function somewhere useful.
	}

	void BuildUI()
	{
		editorMainScreen = UI::CreateScreen();

		Renderer::Colours::Values background = Renderer::Colours::Grey;
		Renderer::Colours::Values highlight = Renderer::Colours::LightGrey;

		Utils::FloatRect mainMenuRect = { /*left*/ 0, /*top*/ 0, /*right*/ Engine::ScreenRect.Width, /*bottom*/ 20 };
		mainMenu = UI::AddItem(editorMainScreen, mainMenuRect, 0, UI::UIElementType::Label, std::u16string(), background, background, true, true);

		Utils::FloatRect fileButtonRect = {/*left*/ 0, /*top*/ 0, /*right*/ 128, /*bottom*/ 22 };
		fileButton = UI::AddItem(editorMainScreen, fileButtonRect, 1, UI::UIElementType::MenuButton, Platform::WideStringToUtf16(L"File"), background, highlight, false, true, mainMenu);

		Utils::FloatRect fileMenuRect = {/*left*/ 0, /*top*/ 20, /*right*/ 120, /*bottom*/ 100 };
		fileMenu = UI::AddItem(editorMainScreen, fileMenuRect, 1, UI::UIElementType::Label, std::u16string(), background, background, true, true, fileButton);

		Utils::FloatRect importButtonRect = {/*left*/ 0, /*top*/ 60, /*right*/ 128, /*bottom*/ 20 };
		importButton = UI::AddItem(editorMainScreen, importButtonRect, 2, UI::UIElementType::MenuButton, Platform::WideStringToUtf16(L"Import..."), background, highlight, true, true, fileMenu);

		Utils::FloatRect importMeshButtonRect = {/*left*/ 127, /*top*/ 0, /*right*/ 128, /*bottom*/ 20 };
		importMeshButton = UI::AddItem(editorMainScreen, importMeshButtonRect, 3, UI::UIElementType::Button, Platform::WideStringToUtf16(L"Mesh"), background, highlight, true, true, importButton, &ImportMesh);

		UI::ScreenUpdateComplete(editorMainScreen);
		UI::ActivateScreen(editorMainScreen);
	}
}