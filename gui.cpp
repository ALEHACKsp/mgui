#include "gui.h"

namespace mgui
{
	DrawList::DrawList()
		:
		fontMap(nullptr)
	{
		dBuffer.reserve(1000);
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

	void DrawList::filledRect(Vector2 position, Vector2 size, Color color)
	{
		auto writePos = vBuffer.size();
		if (vBuffer.size() + 4 > vBuffer.capacity())
		{
			vBuffer.reserve(vBuffer.capacity() + 5000);
			dBuffer.reserve(dBuffer.capacity() + 2000);
		}

		static Descriptor d = { nullptr, 4, 2 };
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

	void DrawList::text(std::wstring text, Vector2 position, Color color)
	{
		auto writePos = vBuffer.size();
		if (vBuffer.size() + (text.length() * 20) > vBuffer.capacity())
		{
			vBuffer.reserve(vBuffer.capacity() + 5000);
			dBuffer.reserve(dBuffer.capacity() + 2000);
		}

		Descriptor d = { fontMap, 4, 2 };

		position -= Vector2(0.5f, 0.5f);
		for (auto& c : text)
		{
			auto ci = font->getCharInfo(L"Arial", c, 11);

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

	DrawData DrawList::getDrawData()
	{
		DrawData data = { 0 };
		data.descriptorList = dBuffer.data();
		data.descriptorCount = static_cast<uint32_t>(dBuffer.size());
		data.vertexList = vBuffer.data();
		data.vertexCount = static_cast<uint32_t>(vBuffer.size());
		return data;
	}

	Gui::Gui()
		:
		drawList(new DrawList())
	{
		// TODO: Gui constructor

		// Apply default style
	}

	void Gui::beginFrame()
	{
		drawList->beginFrame();
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
			// Window hasn't been created yet so let's do that.
			window = new Window(title);

			window->size = size;
			window->pos = startPosition;

			windows.emplace_back(std::move(window));
		}

		// TODO: Add title as a label to window

		// TODO: Handle dragging and resizing

		// TODO: Push new bounds onto stack (seperate stack for min and max?)

		// TODO: Render window 

		drawList->filledRect(window->pos, window->size, Color(20, 20, 20));
		drawList->text(title, window->pos + Vector2(10, 10), Color(255, 255, 255));
	}

	void Gui::endWindow()
	{
	}

	bool Gui::button()
	{
		/*
		Get window, id and calculate label text
		calculate total size of button, taking into consideration padding in the style
		work out if button is pressed, held or hovered
			( pressed when it wasn't pressed and now is)
			( held when was pressed and still is)
			( hovered for as long as mouse is within bounds) 

				^-> Maybe move this to a seperate function as a lot of widgets will make use of this behaviour?

		render button with correct colour (get lighter as we go from default -> hovered -> pressed/held)
		return true if we go from pressed/held -> hovered otherwise false
		*/

		return false;
	}

	DrawData Gui::getDrawData()
	{
		return drawList->getDrawData();
	}

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
		}
		else
		{
			drawList->font->setupFont(fontName, pt);
		}
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