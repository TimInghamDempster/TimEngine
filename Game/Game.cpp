namespace Game
{	
	void BuildUI()
	{
		UI::UIScreenHandle mainMenu = UI::CreateScreen();

		Utils::FloatRect newRect = {200.0f, 200.0f, 300.0f, 20.0f };
		UI::AddItem(mainMenu, newRect, 1, UI::UIElementType::Button, "New Game", Renderer::Colours::LightGrey, Renderer::Colours::LightGrey, true, true);

		UI::ScreenUpdateComplete(mainMenu);
		UI::ActivateScreen(mainMenu);
	}

	void Init()
	{
		BuildUI();
	}
}