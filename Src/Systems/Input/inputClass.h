#pragma once
#include "Systems/System.h"
class InputClass : public System
{
public:

	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	void Initialize() override;
	void Shutdown();
	void Update(float ) override;

	bool IsEscapePressed();
	void GetMouseLocation(int&, int&);


	bool IsPressed(char);
	LONG GetMouseMoveX();
	LONG GetMouseMoveY();

	bool IsRightDown();
	bool IsLeftDown();

	float mouseMover[3] = { 0.0f, 0.0f, 0.0f };
private:
	void UpdateMouseMover();
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_directInput = nullptr;
	IDirectInputDevice8* m_keyboard = nullptr;
	IDirectInputDevice8* m_mouse = nullptr;

	unsigned char m_keyboardState[256] = { 0, };
	DIMOUSESTATE m_mouseState;

	int m_screenWidth = 0;
	int m_screenHeight = 0;
	int m_mouseX = 0;
	int m_mouseY = 0;

};
extern InputClass* mINPUT;