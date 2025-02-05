#pragma once
#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

class WindowContainer;
class RenderWindow : public Listener
{
public:
	enum class CursorType
	{
		NORMAL,
		LINK,
		PRECISION,
		MOVE,
		HELP,
		TEXT,
		PEN,
		UNAVAILABLE
	};
	bool Initialize(
		WindowContainer* windowContainer,
		HINSTANCE hInstance,
		const std::string& windowTitle,
		const std::string& windowClass,
		int width,
		int height
	);
	bool ProcessMessages() noexcept;
	HWND GetHWND() const noexcept;
	~RenderWindow() noexcept;

	inline HCURSOR GetCursor( CursorType type ) noexcept { return cursors[type]; }
	inline bool GetIsStopNextFrame() const noexcept{ return m_bIsStopNextFrame; }
	inline void SetIsStopNextFrame( bool isStop ) { m_bIsStopNextFrame = isStop; }

private:
	void AddToEvent() noexcept;
	void RemoveFromEvent() noexcept;
	void HandleEvent( Event* event ) override;

	void RegisterWindowClass() noexcept;
	HWND m_hWnd = NULL;
	HINSTANCE m_hInstance = NULL;
	std::string m_sWindowTitle = "";
	std::wstring m_wsWindowTitle = L"";
	std::string m_sWindowClass = "";
	std::wstring m_wsWindowClass = L"";
	int m_iWidth, m_iHeight;
	bool m_bIsStopNextFrame = true;
	std::unordered_map<CursorType, HCURSOR> cursors;
};

#endif