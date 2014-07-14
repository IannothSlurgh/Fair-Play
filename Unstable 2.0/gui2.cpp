#ifndef GUI2_GUARD
#define GUI2_GUARD
#include <iostream>
#include <sstream>
//#include <cstring>
#include "gui2.h"

//Declared in main.cpp
extern bool has_saved, has_opened, discard;
extern int precipher_size;
extern char * source_filename, * dest_filename, * precipher, * postcipher, * keyword, ignore, mode;

using namespace std;

//Opens a file explorer screen to get an open file name, or a save filename.
//Warning!!!!: Dynamic array returned, delete[] it.
char * get_file_path( HWND main_wnd_instance, bool is_open )
{
	bool success;
	char * c = 0;
	char filename[ MAX_PATH ]="";
	OPENFILENAME file;
	ZeroMemory( &file, sizeof( file ) );
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
		success = GetOpenFileName( &file );
	}
	else
	{
		success = GetSaveFileName( &file );
	}
	int size = strlen( filename );
	if(success)
	{
		//This must be cleared later.
		c = new char[ size + 1 ];
		//Get filename from explorer and return the result.
		strcpy( c, filename );
	}
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
		precipher_size= GetFileSize( file, NULL );
		if( precipher_size != INVALID_FILE_SIZE )
		{
			//If the file is not too large, try and read in the pre-cipher text.
			precipher= new (std::nothrow) char[ precipher_size + 1 ];
			if( precipher != 0 )
			{
				//If there was enough memory, read in the data and cap it off with null terminator.
				DWORD bytes_read;
				if( ReadFile( file, precipher, precipher_size, &bytes_read, NULL ) )
				{
					precipher[ precipher_size ] = 0;
					success = true;
				}
			}
		}
	}
	//File system no longer needed.
	CloseHandle( file );
	return success;
}

bool get_postcipher( HWND preview_box )
{
	int len_postcipher = GetWindowTextLength( preview_box );
	postcipher = new char[ len_postcipher + 1 ];
	GetWindowText( preview_box, postcipher, len_postcipher );
	postcipher[ len_postcipher ] = 0;
	return true;
}

