#pragma once

#include <d3d9.h>
#include <unordered_set>
#include <memory>

#include "..\structs.h"

namespace mgui
{
	class Font
	{
	public:

		Font(std::wstring fontName, int height); // Only need to take font name I think, just add glyphs to texture seperately

	public:

		// TODO: Get texcoords of char at specific height - if the char doesn't exist, add it to texture
		Vector2 getCharUV(wchar_t c, int h);

	private:

		LPDIRECT3DTEXTURE9 texture;

		// TODO: Char lookup table

		// TODO: Need a tree to represent free space in the texture


	};

	class Renderer
	{
	public:

		Renderer(LPDIRECT3DDEVICE9 device);

	public:

		void render(); // TODO: Fill in args when I've decided on a draw list implementation

		void release(); // TODO: cleanup of fonts, vertex buffer etc

	public:

		bool addFont(std::wstring fontName, int height = 13);

	private:

		LPDIRECT3DDEVICE9 device;

		// Use set to prevent font being created twice (could also use map and just have a check in addfont?)
		std::unordered_set<std::wstring, std::unique_ptr<Font>> fonts;

		// TODO: internal vertex buffer, render states etc
	};




}