#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

#include "structs.h"
#include "font.h"

namespace mgui
{
	class DrawList
	{
		friend class Gui;
	public:

		DrawList();

	public:

		void beginFrame();

		void endFrame();

	public:

		// TODO: Draw commands

		void filledRect(Vector2 position, Vector2 size, Color color);

		void text(std::wstring text, Vector2 position, Color color); // TODO: font flags (Centering, shadow etc)

	public:

		DrawData getDrawData();

		std::unique_ptr<Font> font;
		void* fontMap;
	private:

		std::vector<Descriptor> dBuffer;
		std::vector<Vertex> vBuffer;
	};

	class Gui
	{
	public:

		Gui();

	public:

		void beginFrame();

		void endFrame();

		void beginWindow(std::wstring title, Vector2 size, Vector2 startPosition, uint32_t flags); // TODO: window flags such as no resize, no move etc

		void endWindow(); 

	public:

		// TODO: Widgets

		// Required args: text, ?
		bool button();


	public:

		DrawData getDrawData();

		const uint32_t* getFontData();

		void setFontTexture(void* texture);

		void addFont(std::wstring fontName, uint32_t pt);

	private:

		// TODO: any helper functions

		Window* getWindowByTitle(std::wstring title);

	private:

		std::unique_ptr<DrawList> drawList;

		std::vector<std::unique_ptr<Window>> windows;



		// TODO: gui state: mouse pos, buttons down, etc etc

		// TODO: Need a style class or struct to manage styles
	};

}