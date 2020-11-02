#pragma once

namespace mgui
{

	typedef struct _Vector2
	{
		float x, y;

		_Vector2() : x(0), y(0) {}

		_Vector2(float x, float y) : x(x), y(y) {}

		_Vector2& operator+= (const _Vector2& other)
		{
			x += other.x;
			y += other.y;
			return *this;
		}

		_Vector2& operator-= (const _Vector2& other)
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}

		_Vector2 operator+ (const _Vector2 other)
		{
			return _Vector2(x + other.x, y + other.y);
		}

		_Vector2 operator- (const _Vector2 other)
		{
			return _Vector2(x - other.x, y - other.y);
		}

	} Vector2;

	typedef struct _Vector4
	{
		float left, top, right, bottom;

		_Vector4() : top(0), left(0), bottom(0), right(0) {}

		_Vector4(float left, float top, float right, float bottom) : left(left), top(top), right(right), bottom(bottom) {}

		_Vector4& operator+= (const _Vector4& other)
		{
			left += other.left;
			top += other.top;
			right += other.right;
			bottom += other.bottom;
			return *this;
		}

		_Vector4& operator-= (const _Vector4& other)
		{
			left -= other.left;
			top -= other.top;
			right -= other.right;
			bottom -= other.bottom;
			return *this;
		}
		
		_Vector4 operator+ (const _Vector4& other)
		{
			return _Vector4(left + other.left, top + other.top, right + other.right, bottom + other.bottom);
		}

		_Vector4 operator- (const _Vector4& other)
		{
			return _Vector4(left - other.left, top - other.top, right - other.right, bottom - other.bottom);
		}

		bool operator() ()
		{
			return left && top && right && bottom;
		}

		Vector2 topLeft()
		{
			return Vector2(left, top);
		}

		Vector2 bottomRight()
		{
			return Vector2(right, bottom);
		}

	} Vector4;

	typedef struct _Color
	{
		int r, g, b, a;

		_Color() : r(0), g(0), b(0), a(0) {};

		_Color(int r, int g, int b, int a = 255) : r(r), g(g), b(b), a(a) {}

		unsigned long toDWORD()
		{
			return ((a & 0xff) << 0x18) | ((r & 0xff) << 0x10) | ((g & 0xff) << 0x08) | ((b & 0xff) << 0x0);
		}

	} Color;

	typedef struct _Descriptor
	{
		void* texture;
		uint32_t vertexCount;
		//uint32_t startVertex; (Don't need since can just be done on renderer?)
		uint32_t primitiveCount;
		Vector4 clippingRect;
	} Descriptor;

	typedef struct _Vertex
	{
		float x, y, z;
		Color c;
		float tu, tv;
	} Vertex;

	typedef struct _DrawData
	{
		Descriptor* descriptorList;
		uint32_t descriptorCount;
		Vertex* vertexList;
		uint32_t vertexCount;
	} DrawData;

	typedef struct _CharInfo
	{
		float u0, v0;
		float u1, v1;
		float w, h;
		float spacing;
	} CharInfo;
	// TODO: Define any other global structs here

	typedef struct _Window
	{
		std::wstring title;
		Vector2 pos, size;
		uint64_t id;




		_Window(std::wstring title) : title(title)
		{
			id = std::hash<std::wstring> {}(title);
		}
	} Window;

}

