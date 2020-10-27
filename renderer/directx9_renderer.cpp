#include "directx9_renderer.h"


namespace mgui
{

	Renderer::Renderer(LPDIRECT3DDEVICE9 device)
		: device(device)
	{
		// TODO: Any initialization for renderer to be done here
				// Probably only need to do render state I guess and maybe vertex buffer
				// Also pre-load a default font?
	}


}