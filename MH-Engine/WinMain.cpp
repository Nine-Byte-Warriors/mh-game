#include "stdafx.h"
#include "Application.h"
//Only for Testing Score (needs cleaning after)
#include "stdio.h"
#include <iostream>

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );
    UNREFERENCED_PARAMETER( nCmdShow );

    HRESULT hr = CoInitializeEx( NULL, COINITBASE_MULTITHREADED );
    if ( FAILED( hr ) )
    {
        ErrorLogger::Log( hr, "Failed to call CoInitialize!" );
        return -1;
    }

    Application theApp;
    if ( theApp.Initialize( hInstance, 1280, 720 ) )
    {
        while ( theApp.ProcessMessages() == true )
        {
            theApp.Update();
            theApp.Render();
            //Only for Testing Score (needs cleaning after)
            AllocConsole();
            printf("Debugging Window:\n");
        }
    }
    theApp.CleanupDevice();

    return 0;
}