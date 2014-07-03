
#include "gui2.h"
#include <iostream>

//discard- user decided to discard changes.
bool has_saved, has_opened, discard;

int precipher_size;

//mode- D for decode, E for Encode
char * source_filename, * dest_filename, * precipher, * postcipher, * keyword, ignore, mode;

using namespace std;

int WINAPI WinMain(HINSTANCE app_instance, HINSTANCE previnstance, LPSTR lpcmdlin, int ncmdshow)
{
	//Clean, empty state.
	has_saved=has_opened=discard=source_filename=dest_filename=precipher=postcipher;
	ignore = 'Z';
	mode = 'E';
	keyword = new char[ 8 ];
	keyword[ 0 ] = 'H';
	keyword[ 1 ] = 'E';
	keyword[ 2 ] = 'X';
	keyword[ 3 ] = 'A';
	keyword[ 4 ] = 'P';
	keyword[ 5 ] = 'U';
	keyword[ 6 ] = 'S';
	keyword[ 7 ] = 0;
	//Register Main window class.
	reg_main(app_instance);
	//Open main window
	HWND main_wnd_instance = init_main_window(app_instance);
	//Create two large un-editable text fields on either side of main window
	HWND handle_original_edit = init_original_edit(main_wnd_instance);
	HWND handle_preview_edit = init_preview_edit(main_wnd_instance);
	//Create text identifying the text fields as the original text, and a preview of the edited text.
	HWND handle_original_static = init_original_static(main_wnd_instance);
	HWND handle_preview_static = init_preview_static(main_wnd_instance);
	//If this all went well, wait for user interactions.
	if(GetLastError()==0)
		message_loop();
	cout<<GetLastError();
	delete[] keyword;
	return 0;
}