namespace Editor
{
	UI::UIScreenHandle editorMainScreen;
	UI::UIElementHandle mainMenu;
	UI::UIElementHandle fileButton;
	UI::UIElementHandle fileButtonText;
	UI::UIElementHandle fileMenu;
	UI::UIElementHandle saveButton;

	/* Forward Declarations */
	void BuildUI();

	void Init()
	{
		BuildUI();
	}

	void BuildUI()
	{
		editorMainScreen = UI::CreateScreen(Engine::ScreenRect);

		Utils::FloatRect mainMenuRect = { /*left*/ 0, /*top*/ 0, /*right*/ Engine::ScreenRect.Width, /*bottom*/ 20 };
		mainMenu = UI::AddItem(editorMainScreen, mainMenuRect, 0, UI::UIElementType::Label, std::u16string());

		Utils::FloatRect fileButtonRect = {/*left*/ 0, /*top*/ 0, /*right*/ 128, /*bottom*/ 22 };
		fileButton = UI::AddItem(editorMainScreen, fileButtonRect, 1, UI::UIElementType::MenuButton, Platform::WideStringToUtf16(L"File"), mainMenu);

		Utils::FloatRect fileMenuRect = {/*left*/ 0, /*top*/ 22, /*right*/ 120, /*bottom*/ 100 };
		fileMenu = UI::AddItem(editorMainScreen, fileMenuRect, 1, UI::UIElementType::Label, std::u16string(), mainMenu);

		Utils::FloatRect saveButtonRect = {/*left*/ 0, /*top*/ 60, /*right*/ 128, /*bottom*/ 20 };
		saveButton = UI::AddItem(editorMainScreen, saveButtonRect, 2, UI::UIElementType::MenuButton, std::u16string(), fileMenu);

		UI::ScreenUpdateComplete(editorMainScreen);
		UI::ActivateScreen(editorMainScreen);
	}
}