bool write_file( HWND main_wnd_instance )
{
	HWND preview_box = GetDlgItem( main_wnd_instance, CHILD_PREVIEW );
	HANDLE file = CreateFile( dest_filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	bool success = false;
	if( file != INVALID_HANDLE_VALUE )
	{
		if( get_postcipher( preview_box ) )
		{
			int len_postcipher = strlen( postcipher );
			DWORD len_written;
			if( WriteFile( file, postcipher, len_postcipher, &len_written, NULL ) )
			{
				success = true;
			}
			delete[] postcipher;
			postcipher = 0;
		}
	}
	CloseHandle( file );
	return success;
}

char settings_possible_ignore( HWND settings_instance )
{
	char buffer_ignore[2];
	//Len not including '/0'
	int len_ignore = GetWindowText( GetDlgItem( settings_instance, CHILD_IGNORE ), buffer_ignore, 2 );
	buffer_ignore[ 1 ] = 0;
	char possible_ignore;
	if( len_ignore == 0 )
	{
		possible_ignore = 0;
	}
	possible_ignore = toupper( buffer_ignore[ 0 ] );
	return possible_ignore;
}

char settings_possible_mode( HWND settings_instance )
{
	char possible_mode;
	if( IsDlgButtonChecked( settings_instance, CHILD_ENCODING ) == BST_CHECKED )
	{
		possible_mode = 'E';
	}
	else
	{
		possible_mode = 'D';
	}
	return possible_mode;
}

char * settings_possible_keyword( HWND settings_instance )
{
	char * possible_keyword;
	char buffer_keyword[26];
	//Len not including '/0'
	int len_keyword = GetWindowText( GetDlgItem( settings_instance, CHILD_KEYWORD ), buffer_keyword, 26 );
	buffer_keyword[ 25 ] = 0;
	if( len_keyword != 0 )
	{
		possible_keyword = new char[ len_keyword + 1 ];
		for( int i = 0; i < len_keyword; ++i )
		{
			possible_keyword[ i ] = toupper( buffer_keyword[ i ] );
		}
		possible_keyword[ len_keyword ] = 0;
	}
	else
	{
		possible_keyword = 0;
	}
	return possible_keyword;
}

void settings_error( HWND caller, int state )
{
	//if( state & MISSING_KEYWORD )
	//{
	//	MessageBox( caller, "Error", "Fair Play", MB_YESNOCANCEL );
	//}
	stringstream ss;
	ss << state;
	string blah;
	ss >> blah;
	MessageBox( caller, blah.c_str(), "Fair Play", MB_YESNOCANCEL );
}

void out_of_memory( HWND caller )
{
	MessageBox( caller, "Memory Crisis", "Error", MB_OK | MB_ICONERROR );
}

void settings_apply_button( HWND settings_instance )
{
	HWND main_wnd_instance = GetWindow( settings_instance, GW_OWNER );
	char possible_ignore = settings_possible_ignore( settings_instance );
	//Get the user input into format, options_win validator understands.
	char possible_mode = settings_possible_mode( settings_instance );
	char * possible_keyword = settings_possible_keyword( settings_instance );
	char * empty_file = 0;
	//Check options without major memory expenditure (without reference to the precipher).
	options_win apply_validation( possible_mode, possible_keyword, empty_file, possible_ignore, 0 );
	if( !apply_validation.bad )
	{
		//Try cipher on open-file.
		if( has_opened )
		{
			precipher = new (std::nothrow) char[ precipher_size + 1 ];
			//No memory overuse.
			if( precipher != 0 )
			{
				//Get precipher into options.
				GetWindowText( GetDlgItem( main_wnd_instance, CHILD_ORIGINAL ), precipher, precipher_size );
				precipher[ precipher_size ] = 0;
				apply_validation.set_file( precipher, precipher_size );
				apply_validation.validate();
				//Check ignore in the text
				if( !apply_validation.bad )
				{
					playfair_win convert( apply_validation );
					convert.execute();
					if( convert.postcipher != 0 )
					{
						//Cipher was completely successful
						SetWindowText( GetDlgItem( main_wnd_instance, CHILD_PREVIEW ), convert.postcipher );
						EnableWindow( GetDlgItem( settings_instance, CHILD_APPLY ), FALSE );
						has_saved = false;
					}
					else
					{
						out_of_memory( settings_instance );
					}
					//Even if memory ran short, the settings are good, make them default.
					delete[] keyword;
					keyword = possible_keyword;
					ignore = possible_ignore;
					mode = possible_mode;
				}
				else
				{
					settings_error( settings_instance, apply_validation.state );
				}
				delete[] precipher;
				precipher = 0;
			}
			else
			{
				out_of_memory( settings_instance );
			}
		}
		else
		{
			//Apply was successful, no file to use them on. Default settings set.
			delete[] keyword;
			ignore = possible_ignore;
			mode = possible_mode;
			keyword = possible_keyword;
			EnableWindow( GetDlgItem( settings_instance, CHILD_APPLY ), FALSE );
		}
	}
	else
	{
		settings_error( settings_instance, apply_validation.state );
	}
	if( possible_keyword != keyword )
	{
		delete[] possible_keyword;
	}	
}

//Rather unfinished procedure for a secondary window for settings (in which mode, ignore, etc could be set.)
BOOL CALLBACK Playfair_Settings_Proc( HWND settings_instance, UINT Message, WPARAM param_w, LPARAM param_l )
{
	switch( Message )
	{
		case WM_INITDIALOG:
			//Repeated User controls
			HWND handle_keyword;
			HWND handle_ignore;
			handle_keyword = GetDlgItem( settings_instance, CHILD_KEYWORD );
			handle_ignore = GetDlgItem( settings_instance, CHILD_IGNORE );
			//Remember last settings.
			char c[2];
			c[0]=ignore;
			c[1]=0;
			SetWindowText( handle_keyword, keyword );
			SetWindowText( handle_ignore, c );
			if(mode == 'E')
			{
				PostMessage( GetDlgItem( settings_instance, CHILD_ENCODING ), BM_SETCHECK, TRUE, 0 );
			}
			else
			{
				PostMessage( GetDlgItem( settings_instance, CHILD_DECODING ), BM_SETCHECK, TRUE, 0 );
			}
			//Prevent impossible keywords and ignores by length.
			PostMessage(handle_keyword, EM_LIMITTEXT, 25, 0);
			PostMessage(handle_ignore, EM_LIMITTEXT, 1, 0);
			//Apply Changes button is disabled by default.
			EnableWindow( GetDlgItem( settings_instance, CHILD_APPLY ), FALSE );
			return TRUE;
		break;
		case WM_CLOSE:
			EndDialog(settings_instance, IDOK);
		break;
		case WM_COMMAND:
			switch( LOWORD( param_w ) )
			{
				case IDOK:
						EndDialog(settings_instance, IDOK);
				break;
				case CHILD_APPLY:
					settings_apply_button( settings_instance );
				break;
				case CHILD_IGNORE:
					case CHILD_KEYWORD:
						if( HIWORD( param_w ) == EN_CHANGE )
						{
							EnableWindow( GetDlgItem( settings_instance, CHILD_APPLY ), TRUE );
						}
				break;
				case CHILD_DECODING:
					case CHILD_ENCODING:
						if( HIWORD( param_w ) == BN_CLICKED )
						{
							EnableWindow( GetDlgItem( settings_instance, CHILD_APPLY ), TRUE );
						}
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
				if( temp == IDYES )
				{
					if( dest_filename == 0 )
					{
						SendMessage( main_wnd_instance, WM_COMMAND, SAVE_AS_MESSAGE, 0 );
					}
					else
					{
						SendMessage( main_wnd_instance, WM_COMMAND, SAVE_MESSAGE, 0 );
					}
					DestroyWindow( main_wnd_instance );
				}
				if( temp == IDNO )
				{
					DestroyWindow( main_wnd_instance );
				}
				if( temp == IDCANCEL )
				{
					break;
				}
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
					if( temp == IDYES )
					{
						SendMessage( main_wnd_instance, WM_COMMAND, SAVE_MESSAGE, 0 );
					}
					if( temp == IDNO )
					{
						discard = true;
					}
					if( temp == IDCANCEL )
					{
						break;
					}
					if( has_saved || discard || !has_opened )
					{
						//Use explorer to get open filename.
						source_filename = get_file_path( main_wnd_instance, true );
						//If 0, then error or user cancel
						if( source_filename != 0 )
						{
							//Grab text in specified file.
							if( get_precipher() )
							{
								//Plop it in pre-cipher text box.
								SetWindowText( GetDlgItem( main_wnd_instance,
								CHILD_ORIGINAL ), precipher );
								options_win validator( mode, keyword, precipher, ignore, strlen( precipher ) );
								if( !validator.bad )
								{
									playfair_win convert( validator );
									convert.execute();
									SetWindowText( GetDlgItem( main_wnd_instance, CHILD_PREVIEW ), convert.postcipher );
								}
								else
								{
									SetWindowText( GetDlgItem( main_wnd_instance, CHILD_PREVIEW ), "Your current cipher settings are incompatible with this file." );
								}
							}
							//A successful opening of a new file means new save path.
							delete[] dest_filename;
							dest_filename = 0;
							delete[] source_filename;
							source_filename = 0;
							delete[] precipher;
							precipher = 0;
							has_opened = true;
							has_saved=discard=false;
						}
					}
				}
				break;
				//Save submenu pressed.
				case SAVE_MESSAGE:
				{
					if( dest_filename == 0 )
					{
						PostMessage( main_wnd_instance, WM_COMMAND, SAVE_AS_MESSAGE, 0 );
					}
					else
					{
						if( write_file( main_wnd_instance ) )
						{
							has_saved = true;
						}
						else
						{
							//Error
						}
					}
				}
				break;
				//Save as submenu pressed.
				case SAVE_AS_MESSAGE:
				{
					char * temp_dest;
					//Come back here and fix this!
					temp_dest = get_file_path( main_wnd_instance, false );
					if( temp_dest != 0 )
					{
						dest_filename = temp_dest;
						if( write_file( main_wnd_instance ) )
						{
							has_saved = true;
						}
						else
						{
							//Error
						}
					}
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