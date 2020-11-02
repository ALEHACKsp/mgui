#include "gui.h"

namespace mgui
{
	DrawList::DrawList()
		:
		fontMap(nullptr)
	{
		dBuffer.reserve(2000);
		vBuffer.reserve(5000);
	}

	void DrawList::beginFrame()
	{
		dBuffer.clear();
		vBuffer.clear();
	}

	void DrawList::endFrame()
	{

	}

	void DrawList::filledRect(Vector2 position, Vector2 size, Color color, Vector4 clippingRect)
	{
		if (vBuffer.size() + 4 > vBuffer.capacity())
		{
			vBuffer.reserve(vBuffer.capacity() + 5000);
			dBuffer.reserve(dBuffer.capacity() + 2000);
		}

		Descriptor d = { nullptr, 4, 2, clippingRect };
		Vertex vertices[4] =
		{
			{position.x, position.y + size.y, 0.f, color, 0.f, 0.f},
			{position.x, position.y, 0.f, color, 0.f, 0.f},
			{position.x + size.x, position.y + size.y, 0.f, color, 0.f, 0.f},
			{position.x + size.x, position.y, 0.f, color, 0.f, 0.f}
		};
		
		vBuffer.insert(vBuffer.end(), vertices, vertices + 4);
		dBuffer.emplace_back(d);
	}

	void DrawList::text(std::wstring text, Vector2 position, Color color, Vector4 clippingRect)
	{
		if (vBuffer.size() + (text.length() * 20) > vBuffer.capacity())
		{
			vBuffer.reserve(vBuffer.capacity() + 5000);
			dBuffer.reserve(dBuffer.capacity() + 2000);
		}

		Descriptor d = { fontMap, 4, 2, clippingRect };

		position -= Vector2(0.5f, 0.5f);
		for (auto& c : text)
		{
			auto ci = font->getCharInfo(fontStack.top().first, c, fontStack.top().second);

			if (!ci)
			{
				continue;
			}

			position.x -= ci->spacing;

			Vertex vertices[4] =
			{
				{position.x, position.y + ci->h, 0.f, color, ci->u0, ci->v1},
				{position.x, position.y, 0.f, color, ci->u0, ci->v0 },
				{position.x + ci->w, position.y + ci->h, 0.f, color, ci->u1, ci->v1},
				{position.x + ci->w, position.y, 0.f, color, ci->u1, ci->v0 }
			};

			vBuffer.insert(vBuffer.end(), vertices, vertices + 4);
			dBuffer.emplace_back(d);

			position.x += ci->w + (2.f * ci->spacing);
		}
	}
	
	//
	//
	//		GUI
	//
	//

	Gui::Gui()
		:
		drawList(new DrawList()),
		style(new Style())
	{
		setStyleDark();
	}

	void Gui::setStyleDark()
	{
		style->colors[window_bg]			= Color(60, 60, 60);
		style->colors[window_title_bg]		= Color(80, 80, 80);
		style->colors[window_title_fg]		= Color(255, 255, 255);
		style->colors[button_bg_default]	= Color(120, 120, 120);
		style->colors[button_bg_hovered]	= Color(140, 140, 140);
		style->colors[button_bg_held]		= Color(160, 160, 160);
		style->colors[button_fg_default]	= Color(255, 255, 255);
		style->colors[button_fg_hovered]	= Color(255, 255, 255);
		style->colors[button_fg_held]		= Color(255, 255, 255);
	}

	void Gui::beginFrame()
	{
		drawList->beginFrame();
		boundsStack.clear();
	}

	void Gui::endFrame()
	{
		drawList->endFrame();
	}

	void Gui::beginWindow(std::wstring title, Vector2 size, Vector2 startPosition, uint32_t flags)
	{
		auto window = getWindowByTitle(title);
		if (!window)
		{
			window = new Window(title);

			window->size = size;
			window->pos = startPosition;

			windows.emplace_back(std::move(window));
		}

		// TODO: Handle dragging and resizing

		Vector4 bounds =
		{
			window->pos.x,
			window->pos.y,
			window->pos.x + window->size.x,
			window->pos.y + window->size.y
		};

		drawList->filledRect(window->pos, window->size, style->colors[window_bg], bounds);
		drawList->filledRect(window->pos, Vector2(window->size.x, 20), style->colors[window_title_bg], bounds);
		drawList->text(title, window->pos + Vector2(5, 0), style->colors[window_title_fg], bounds);

		bounds += {10, 30, -10, -10};

		boundsStack.push_back(bounds);
	}

	void Gui::endWindow()
	{
	}

	//
	//
	//		Widgets
	//
	//

	bool Gui::button(std::wstring label)
	{
		bool hovered;
		bool pressed;
		bool held;

		// TODO: Button behaviour
		hovered=pressed=held=false;

		Color bg;
		Color fg;


		if (pressed || held)
		{
			bg = style->colors[button_bg_held];
			fg = style->colors[button_fg_held];
		}
		else if (hovered)
		{
			bg = style->colors[button_bg_hovered];
			fg = style->colors[button_fg_hovered];
		}
		else
		{
			bg = style->colors[button_bg_default];
			fg = style->colors[button_fg_default];
		}
		
		auto& bounds = boundsStack.back();

		drawList->filledRect(bounds.topLeft(), Vector2(bounds.right - bounds.left, 20), bg, bounds);
		drawList->text(label, bounds.topLeft() + Vector2(10, 0), fg, bounds);


		bounds.top += 30;

		return pressed;
	}

	//
	//
	//		Font
	//
	//

	const uint32_t* Gui::getFontData()
	{
		if (drawList->font->updated)
		{
			drawList->font->updated = false;
			return drawList->font->getFontMap();
		}
		return nullptr;
	}

	void Gui::setFontTexture(void* texture)
	{
		drawList->fontMap = texture;
	}

	void Gui::addFont(std::wstring fontName, uint32_t pt)
	{
		if (!drawList->font)
		{
			drawList->font = std::unique_ptr<Font>(new Font(fontName, pt));
			drawList->fontStack.push(std::make_pair(fontName, pt));
		}
		else
		{
			drawList->font->setupFont(fontName, pt);
		}
	}

	void Gui::pushFont(std::wstring fontName, uint32_t pt)
	{
		drawList->fontStack.push(std::make_pair(fontName, pt));
	}

	void Gui::popFont()
	{
		if (drawList->fontStack.size() > 1)
		{
			drawList->fontStack.pop();
		}
	}

	//
	//
	//		Misc
	//
	//

	DrawData Gui::getDrawData()
	{
		DrawData data = { 0 };
		data.descriptorList = drawList->dBuffer.data();
		data.descriptorCount = static_cast<uint32_t>(drawList->dBuffer.size());
		data.vertexList = drawList->vBuffer.data();
		data.vertexCount = static_cast<uint32_t>(drawList->vBuffer.size());
		return data;
	}

	Window* Gui::getWindowByTitle(std::wstring title)
	{
		for (auto& w : windows)
		{
			if (title.compare(w->title) == 0)
			{
				return w.get();
			}
		}
		return nullptr;
	}

}