#pragma once

#include <dinput.h>

class DInputClass
{
private:
	LPDIRECTINPUT8				m_pDirectInput;
	LPDIRECTINPUTDEVICE8		m_KeyboardDevice;
	char						m_keyBuffer[256];

	LPDIRECTINPUTDEVICE8		m_MouseDevice;
	DIMOUSESTATE				m_MouseState;

public:
	HRESULT Init(HWND hWnd, HINSTANCE hInstance, DWORD keyboardCoopFlags, DWORD mouseCoopFlags);
	void GetInput();
	bool IsKeyDown(int iKey);
	bool IsMouseButtonDown(int button);
	float MouseDX();
	float MouseDY();
	float MouseDZ();


public:
	DInputClass();
	~DInputClass();
};

