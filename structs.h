#pragma once

namespace mgui
{
	typedef struct _Vector2
	{
		float x, y;

		// TODO: Sort out math operators for vector 2

	} Vector2;

	// TODO: Define any other global structs here



	typedef struct _Window
	{
		std::wstring title;
		Vector2 pos, size;
		uint64_t id = std::hash<std::wstring> {}(L"a");
		// TODO: need a way to generate a unique id for widgets added to windows that remains constant between frames
	} Window;

}

