#pragma once
#include <dinput.h>

class Input
{
public:
	Input();
	HRESULT Initialize(HINSTANCE inst, HWND wnd);
	void ReadInputStates();
	bool IsKeyPressed(unsigned char DI_keycode);
	void KeyboardInput(camera* cam);
	void MouseInput(camera* cam);
	~Input();
};

