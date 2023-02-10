#include "stdafx.h"
#include "WindowContainer.h"

#if _DEBUG
extern bool g_bDebug;
#include "MouseCapture.h"
#endif

WindowContainer::WindowContainer()
{
	static bool rawInputInitialized = false;
	if ( !rawInputInitialized )
	{
		RAWINPUTDEVICE rid = { 0 };
		rid.usUsagePage = 0x01;
		rid.usUsage = 0x02;
		rid.dwFlags = 0;
		rid.hwndTarget = NULL;

		if ( RegisterRawInputDevices( &rid, 1, sizeof( rid ) ) == FALSE )
		{
			ErrorLogger::Log( GetLastError(), "Failed to register raw input devices!" );
			exit( -1 );
		}

		rawInputInitialized = true;
	}
}

WindowContainer::~WindowContainer() { }

#if _DEBUG
extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
#endif
LRESULT CALLBACK WindowContainer::WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
#if _DEBUG
    if ( g_bDebug )
        if ( ImGui_ImplWin32_WndProcHandler( hWnd, uMsg, wParam, lParam ) )
            return true;
    const auto& imio = ImGui::GetIO();
#endif

    switch( uMsg )
    {
    case WM_ACTIVATE:
		if ( !m_bCursorEnabled )
		{
			if ( wParam & WA_ACTIVE )
				DisableCursor();
			else
				EnableCursor();
		}
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    // Keyboard Events
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
#if _DEBUG
        if ( g_bDebug )
            if ( imio.WantCaptureKeyboard )
			    return 0;
#endif
        unsigned char keycode = static_cast<unsigned char>( wParam );
        m_keyboard.OnKeyReleased( keycode );
        return 0;
    }
    case WM_CHAR:
    {
#if _DEBUG
        if ( g_bDebug )
            if ( imio.WantCaptureKeyboard )
			    return 0;
#endif
        unsigned char ch = static_cast<unsigned char>( wParam );
        if ( m_keyboard.IsCharsAutoRepeat() )
            m_keyboard.OnChar( ch );
        else
        {
            const bool wasPressed = lParam & 0x40000000;
            if ( !wasPressed )
                m_keyboard.OnChar( ch );
        }
        return 0;
    }
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    {
#if _DEBUG
        if ( g_bDebug )
            if ( imio.WantCaptureKeyboard )
			    return 0;
#endif
        unsigned char keycode = static_cast<unsigned char>( wParam );
        if ( m_keyboard.IsKeysAutoRepeat() )
            m_keyboard.OnKeyPressed( keycode );
        else
        {
            const bool wasPressed = lParam & 0x40000000;
            if ( !wasPressed )
                m_keyboard.OnKeyPressed( keycode );
        }
        return 0;
    }

    // Mouse Events
    case WM_MOUSEMOVE:
    {
        int x = LOWORD( lParam );
		int y = HIWORD( lParam );

		if ( !m_bCursorEnabled )
		{
			if ( !m_mouse.IsInWindow() )
			{
				SetCapture( m_renderWindow.GetHWND() );
                m_mouse.OnMouseEnter( x, y );
				HideCursor();
			}
			return 0;
		}

#if _DEBUG
        if ( g_bDebug )
        {
		    if ( imio.WantCaptureMouse )
            {
			    Vector2f vImGuiMousePos = Vector2f( ImGui::GetMousePos().x, ImGui::GetMousePos().y );
                EventSystem::Instance()->AddEvent( EVENTID::MousePosition, &vImGuiMousePos );
                return 0;
            }
        }
#endif

        Vector2f* mousePos = new Vector2f( x, y );
		if ( mousePos->x >= 0 && mousePos->x < m_windowSize.x && mousePos->y >= 0 && mousePos->y < m_windowSize.y )
		{
            m_mouse.OnMouseMove( x, y );
			if ( !m_mouse.IsInWindow() )
			{
				SetCapture( m_renderWindow.GetHWND() );
                m_mouse.OnMouseEnter( x, y );
			}
		}
		else
		{
			if ( wParam & ( MK_LBUTTON | MK_RBUTTON ) )
			{
                m_mouse.OnMouseMove( x, y );
			}
			else
			{
				ReleaseCapture();
                m_mouse.OnMouseLeave( x, y );
			}
		}
        EventSystem::Instance()->AddEvent( EVENTID::MousePosition, mousePos );

		return 0;
    }
    case WM_LBUTTONDOWN:
    {
        SetForegroundWindow( m_renderWindow.GetHWND() );

        int x = LOWORD( lParam );
		int y = HIWORD( lParam );

#if _DEBUG
        if ( g_bDebug )
        {
		    if ( imio.WantCaptureMouse )
            {
                Vector2f vImGuiMousePos = Vector2f( ImGui::GetMousePos().x, ImGui::GetMousePos().y );
                EventSystem::Instance()->AddEvent( EVENTID::LeftMouseClick, &vImGuiMousePos );
			    return 0;
            }
        }
#endif

        m_mouse.OnLeftPressed( x, y );
        Vector2f* mousePos = new Vector2f( x, y );
        EventSystem::Instance()->AddEvent( EVENTID::LeftMouseClick, mousePos );

		if ( !m_bCursorEnabled )
		{
			ConfineCursor();
			HideCursor();
		}
		return 0;
    }
    case WM_LBUTTONUP:
    {
        SetForegroundWindow( m_renderWindow.GetHWND() );

        int x = LOWORD( lParam );
		int y = HIWORD( lParam );

#if _DEBUG
        if ( g_bDebug )
        {
		    if ( imio.WantCaptureMouse )
            {
                Vector2f vImGuiMousePos = Vector2f( ImGui::GetMousePos().x, ImGui::GetMousePos().y );
                EventSystem::Instance()->AddEvent( EVENTID::LeftMouseRelease, &vImGuiMousePos );
			    return 0;
            }
        }
#endif

        m_mouse.OnLeftReleased( x, y );
        Vector2f* mousePos = new Vector2f( x, y );
        EventSystem::Instance()->AddEvent( EVENTID::LeftMouseRelease, mousePos );

		if ( mousePos->x < 0 || mousePos->x >= m_windowSize.x || mousePos->y < 0 || mousePos->y >= m_windowSize.y )
		{
			ReleaseCapture();
            m_mouse.OnMouseLeave( x, y );
		}

		return 0;
    }
    case WM_RBUTTONDOWN:
    {
        SetForegroundWindow( m_renderWindow.GetHWND() );

        int x = LOWORD( lParam );
		int y = HIWORD( lParam );

#if _DEBUG
        if ( g_bDebug )
        {
            if ( imio.WantCaptureMouse )
            {
                Vector2f vImGuiMousePos = Vector2f( ImGui::GetMousePos().x, ImGui::GetMousePos().y );
                EventSystem::Instance()->AddEvent( EVENTID::RightMouseClick, &vImGuiMousePos );
			    return 0;
            }
        }
#endif

        m_mouse.OnRightPressed( x, y );
        Vector2f* mousePos = new Vector2f( x, y );
        EventSystem::Instance()->AddEvent( EVENTID::RightMouseClick, mousePos );

        return 0;
    }
    case WM_RBUTTONUP:
    {
        SetForegroundWindow( m_renderWindow.GetHWND() );

        int x = LOWORD( lParam );
		int y = HIWORD( lParam );

#if _DEBUG
        if ( g_bDebug )
        {
            if ( imio.WantCaptureMouse )
            {
                Vector2f vImGuiMousePos = Vector2f( ImGui::GetMousePos().x, ImGui::GetMousePos().y );
                EventSystem::Instance()->AddEvent( EVENTID::RightMouseRelease, &vImGuiMousePos );
			    return 0;
            }
        }
#endif

        m_mouse.OnRightReleased( x, y );
        Vector2f* mousePos = new Vector2f( x, y );
        EventSystem::Instance()->AddEvent( EVENTID::RightMouseRelease, mousePos );

		if ( mousePos->x < 0 || mousePos->x >= m_windowSize.x || mousePos->y < 0 || mousePos->y >= m_windowSize.y )
		{
			ReleaseCapture();
            m_mouse.OnMouseLeave( x, y );
		}

		return 0;
    }
    case WM_MBUTTONDOWN:
    {
        SetForegroundWindow( m_renderWindow.GetHWND() );

        int x = LOWORD( lParam );
		int y = HIWORD( lParam );

#if _DEBUG
        if ( g_bDebug )
        {
            if ( imio.WantCaptureMouse )
            {
                Vector2f vImGuiMousePos = Vector2f( ImGui::GetMousePos().x, ImGui::GetMousePos().y );
                EventSystem::Instance()->AddEvent( EVENTID::MiddleMouseClick, &vImGuiMousePos );
			    return 0;
            }
        }
#endif

        m_mouse.OnMiddlePressed( x, y );
        Vector2f* mousePos = new Vector2f( x, y );
        EventSystem::Instance()->AddEvent( EVENTID::MiddleMouseClick, mousePos );

        return 0;
    }
    case WM_MBUTTONUP:
    {
        SetForegroundWindow( m_renderWindow.GetHWND() );

        int x = LOWORD( lParam );
		int y = HIWORD( lParam );

#if _DEBUG
        if ( g_bDebug )
        {
            if ( imio.WantCaptureMouse )
            {
                Vector2f vImGuiMousePos = Vector2f( ImGui::GetMousePos().x, ImGui::GetMousePos().y );
                EventSystem::Instance()->AddEvent( EVENTID::MiddleMouseRelease, &vImGuiMousePos );
			    return 0;
            }
        }
#endif

        m_mouse.OnMiddleReleased( x, y );
        Vector2f* mousePos = new Vector2f( x, y );
        EventSystem::Instance()->AddEvent( EVENTID::MiddleMouseRelease, mousePos );

        return 0;
    }
    case WM_MOUSEWHEEL:
    {
#if _DEBUG
        if ( g_bDebug )
            if ( imio.WantCaptureMouse )
			    return 0;
#endif
        int x = LOWORD( lParam );
        int y = HIWORD( lParam );
        if ( GET_WHEEL_DELTA_WPARAM( wParam ) > 0 )
        {
            m_mouse.OnWheelUp( x, y );
        }
        else if ( GET_WHEEL_DELTA_WPARAM( wParam ) < 0 )
        {
            m_mouse.OnWheelDown( x, y );
        }
        return 0;
    }
    case WM_INPUT:
    {
        UINT dataSize;
        GetRawInputData( reinterpret_cast<HRAWINPUT>( lParam ), RID_INPUT, NULL, &dataSize, sizeof( RAWINPUTHEADER ) );
        if ( dataSize > 0 )
        {
            std::unique_ptr<BYTE[]> rawData = std::make_unique<BYTE[]>( dataSize );
            if ( GetRawInputData( reinterpret_cast<HRAWINPUT>( lParam ), RID_INPUT, rawData.get(), &dataSize, sizeof( RAWINPUTHEADER ) ) == dataSize )
            {
                RAWINPUT* raw = reinterpret_cast<RAWINPUT*>( rawData.get() );
                if ( raw->header.dwType == RIM_TYPEMOUSE )
                {
                    m_mouse.OnMouseMoveRaw( raw->data.mouse.lLastX, raw->data.mouse.lLastY );
                }
            }
        }
        return DefWindowProc( hWnd, uMsg, wParam, lParam );
    }
    // Window Resize Event
    case WM_SIZE:
	{
		RECT windowRect = { 0, 0 };
		if ( GetClientRect( m_renderWindow.GetHWND(), &windowRect ) )
		{
			m_windowSize =
            {
                static_cast<float>( windowRect.right - windowRect.left ),
                static_cast<float>( windowRect.bottom - windowRect.top )
            };
            EventSystem::Instance()->AddEvent( EVENTID::WindowSizeChangeEvent, &m_windowSize );
			return DefWindowProc( hWnd, uMsg, wParam, lParam );;
		}
	}

    default:
        return DefWindowProc( hWnd, uMsg, wParam, lParam );
    }

    return 0;
}

void WindowContainer::EnableCursor() noexcept
{
	m_bCursorEnabled = true;
	ShowCursor();
#if _DEBUG
	EnableImGuiMouse();
#endif
	FreeCursor();
}

void WindowContainer::DisableCursor() noexcept
{
	m_bCursorEnabled = false;
	HideCursor();
#if _DEBUG
	DisableImGuiMouse();
#endif
	ConfineCursor();
}

void WindowContainer::ConfineCursor() noexcept
{
	RECT rect;
	GetClientRect( m_renderWindow.GetHWND(), &rect );
	MapWindowPoints( m_renderWindow.GetHWND(), nullptr, reinterpret_cast<POINT*>( &rect ), 2 );
	ClipCursor( &rect );
}

void WindowContainer::FreeCursor() noexcept
{
	ClipCursor( nullptr );
}

void WindowContainer::ShowCursor() noexcept
{
	while ( ::ShowCursor( TRUE ) < 0 );
}

void WindowContainer::HideCursor() noexcept
{
	while ( ::ShowCursor( FALSE ) >= 0 );
}

#if _DEBUG
void WindowContainer::EnableImGuiMouse() noexcept
{
	ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

void WindowContainer::DisableImGuiMouse() noexcept
{
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
}
#endif