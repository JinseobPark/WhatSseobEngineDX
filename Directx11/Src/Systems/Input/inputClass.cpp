#include "stdafx.h"
#include "inputclass.h"
#include "Systems/Application/Application.h"

InputClass * mINPUT = nullptr;

InputClass::InputClass()
{
	assert(mINPUT == nullptr && "No more than one instance of Application system!");
	mINPUT = this;
}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
}


void InputClass::Initialize()
{
	// Store the screen size which will be used for positioning the mouse cursor.
	m_screenWidth = mAPPLICATION->GetWidth();
	m_screenHeight = mAPPLICATION->GetHeight();

	// Initialize the location of the mouse on the screen.
	m_mouseX = 0;
	m_mouseY = 0;

	// Initialize the main direct input interface.
	HRESULT result = DirectInput8Create(mAPPLICATION->GetHinstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
	{
		return ;
	}

	// Initialize the direct input interface for the keyboard.
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
	{
		return ;
	}

	// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return ;
	}

	// Set the cooperative level of the keyboard to not share with other programs.
	result = m_keyboard->SetCooperativeLevel(mAPPLICATION->GetHWND(), DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		return ;
	}

	// Acquire the keyboard.
	result = m_keyboard->Acquire();
	if (FAILED(result))
	{
		return ;
	}

	// Initialize the direct input interface for the mouse.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
	{
		return ;
	}

	// Set the data format for the mouse using the pre-defined mouse data format.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return ;
	}

	// Set the cooperative level of the mouse to share with other programs.
	result = m_mouse->SetCooperativeLevel(mAPPLICATION->GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return ;
	}

	// Acquire the mouse.
	result = m_mouse->Acquire();
	if (FAILED(result))
	{
		return ;
	}
	
	return ;
}


void InputClass::Shutdown()
{
	// Release the mouse.
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// Release the keyboard.
	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// Release the main interface to direct input.
	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}
}


void InputClass::Update(float)
{
	// Read the current state of the keyboard.
	if (!ReadKeyboard())
	{
		return ;
	}

	// Read the current state of the mouse.
	if (!ReadMouse())
	{
		return ;
	}

	// Process the changes in the mouse and keyboard.
	ProcessInput();
	UpdateMouseMover();
	return ;
}



bool InputClass::ReadKeyboard()
{
	// Read the keyboard device.
	HRESULT result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}


bool InputClass::ReadMouse()
{
	// Read the mouse device.
	HRESULT result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}


void InputClass::ProcessInput()
{
	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// Ensure the mouse location doesn't exceed the screen width or height.
	if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }

	if (m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth; }
	if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }
}


bool InputClass::IsEscapePressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}


void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
}


bool InputClass::IsPressed(char key)
{
	if (m_keyboardState[key] & 0x80)
	{
		return true;
	}
	return false;
}

LONG InputClass::GetMouseMoveX()
{
	return m_mouseState.lX;
}

LONG InputClass::GetMouseMoveY()
{
	return m_mouseState.lY;
}

bool InputClass::IsRightDown()
{
	return m_mouseState.rgbButtons[1];
}

bool InputClass::IsLeftDown()
{
	return m_mouseState.rgbButtons[0];
}

void InputClass::UpdateMouseMover()
{
	mouseMover[0] = m_mouseState.lX;
	mouseMover[1] = m_mouseState.lY;
	mouseMover[2] = m_mouseState.lZ;
}
