#pragma once

#include <string>
#include <memory>
#include <vector>

#include "structs.h"

namespace mgui
{
	class DrawList;

	class Gui
	{
	public:

		Gui();

	public:

		void begin(std::wstring title, Vector2 size, Vector2 startPosition, uint32_t flags); // TODO: window flags such as no resize, no move etc

		void end(); 

	public:

		// TODO: Widgets

		// Required args: text, ?
		bool button();


	private:

		// TODO: any helper functions

		Window* GetWindowByTitle(std::wstring title);

	private:

		// TODO: drawlist class -> allow getting of draw data from Gui so we can call render on the renderer
		std::unique_ptr<DrawList> drawList;

		std::vector<std::unique_ptr<Window>> windows;

		// TODO: gui state: mouse pos, buttons down, etc etc

		// TODO: Need a style class or struct to manage styles
	};

}