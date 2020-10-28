#include "gui.h"

namespace mgui
{
	DrawList::DrawList()
	{
		dBuffer.reserve(1000);
		vBuffer.reserve(5000);
	}

	void DrawList::preFrame()
	{
		dBuffer.clear();
		vBuffer.clear();
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

	void Gui::begin(std::wstring title, Vector2 size, Vector2 startPosition, uint32_t flags)
	{
		/*
		Check if window already created
			If it has, use the position from there instead of startPosition
			If it hasn't, create a window struct and add pos, size, title etc
		calculate size of label
		handle drag for moving of window
		handle dragging on edges for resizing of window

		after all that render a box of size from pos, add label in top left
		*/

		Window* window = GetWindowByTitle(title);
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
	}

	void Gui::end()
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

	Window* Gui::GetWindowByTitle(std::wstring title)
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