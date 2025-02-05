#pragma once
#ifndef WINDOWCONTAINER_H
#define WINDOWCONTAINER_H

#include "Mouse.h"
#include "Keyboard.h"
#include "RenderWindow.h"

class WindowContainer
{
public:
	WindowContainer();
	~WindowContainer();
	LRESULT CALLBACK WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	inline bool IsCursorEnabled() const noexcept { return m_bCursorEnabled; }

protected:
	void ConfineCursor() noexcept;
	void FreeCursor() noexcept;
	void ShowCursor() noexcept;
	void HideCursor() noexcept;
#if _DEBUG
	void EnableImGuiMouse() noexcept;
	void DisableImGuiMouse() noexcept;
#endif

	bool m_bCursorEnabled = true;
	XMFLOAT2 m_windowSize;

	RenderWindow m_renderWindow;
	Keyboard m_keyboard;
	Mouse m_mouse;
};

#endif