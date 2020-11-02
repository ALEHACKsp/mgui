#include "directx9_renderer.h"

#define TEX_SIZE 4096

namespace mgui
{
	Renderer::Renderer(LPDIRECT3DDEVICE9 device)
		:
		device(device),
		vBuffer(nullptr),
		fontMap(nullptr),
		vertexBufferSize(0ul)
	{
		// TODO: Render state
		device->AddRef();
	}

	Renderer::~Renderer()
	{
		if (vBuffer)
		{
			vBuffer->Release();
		}
		if (fontMap)
		{
			fontMap->Release();
		}
		if (device)
		{
			device->Release();
		}
	}

	void Renderer::render(DrawData* data)
	{
		HRESULT hr;

		if (vertexBufferSize < data->vertexCount)
		{
			if (vBuffer)
			{
				vBuffer->Release();
			}

			hr = device->CreateVertexBuffer(sizeof(DX9Vertex) * (data->vertexCount + 2000ull), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, CUSTOMFVF, D3DPOOL_DEFAULT, &vBuffer, nullptr);

			if (!SUCCEEDED(hr))
			{
				wprintf_s(L"[-] DX9: Failed to create vertex buffer\n");
				return;
			}
			vertexBufferSize = data->vertexCount + 2000ull;
		}

		DX9Vertex* buffer;

		hr = vBuffer->Lock(0, NULL, reinterpret_cast<void**>(&buffer), D3DLOCK_DISCARD);

		if (!SUCCEEDED(hr))
		{
			wprintf_s(L"[-] DX9: Failed to lock vertex buffer\n");
			return;
		}


		for (auto i = 0ul; i < data->vertexCount; ++i)
		{
			auto& vertex = data->vertexList[i];
			buffer[i].x = vertex.x;
			buffer[i].y = vertex.y;
			buffer[i].z = vertex.z;
			buffer[i].rhw = 1;
			buffer[i].color = vertex.c.toDWORD();
			buffer[i].tu = vertex.tu;
			buffer[i].tv = vertex.tv;
		}

		hr = vBuffer->Unlock();

		if (!SUCCEEDED(hr))
		{
			wprintf_s(L"[-] DX9: Failed to unlock vertex buffer\n");
			return;
		}	


		device->SetFVF(CUSTOMFVF);
		device->SetStreamSource(0, vBuffer, 0, sizeof(DX9Vertex));

		uint32_t vertexIndex = 0;
		for (auto i = 0ul; i < data->descriptorCount; ++i)
		{
			auto& descriptor = data->descriptorList[i];

			device->SetTexture(0, (LPDIRECT3DTEXTURE9)descriptor.texture);

			// TODO: Need to sort out clipping, should be stored in descriptor so a call to a draw command must have the clipping rect
			// device->SetScissorRect(nullptr);
			hr = device->DrawPrimitive(D3DPT_TRIANGLESTRIP, vertexIndex, descriptor.primCount);

			vertexIndex += descriptor.vertexCount;
		}
		
	}

	void Renderer::release()
	{
		if (vBuffer)
		{
			vBuffer->Release();
			vBuffer = nullptr;
		}
		if (fontMap)
		{
			fontMap->Release();
			fontMap = nullptr;
		}
	}

	void Renderer::updateFontMap(Gui* gui)
	{
		auto bitmap = gui->getFontData();
		if (!bitmap)
		{
			return;
		}

		HRESULT hr;

		if (!fontMap)
		{
			hr = device->CreateTexture(TEX_SIZE, TEX_SIZE, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &fontMap, NULL);

			if (!SUCCEEDED(hr))
			{
				wprintf_s(L"[-] DX9: Failed to create texture for font map\n");
				return;
			}
		}

		RECT rect = { 0 };
		rect.left = rect.top = 0;
		rect.right = rect.bottom = TEX_SIZE - 1;

		D3DLOCKED_RECT lRect;

		hr = fontMap->LockRect(0, &lRect, &rect, NULL);

		if (!SUCCEEDED(hr))
		{
			wprintf_s(L"[-] DX9: Failed to lock texture rect\n");
			return;
		}
		
		size_t size = sizeof(uint32_t) * TEX_SIZE * TEX_SIZE;

		memcpy_s(lRect.pBits, size, bitmap, size);
		
		hr = fontMap->UnlockRect(0);

		if (!SUCCEEDED(hr))
		{
			wprintf_s(L"[-] DX9: Failed to unlock texture rect\n");
			return;
		}

		gui->setFontTexture(fontMap);
	}

}