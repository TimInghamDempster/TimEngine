namespace Editor
{
	UI::UIHandle mainMenu;
	UI::UIHandle fileButton;
	UI::UIHandle fileMenu;
	UI::UIHandle saveButton;

	/* Forward Declarations */
	void BuildUI();

	void Init()
	{
		BuildUI();
	}

	void BuildUI()
	{
		Utils::Rect mainMenuRect = { /*top*/ 0, /*left*/ 0, /*bottom*/ 10, /*right*/ Engine::ScreenRect.Width };
		mainMenu = UI::AddItem(mainMenuRect, 0);

		Utils::Rect fileButtonRect = {/*top*/ 0, /*left*/ 0, /*bottom*/ 10, /*right*/ 128 };
		fileButton = UI::AddItem(fileButtonRect, 1, mainMenu);

		Utils::Rect fileMenuRect = {/*top*/ 0, /*left*/ 0, /*bottom*/ 100, /*right*/ 128 };
		fileMenu = UI::AddItem(fileButtonRect, 1, mainMenu);

		Utils::Rect saveButtonRect = {/*top*/ 10, /*left*/ 0, /*bottom*/ 20, /*right*/ 128 };
		saveButton = UI::AddItem(saveButtonRect, 2, fileMenu);
	}
}