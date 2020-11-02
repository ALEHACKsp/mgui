#include "font.h"

#define TEX_SIZE 4096

namespace mgui
{
	const Font::_Node* Font::_Node::insert(float width, float height)
	{
		if (!left && !right)
		{
			if (filled)
			{
				return nullptr;
			}

			if (w < width || h < height)
			{
				return nullptr;
			}

			if (w == width && h == height)
			{
				filled = true;
				return this;
			}

			float dw = w - width;
			float dh = h - height;

			if (dw > dh)
			{
				auto child = std::shared_ptr<Font::_Node>(new Font::_Node());

				child->x0 = x0;
				child->y0 = y0;
				child->x1 = x0 + width - 1;
				child->y1 = y1;
				child->w = width;
				child->h = h;
				left = child;

				child = std::shared_ptr<Font::_Node>(new Font::_Node());

				child->x0 = x0 + width;
				child->y0 = y0;
				child->x1 = x1;
				child->y1 = y1;
				child->w = dw;
				child->h = h;
				right = child;
			}
			else
			{
				auto child = std::shared_ptr<Font::_Node>(new Font::_Node());

				child->x0 = x0;
				child->y0 = y0;
				child->x1 = x1;
				child->y1 = y0 + height - 1;
				child->w = w;
				child->h = height;
				left = child;

				child = std::shared_ptr<Font::_Node>(new Font::_Node());

				child->x0 = x0;
				child->y0 = y0 + height;
				child->x1 = x1;
				child->y1 = y1;
				child->w = w;
				child->h = dh;
				right = child;
			}

			return left->insert(width, height);
		}
		else
		{
			auto node = left->insert(width, height);
			return node ? node : right->insert(width, height);
		}
	}

	Font::Font(std::wstring fontName, uint32_t pt)
	{
		root = std::shared_ptr<Node>(new Node());
		root->w = TEX_SIZE;
		root->h = TEX_SIZE;
		root->x1 = TEX_SIZE - 1;
		root->y1 = TEX_SIZE - 1;

		bitmap = std::shared_ptr<uint32_t[]>(new uint32_t[TEX_SIZE * TEX_SIZE]);
		memset(bitmap.get(), 0, sizeof(uint32_t) * TEX_SIZE * TEX_SIZE);

		if (!setupFont(fontName, pt))
		{
			wprintf_s(L"[-] Font: Failed to setup font %s\n", fontName.c_str());
		}
	}

	CharInfo* Font::getCharInfo(std::wstring fontName, wchar_t c, uint32_t pt)
	{
		CharInfo* ci = nullptr;
		try
		{
			 ci = charMap.at(std::make_tuple(fontName, c, pt)).get();
		}
		catch (...)
		{
			ci = addChar(fontName, c, pt);
			updated = true;
		}

		return ci;
	}

	bool Font::setupFont(std::wstring fontName, uint32_t pt)
	{
		auto hdc = CreateCompatibleDC(nullptr);
		SetMapMode(hdc, MM_TEXT);

		auto px = -MulDiv(pt, GetDeviceCaps(hdc, LOGPIXELSY), 72);

		auto font = CreateFontW(px, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH || FF_DONTCARE, fontName.c_str());

		if (!font)
		{
			wprintf_s(L"[-] Font: Failed to create font %s (%ulpt)", fontName.c_str(), pt);
			return false;
		}

		auto oFont = SelectObject(hdc, font);

		for (wchar_t c = 32; c < 127; ++c)
		{
			auto ci = addChar(hdc, c, px);

			if (!ci)
			{
				wprintf_s(L"[-] Font: Failed to add char %c to font map\n", c);
				return false;
			}

			charMap.emplace(std::make_tuple(fontName, c, pt), std::shared_ptr<CharInfo>(ci));
		}

		SelectObject(hdc, oFont);

		DeleteObject(font);
		DeleteDC(hdc);

		updated = true;
		return true;
	}

	const uint32_t* Font::getFontMap()
	{
		return const_cast<const uint32_t*>(bitmap.get());
	}

	CharInfo* Font::addChar(std::wstring fontName, wchar_t c, uint32_t pt)
	{
		auto hdc = CreateCompatibleDC(nullptr);
		SetMapMode(hdc, MM_TEXT);

		auto px = -MulDiv(pt, GetDeviceCaps(hdc, LOGPIXELSY), 72);

		auto font = CreateFontW(px, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH || FF_DONTCARE, fontName.c_str());

		if (!font)
		{
			wprintf_s(L"[-] Font: Failed to create font %s (%ulpt)", fontName.c_str(), pt);
			return nullptr;
		}

		auto oFont = SelectObject(hdc, font);

		auto ci = addChar(hdc, c, px);

		charMap.emplace(std::make_tuple(fontName, c, pt), std::shared_ptr<CharInfo>(ci));

		SelectObject(hdc, oFont);

		DeleteObject(font);
		DeleteDC(hdc);

		return ci;
	}

	CharInfo* Font::addChar(HDC hdc, wchar_t c, int px)
	{
		auto ci = new CharInfo();
		
		SIZE s = { 0 };

		if (!GetTextExtentPoint32W(hdc, &c, 1, &s))
		{
			return nullptr;
		}

		ci->w = static_cast<float>(s.cx);
		ci->h = static_cast<float>(s.cy);

		auto node = root->insert(ci->w, ci->h);

		if (!node)
		{
			wprintf_s(L"[-] Font: No space in font map for %c\n", c);
			return nullptr;
		}

		ci->u0 = node->x0 / (float)TEX_SIZE;
		ci->u1 = (node->x0 + ci->w) / (float)TEX_SIZE;
		ci->v0 = node->y0 / (float)TEX_SIZE;
		ci->v1 = (node->y0 + ci->h) / (float)TEX_SIZE;
		ci->spacing = ceil(s.cy * 0.01f);

		BITMAPINFO bmi;
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = s.cx;
		bmi.bmiHeader.biHeight = -s.cy;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;

		uint32_t* bmpBits;

		auto bmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&bmpBits, nullptr, 0);

		if (!bmp)
		{
			return nullptr;
		}

		auto oBmp = SelectObject(hdc, bmp);

		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkColor(hdc, 0);
		SetTextAlign(hdc, TA_TOP);

		ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, nullptr, &c, 1, nullptr);

		uint32_t* dstRow = &bitmap[TEX_SIZE * (uint64_t)node->y0 + (uint64_t)node->x0];

		for (auto y = 0; y < s.cy; ++y)
		{
			auto dst = dstRow;
			for (auto x = 0; x < s.cx; ++x)
			{
				auto c = (bmpBits[s.cx * y + x] & 0xFF) >> 4;
				if (c > 4)
				{
					*dst++ = 0x00FFFFFF | (c << 28) | (c << 24);
				}
				else
				{
					*dst++ = 0;
				}
			}
			dstRow += TEX_SIZE;
		}

		SelectObject(hdc, oBmp);
		DeleteObject(bmp);
		
		return ci;
	}

}