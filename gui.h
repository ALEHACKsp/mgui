#pragma once

#include <string>
#include <memory>
#include <vector>
#include <stack>

#include "structs.h"
#include "font.h"

namespace mgui
{
	enum styleColors : uint32_t
	{
		window_bg,
		window_title_bg,
		window_title_fg,
		button_bg_default,
		button_bg_hovered,
		button_bg_held,
		button_fg_default,
		button_fg_hovered,
		button_fg_held
	};

	class Style
	{
	public:

		Color colors[64];

	};

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

		void filledRect(Vector2 position, Vector2 size, Color color, Vector4 clippingRect);

		void text(std::wstring text, Vector2 position, Color color, Vector4 clippingRect); // TODO: font flags (Centering, shadow etc)

	private:

		std::unique_ptr<Font> font;
		void* fontMap;

		std::stack<std::pair<std::wstring, uint32_t>> fontStack;

		std::vector<Descriptor> dBuffer;
		std::vector<Vertex> vBuffer;
	};

	class Gui
	{
	public:

		Gui();

	public:

		void setStyleDark();

		void beginFrame();

		void endFrame();

		void beginWindow(std::wstring title, Vector2 size, Vector2 startPosition, uint32_t flags); // TODO: window flags such as no resize, no move etc

		void endWindow(); 


	public:

		// TODO: Widgets

		// Required args: text, ?
		bool button(std::wstring label);


	public:

		const uint32_t* getFontData();

		void setFontTexture(void* texture);

		void addFont(std::wstring fontName, uint32_t pt);

		void pushFont(std::wstring fontName, uint32_t pt);

		void popFont();

	public:

		DrawData getDrawData();

	private:

		// TODO: any helper functions

		Window* getWindowByTitle(std::wstring title);



	private:

		std::unique_ptr<DrawList> drawList;

		std::vector<std::unique_ptr<Window>> windows;


		std::vector<Vector4> boundsStack;

		// TODO: gui state: mouse pos, buttons down, etc etc

		std::unique_ptr<Style> style;
	};

}