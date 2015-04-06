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
		Renderer::UpdateUITransformBuffer(UI::Rectangles);
	}

	void BuildUI()
	{
		Utils::FloatRect mainMenuRect = { /*left*/ 0, /*top*/ 0, /*right*/ Engine::ScreenRect.Width, /*bottom*/ 20 };
		mainMenu = UI::AddItem(mainMenuRect, 0);

		Utils::FloatRect fileButtonRect = {/*left*/ 0, /*top*/ 0, /*right*/ 128, /*bottom*/ 22 };
		fileButton = UI::AddItem(fileButtonRect, 1, mainMenu);

		Utils::FloatRect fileMenuRect = {/*left*/ 0, /*top*/ 0, /*right*/ 120, /*bottom*/ 100 };
		fileMenu = UI::AddItem(fileMenuRect, 1, mainMenu);

		Utils::FloatRect saveButtonRect = {/*left*/ 0, /*top*/ 60, /*right*/ 128, /*bottom*/ 20 };
		saveButton = UI::AddItem(saveButtonRect, 2, fileMenu);
	}
}