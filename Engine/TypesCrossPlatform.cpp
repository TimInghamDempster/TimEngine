namespace UI
{
	namespace UIElementType
	{
		enum Values
		{
			Rectangle,
			Text
		};
	}

	struct UIElementTag{};
	typedef Utils::Handle<UIElementTag, int32, -1> UIElementHandle;

	struct UIScreenTag{};
	typedef Utils::Handle<UIScreenTag, int32, -1> UIScreenHandle;
}

namespace Engine
{
	struct Point2d
	{
		int32 x;
		int32 y;
	};
}

namespace Renderer
{
	struct UIScreenRenderTag{};
	typedef Utils::Handle<UIScreenRenderTag, int32, -1> UIScreenRenderHandle;
}
