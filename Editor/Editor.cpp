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
		Utils::uRect mainMenuRect = { /*top*/ 0, /*left*/ 0, /*bottom*/ 10, /*right*/ 1024 };
		mainMenu = UI::AddItem(mainMenuRect);

		Utils::uRect fileButtonRect = {/*top*/ 0, /*left*/ 0, /*bottom*/ 10, /*right*/ 128 };
		fileButton = UI::AddItem(fileButtonRect, mainMenu);

		Utils::uRect fileMenuRect = {/*top*/ 0, /*left*/ 0, /*bottom*/ 100, /*right*/ 128 };
		fileMenu = UI::AddItem(fileButtonRect, mainMenu);

		Utils::uRect saveButtonRect = {/*top*/ 10, /*left*/ 0, /*bottom*/ 20, /*right*/ 128 };
		saveButton = UI::AddItem(saveButtonRect, fileMenu);
	}
}