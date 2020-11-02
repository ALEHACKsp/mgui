#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <tuple>

#include "structs.h"

namespace mgui
{

	class Font
	{
	private:

		typedef struct _Node
		{
			float x0, y0;
			float x1, y1;
			float w, h;
			bool filled;
			std::shared_ptr<_Node> left, right;

			_Node() : x0(0), y0(0), x1(0), y1(0), w(0), h(0), filled(false), left(nullptr), right(nullptr) {}

			const _Node* insert(float width, float height);
		} Node;

		typedef struct _CharInfoHasher
		{
			std::size_t operator()(const std::tuple<std::wstring, wchar_t, uint32_t>& k) const
			{
				size_t r = 17;
				r *= 31 + std::hash<std::wstring>()(std::get<0>(k));
				r *= 31 + std::hash<wchar_t>()(std::get<1>(k));
				r *= 31 + std::hash<uint32_t>()(std::get<2>(k));
				return r;
			}
		} CharInfoHasher;

	public:

		Font(std::wstring name, uint32_t pt);

	public:

		CharInfo* getCharInfo(std::wstring fontName, wchar_t c, uint32_t pt);

		bool setupFont(std::wstring fontName, uint32_t pt);

		const uint32_t* getFontMap();

		bool updated;
	private:

		CharInfo* addChar(std::wstring fontName, wchar_t c, uint32_t pt);

		CharInfo* addChar(HDC hdc, wchar_t c, int px);

	private:
		std::unordered_map<std::tuple<std::wstring, wchar_t, uint32_t>, std::shared_ptr<CharInfo>, CharInfoHasher> charMap;

		std::shared_ptr<Node> root;

		std::shared_ptr<uint32_t[]> bitmap;


	};

}