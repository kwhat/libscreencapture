/* libScreenShot: Cross-platfrom fast screenshot library.
 * Copyright (C) 2014 Alexander Barker.  All Rights Received.
 * https://github.com/kwhat/libscreenshot/
 *
 * libScreenShot is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libScreenShot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

static IDirect3D9*	direct3d = NULL;
static IDirect3DDevice9* device = NULL;
static IDirect3DSurface9* surface = NULL;

int init() {
	direct3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (direct3d != NULL) {
		D3DDISPLAYMODE mode;

		// FIXME Assumes error codes are != 0
		if (direct3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&mode)) == 0) {
			HWND hDesktopWnd = GetDesktopWindow();

			D3DPRESENT_PARAMETERS params = { 0 };
			params.Windowed = WINDOW_MODE;
			params.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
			params.BackBufferFormat = mode.Format;
			params.BackBufferHeight = nDisplayHeight = gScreenRect.bottom = mode.Height;
			params.BackBufferWidth = nDisplayWidth = gScreenRect.right = mode.Width;
			params.MultiSampleType = D3DMULTISAMPLE_NONE;
			params.SwapEffect = D3DSWAPEFFECT_DISCARD;
			params.hDeviceWindow = hDesktopWnd;
			params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
			params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

			if (direct3d->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hDesktopWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING ,&params, &device) == 0) {
				if (device->CreateOffscreenPlainSurface(mode.Width, mode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &surface, NULL) == 0) {

				}
				else {
					// Unable to create surface
					//return E_FAIL;
				}
			}
			else {
				// Unable to create device.
				//return E_FAIL;
			}
		}
		else {
			//Unable to get the adapter display mode
			//return E_FAIL;
		}
	}
	else {
		// TODO Error, could not create direct 3d pointer.
	}
}

int cleanup() {
	if (surface) {
		surface->Release();
		surface = NULL;
	}

	if (device) {
		device->Release();
		device = NULL;
	}

	if (direct3d) {
		direct3d->Release();
		direct3d = NULL;
	}
}

screenshot* capture() {
	device->GetFrontBufferData(0, surface);

	D3DLOCKED_RECT lockedRect;
	surface->LockRect(&lockedRect, NULL, D3DLOCK_NO_DIRTY_UPDATE | D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY);

	for (int i = 0; i < ScreenHeight; i++) {
		memcpy( (BYTE*) pBits + i * ScreenWidth * BITSPERPIXEL / 8 ,
			(BYTE*) lockedRect.pBits + i* lockedRect.Pitch ,
			ScreenWidth * BITSPERPIXEL / 8);
	}

	surface->UnlockRect();
}
