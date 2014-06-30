#ifndef GUI2_GUARD
#define GUI2_GUARD
#include <iostream>
//#include <cstring>
#include "gui2.h"

//Declared in main.cpp
extern bool has_saved, has_opened, discard;
extern char * source_filename, * dest_filename, * precipher, * postcipher, * keyword, ignore, mode;

using namespace std;

//Opens a file explorer screen to get an open file name, or a save filename.
//Warning!!!!: Dynamic array returned, delete[] it.
char * get_file_path( HWND main_wnd_instance, bool is_open )
{
	char filename[ MAX_PATH ]="";
	OPENFILENAME file;
	ZeroMemory( &file, sizeof(file) );
	file.lStructSize = sizeof( OPENFILENAME );
	file.hwndOwner = main_wnd_instance;
	//The filter which shows .txt files versus all files.
	file.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
	//The explorer can only open existing files.
	file.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
	file.nMaxFile = MAX_PATH;
	file.lpstrFile = filename;
	//default filter to .txt files.
	file.lpstrDefExt = "txt";
	//Choose between a save or an open explorer.
	if( is_open )
	{
		GetOpenFileName( &file );
	}
	else
	{
		GetSaveFileName( &file );
	}
	int size = strlen( filename );
	//This must be cleared later.
	char * c = new char[ size + 1 ];
	//Get filename from explorer and return the result.
	strcpy( c, filename );
	return c;
}

