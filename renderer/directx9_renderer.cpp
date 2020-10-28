#include "directx9_renderer.h"


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
				printf_s("[-] DX9: Failed to create vertex buffer\n");
				return;
			}
			vertexBufferSize = data->vertexCount + 2000ull;
		}

		DX9Vertex* buffer;

		hr = vBuffer->Lock(0, NULL, reinterpret_cast<void**>(&buffer), D3DLOCK_DISCARD);

		if (!SUCCEEDED(hr))
		{
			printf_s("[-] DX9: Failed to lock vertex buffer\n");
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
			printf_s("[-] DX9: Failed to unlock vertex buffer\n");
			return;
		}	


		device->SetFVF(CUSTOMFVF);
		device->SetStreamSource(0, vBuffer, 0, sizeof(DX9Vertex));

		uint32_t vertexIndex = 0;
		for (auto i = 0ul; i < data->descriptorCount; ++i)
		{
			auto& descriptor = data->descriptorList[i];

			device->SetTexture(0, (LPDIRECT3DTEXTURE9)descriptor.texture);

			// TODO: Need to sort out clipping
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

	bool Renderer::updateFontMap()
	{
		return false;
	}

}