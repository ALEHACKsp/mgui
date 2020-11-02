#include "directx9_renderer.h"

#define TEX_SIZE 4096

namespace mgui
{
	Renderer::Renderer(LPDIRECT3DDEVICE9 device)
		:
		device(device),
		vBuffer(nullptr),
		fontMap(nullptr),
		oldState(nullptr),
		vertexBufferSize(0ul)
	{
		

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

	void Renderer::setRenderState()
	{
		if (!oldState)
		{
			HRESULT hr = device->CreateStateBlock(D3DSBT_ALL, &oldState);
			if (!SUCCEEDED(hr))
			{
				wprintf_s(L"[-] DX9: Failed to create state block\n");
				return;
			}

		}
		oldState->Capture();

		device->SetFVF(CUSTOMFVF);
		device->SetStreamSource(0, vBuffer, 0, sizeof(DX9Vertex));

		device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

		device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		device->SetRenderState(D3DRS_ALPHAREF, 0x08);
		device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

		device->SetRenderState(D3DRS_LIGHTING, FALSE);

		device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);

		device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
		device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
		device->SetRenderState(D3DRS_CLIPPING, TRUE);
		device->SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE);
		device->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
		device->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
		device->SetRenderState(D3DRS_FOGENABLE, FALSE);
		device->SetRenderState(D3DRS_COLORWRITEENABLE,
								D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |
								D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);

		device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
		device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
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

			hr = device->CreateVertexBuffer(sizeof(DX9Vertex) * (data->vertexCount + 5000ull), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, CUSTOMFVF, D3DPOOL_DEFAULT, &vBuffer, nullptr);

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

		setRenderState();

		uint32_t vertexIndex = 0;
		for (auto i = 0ul; i < data->descriptorCount; ++i)
		{
			auto& descriptor = data->descriptorList[i];

			device->SetTexture(0, (LPDIRECT3DTEXTURE9)descriptor.texture);

			RECT r =
			{
				static_cast<LONG>(descriptor.clippingRect.left),
				static_cast<LONG>(descriptor.clippingRect.top),
				static_cast<LONG>(descriptor.clippingRect.right),
				static_cast<LONG>(descriptor.clippingRect.bottom)
			};
			device->SetScissorRect(&r);	

			hr = device->DrawPrimitive(D3DPT_TRIANGLESTRIP, vertexIndex, descriptor.primitiveCount);

			vertexIndex += descriptor.vertexCount;
		}
		
		oldState->Apply();
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