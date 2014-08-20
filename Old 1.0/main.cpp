#include <windows.h>
#include <iostream>
#include "gui.h"

int app_state=0; //If not 0, Something bad has happened.

int WINAPI WinMain(HINSTANCE app_instance, HINSTANCE previnstance, LPSTR lpcmdline, int ncmdshow)
{
	reg_main(app_instance);
	HWND main_wnd_instance = init_main_window(app_instance);
	init_keyword(main_wnd_instance);
	init_ignore(main_wnd_instance);
	init_input(main_wnd_instance);
	init_output(main_wnd_instance);
	init_mode(main_wnd_instance);
	init_encode(main_wnd_instance);
	init_decode(main_wnd_instance);
	init_text_output(main_wnd_instance);
	init_text_input(main_wnd_instance);
	init_text_keyword(main_wnd_instance);
	init_text_ignore(main_wnd_instance);
	if(!check_error()) //Initialization was successful.
		return message_loop();
	cerr<<GetLastError();
}