//Read the precipher text from the opened file (source_filename).
bool get_precipher()
{
	bool success=false;
	//If no source, task is impossible.
	if( source_filename == 0 )
	{
		return success;
	}
	//If there is a source, open windows file system file.
	HANDLE file;
	file = CreateFile( source_filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	if(file != INVALID_HANDLE_VALUE)
	{
		//If file system was OK with request, check the file size.
		DWORD file_size= GetFileSize( file, NULL );
		if( file_size != 0xFFFFFFFF )
		{
			//If the file is not too large, try and read in the pre-cipher text.
			precipher= new (std::nothrow) char[ file_size + 1 ];
			if( precipher != 0 )
			{
				//If there was enough memory, read in the data and cap it off with null terminator.
				DWORD bytes_read;
				if( ReadFile( file, precipher, file_size, &bytes_read, NULL ) )
				{
					precipher[ file_size ] = 0;
					success = true;
				}
			}
		}
	}
	//File system no longer needed.
	CloseHandle( file );
	return success;
}

//Rather unfinished procedure for a secondary window for settings (in which mode, ignore, etc could be set.)
BOOL CALLBACK Playfair_Settings_Proc( HWND settings_instance, UINT Message, WPARAM param_w, LPARAM param_l )
{
	switch( Message )
	{
		case WM_INITDIALOG:
			return TRUE;
		break;
		case WM_COMMAND:
			switch( LOWORD( param_w ) )
			{
				case IDOK:
					EndDialog(settings_instance, IDOK);
				break;
			}
		break;
		default:
			return FALSE;
	}
	return TRUE;
} 

//Standard windows OS message loop.
int message_loop()
{
	MSG msg;
	while( GetMessage( &msg, NULL, 0, 0 ) > 0)
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
	return msg.wParam;
}

//Fair Play's main window's procedure.
LRESULT CALLBACK main_procedure( HWND main_wnd_instance, UINT msg, WPARAM param_w, LPARAM param_l )
{
	switch( msg )
	{
		//At beginning, set the menu.
		case WM_CREATE:
			SetMenu( main_wnd_instance, init_main_menu( main_wnd_instance ) );
		break;
		//On attempt to close, query user to save changes.
		case WM_CLOSE:
		{
			int temp = 0;
			if( !has_saved && has_opened )
			{
				temp = MessageBox( main_wnd_instance, HAVE_NOT_SAVED, 
				"Fair Play", MB_YESNOCANCEL );
				DestroyWindow( main_wnd_instance );
			}
			else
			{
				DestroyWindow( main_wnd_instance );
			}
		}
		break;
		//If main window closes, kill message loop.
		case WM_DESTROY:
			PostQuitMessage( 0 );
		break;
		case WM_COMMAND:
			switch( LOWORD( param_w ) )
			{
				//Exit submenu pressed.
				case EXIT_MESSAGE:
					PostMessage( main_wnd_instance, WM_CLOSE, 0, 0 );
				break;
				//Open submenu pressed.
				case OPEN_MESSAGE:
				{
					int temp = 0;
					//Is a file currently open. Query to save.
					if( !has_saved && has_opened )
					{
						temp = MessageBox( main_wnd_instance, HAVE_NOT_SAVED, 
						"Fair Play", MB_YESNOCANCEL );
					}
					//If user wants to save current file, send appropriate message.
					if( temp==IDYES )
					{
						SendMessage( main_wnd_instance, WM_COMMAND, SAVE_MESSAGE, 0 );
					}
					if( temp == IDNO )
					{
						discard=true;
					}
					if( has_saved || discard || !has_opened )
					{
						//Use explorer to get open filename.
						source_filename = get_file_path( main_wnd_instance, true );
						//Grab text in specified file.
						if( get_precipher() )
						{
							//Plop it in pre-cipher text box.
							SetWindowText( GetDlgItem( main_wnd_instance,
							CHILD_ORIGINAL ), precipher );
						}
						delete[] source_filename;
						source_filename=0;
						delete[] precipher;
						precipher=0;
						has_opened=true;
						has_saved=discard=false;
					}
				}
				break;
				//Save submenu pressed.
				case SAVE_MESSAGE:
				{
					if( !has_saved )
					{
						PostMessage( main_wnd_instance, WM_COMMAND, SAVE_AS_MESSAGE, 0 );
					}
				}
				break;
				//Save as submenu pressed.
				case SAVE_AS_MESSAGE:
				{
					char * path = get_file_path( main_wnd_instance, false );
					cout << path;
					delete[] path;
				}
				break;
				case CONFIG_CIPHER_MESSAGE:
				{
					int return_value = DialogBox( GetModuleHandle( NULL ), MAKEINTRESOURCE( IDD_CONFIG_CIPHER ), main_wnd_instance, (DLGPROC) Playfair_Settings_Proc );
				}
				break;
				default:
				break;
			}
		break;
		default:
		//Otherwise, handle ordinary messages.
			return DefWindowProc( main_wnd_instance, msg, param_w, param_l );
	}
	return 0;
}

//Register the main window class.
ATOM reg_main(HINSTANCE app_instance)
{
	WNDCLASSEX main;
	main.cbSize=sizeof(WNDCLASSEX);
	main.cbClsExtra=0;
	main.cbWndExtra=0;
	main.hInstance=app_instance;
	main.lpfnWndProc=main_procedure;
	main.lpszClassName="FairPlayMain";
	main.style=0;
	main.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
	main.hIcon = NULL;
	main.hIconSm = NULL;
	main.lpszMenuName = NULL;
	main.hCursor = NULL;
	return RegisterClassEx(&main);
}

//Create the main window, entitled "Fair Play"
HWND init_main_window(HINSTANCE app_instance)
{
	HWND handle_main=NULL;
	//Ask OS to make window using most defaults, 400 px by 400 px.
	handle_main= CreateWindowEx( 0, "FairPlayMain", "Fair Play", WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT,
	CW_USEDEFAULT, 400, 400, NULL, NULL, app_instance, NULL );
	//If successful, show GUI main.
	if( handle_main != NULL )
	{
		ShowWindow( handle_main, SW_SHOWNORMAL );
		UpdateWindow( handle_main );		
	}
	return handle_main;
}

//Create top bar menu and associate each option with a message.
HMENU init_main_menu( HWND main_wnd_instance )
{
	HMENU main_menu = CreateMenu();
	HMENU submenu_file = CreatePopupMenu();
	//Basic file options.
	AppendMenu( submenu_file, MF_STRING, OPEN_MESSAGE, "&Open" );
	AppendMenu( submenu_file, MF_STRING, SAVE_MESSAGE, "&Save" );
	AppendMenu( submenu_file, MF_STRING, SAVE_AS_MESSAGE, "Save &As..." );
	AppendMenu( submenu_file, MF_SEPARATOR, 0, "" );
	//Networking util options.
	AppendMenu( submenu_file, MF_STRING, SEND_MESSAGE, "S&end" );
	AppendMenu( submenu_file, MF_STRING, RECEIVE_MESSAGE, "&Receive" );
	AppendMenu( submenu_file, MF_SEPARATOR, 0, "" );
	//Quit.
	AppendMenu( submenu_file, MF_STRING, EXIT_MESSAGE, "E&xit" );
	HMENU submenu_configure = CreatePopupMenu();
	//Cipher options.
	AppendMenu( submenu_configure, MF_STRING, CONFIG_CIPHER_MESSAGE, "&Cipher" );
	HMENU submenu_help = CreatePopupMenu();
	//User friendly section.
	AppendMenu( submenu_help, MF_STRING, ABOUT_MESSAGE, "&About" );
	AppendMenu( submenu_help, MF_STRING, HELP_MESSAGE, "&View Help" );
	//Attach all menus.
	AppendMenu( main_menu, MF_STRING | MF_POPUP, (UINT_PTR) submenu_file, "&File" );
	AppendMenu( main_menu, MF_STRING | MF_POPUP, (UINT_PTR) submenu_configure, "&Configure" );
	AppendMenu( main_menu, MF_STRING | MF_POPUP, (UINT_PTR) submenu_help, "&Help" );
	return main_menu;
	
}

//Create uneditable text box which contains what is in an opened file.
HWND init_original_edit( HWND main_wnd_instance )
{
	HWND original_edit = NULL;
	//Multi-line text box with scroll bars. Cannot be edited.
	original_edit = CreateWindowEx( WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | ES_READONLY | WS_VISIBLE
	 | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL, 10, 10, 185, 300, main_wnd_instance, (HMENU) CHILD_ORIGINAL, 
	GetModuleHandle(NULL), NULL );
	return original_edit;
}

//Creates uneditable text box which contains a preview of the post-cipher text.
HWND init_preview_edit(HWND main_wnd_instance)
{
	HWND preview_edit=NULL;
	//Multi-line text box with scroll bars. Cannot be edited.
	preview_edit = CreateWindowEx( WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | ES_READONLY | WS_VISIBLE
	| ES_MULTILINE | WS_VSCROLL | WS_HSCROLL, 205, 10, 185, 300, main_wnd_instance, (HMENU) CHILD_PREVIEW, 
	GetModuleHandle(NULL), NULL );
	return preview_edit;
}

//Create a small patch of text on the screen which labels the pre-cipher text box.
HWND init_original_static(HWND main_wnd_instance)
{
	HWND original_static=NULL;
	original_static = CreateWindowEx( 0, "STATIC", "", WS_CHILD | WS_VISIBLE | SS_CENTER, 60, 315, 80, 20, 
	main_wnd_instance, (HMENU) CHILD_ORIGINAL_STATIC, GetModuleHandle(NULL), NULL);
	SetWindowText(original_static, "Pre-Cipher" );
	return original_static;
}

//Create a small patch of text on the screen which labels the post-cipher text box.
HWND init_preview_static(HWND main_wnd_instance)
{
	HWND preview_static=NULL;
	preview_static = CreateWindowEx( 0, "STATIC", "", WS_CHILD | WS_VISIBLE | SS_CENTER, 260, 315, 80, 20, 
	main_wnd_instance, (HMENU) CHILD_PREVIEW_STATIC, GetModuleHandle(NULL), NULL);
	SetWindowText(preview_static, "Post-Cipher" );
	return preview_static;
}

#endif