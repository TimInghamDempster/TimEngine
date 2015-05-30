namespace UI
{
	namespace UIElementType
	{
		enum Values
		{
			ScreenRoot,
			Label,
			Button,
			MenuButton
		};
	}

	struct UIElementTag{};
	typedef Engine::Handle<UIElementTag, int32, -1> UIElementHandle;

	struct UIScreenTag{};
	typedef Engine::Handle<UIScreenTag, int32, -1> UIScreenHandle;
}

namespace Engine
{
	struct GameEntityTag{};
	typedef Engine::Handle<GameEntityTag, int32, -1> GameEntityHandle;

	struct Point2d
	{
		int32 x;
		int32 y;
	};
}

namespace Renderer
{
	struct UIScreenRenderTag{};
	typedef Engine::Handle<UIScreenRenderTag, int32, -1> UIScreenRenderHandle;
}
