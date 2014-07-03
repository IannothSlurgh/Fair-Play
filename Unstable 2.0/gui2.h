#include <windows.h>
#include "macros.h"
#include "playfair_win.h"

#define CHILD_ORIGINAL_STATIC 8996
#define CHILD_PREVIEW_STATIC 8997
#define CHILD_PREVIEW 8998
#define CHILD_ORIGINAL 8999

#define OPEN_MESSAGE 9000
#define SAVE_MESSAGE 9001
#define SAVE_AS_MESSAGE 9002
#define SEND_MESSAGE 9003
#define RECEIVE_MESSAGE 9004
#define EXIT_MESSAGE 9005
#define CONFIG_CIPHER_MESSAGE 9006
#define CONFIG_NETWORKING_MESSAGE 9007
#define ABOUT_MESSAGE 9008
#define HELP_MESSAGE 9009

//On is_open true, get a source filename, on false get a destination filename.
//Warning!!!!: Dynamic array returned, delete[] it.
char * get_file_path(HWND main_wnd_instance, bool is_open);

//Fair Play's main window's procedure.
LRESULT CALLBACK main_procedure(HWND main_wnd_instance, UINT msg, WPARAM param_w, LPARAM param_l);

//Rather unfinished procedure for a secondary window for settings (in which mode, ignore, etc could be set.)
BOOL CALLBACK Playfair_Settings_Proc( HWND settings_instance, UINT Message, WPARAM param_w, LPARAM param_l );

//Register the main window class.
ATOM reg_main(HINSTANCE app_instance);

//Create the main window, entitled "Fair Play"
HWND init_main_window(HINSTANCE app_instance);

//Create top bar menu and associate each option with a message.
HMENU init_main_menu(HWND main_wnd_instance);

//Create uneditable text box which contains what is in an opened file.
HWND init_original_edit(HWND main_wnd_instance);

//Creates uneditable text box which contains a preview of the post-cipher text.
HWND init_preview_edit(HWND main_wnd_instance);

//Create a small patch of text on the screen which labels the pre-cipher text box.
HWND init_original_static(HWND main_wnd_instance);

//Create a small patch of text on the screen which labels the post-cipher text box.
HWND init_preview_static(HWND main_wnd_instance);

int message_loop();