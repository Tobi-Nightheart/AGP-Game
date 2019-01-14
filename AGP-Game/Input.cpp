#pragma once
#include "Input.h"
#include "pch.h"
#include <minwinbase.h>

IDirectInput8A* g_direct_input;
IDirectInputDevice8A* g_keyboard_device;
IDirectInputDevice8A* g_mouse_device;
DIMOUSESTATE g_mouse_state;
unsigned char g_keyboard_state[256];
HWND g_iWnd;


Input::Input()
{
	
}

HRESULT Input::Initialize(HINSTANCE inst, HWND wnd)
{
	g_iWnd = wnd;
	
	HRESULT hr;
	ZeroMemory(g_keyboard_state, sizeof(g_keyboard_state));
	//setting up the keyboard
	hr = DirectInput8Create(inst, DIRECTINPUT_VERSION, IID_IDirectInput8A, reinterpret_cast<void**>(&g_direct_input), nullptr);
	if (FAILED(hr)) return hr;
	hr = g_direct_input->CreateDevice(GUID_SysKeyboard, &g_keyboard_device, nullptr);
	if (FAILED(hr)) return hr;
	hr = g_keyboard_device->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr)) return hr;
	hr = g_keyboard_device->SetCooperativeLevel(wnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) return hr;
	hr = g_keyboard_device->Acquire();
	if (FAILED(hr)) return hr;
	//setting up the mouse --  is set to exclusive to get rid of the mouse pointer
	hr = g_direct_input->CreateDevice(GUID_SysMouse, &g_mouse_device, nullptr);
	if (FAILED(hr)) return hr;
	hr = g_mouse_device->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr)) return hr;
	hr = g_mouse_device->SetCooperativeLevel(wnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(hr)) return hr;
	hr = g_mouse_device->Acquire();
	if (FAILED(hr)) return hr;

	return S_OK;
}

void Input::ReadInputStates()
{
	HRESULT hr;
	hr = g_keyboard_device->GetDeviceState(sizeof(g_keyboard_state), (LPVOID) &g_keyboard_state);

	if(FAILED(hr))
	{
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			g_keyboard_device->Acquire();
		}
	}

	hr = g_mouse_device->GetDeviceState(sizeof(g_mouse_device), (LPVOID) &g_mouse_device);
	if(FAILED(hr))
	{
		if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			g_mouse_device->Acquire();
		}
	}
}

bool Input::IsKeyPressed(unsigned char DI_keycode)
{
	return g_keyboard_state[DI_keycode] & 0x80;
}

void Input::KeyboardInput(camera* cam)
{
	if (IsKeyPressed(DIK_ESCAPE)) DestroyWindow(g_iWnd);
	if (IsKeyPressed(DIK_W)) cam->Forward(0.0005f);
	if (IsKeyPressed(DIK_A)) cam->Strafe(-0.0005f);
	if (IsKeyPressed(DIK_D)) cam->Strafe(0.0005f);
	if (IsKeyPressed(DIK_S)) cam->Forward(-0.0005f);
	if (IsKeyPressed(DIK_Q)) cam->Rotate(0.05f, 0.f);
	if (IsKeyPressed(DIK_E)) cam->Rotate(-0.05f, 0.f);
}

void Input::MouseInput(camera* cam)
{

}


Input::~Input()
{
	if(g_keyboard_device)
	{
		g_keyboard_device->Unacquire();
		g_keyboard_device->Release();
	}
	if(g_mouse_device)
	{
		g_mouse_device->Unacquire();
		g_mouse_device->Release();
	}
	if (g_direct_input) g_direct_input->Release();
}
