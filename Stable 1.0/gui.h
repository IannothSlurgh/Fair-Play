#ifndef GUI_GUARD
#define GUI_GUARD

#include <windows.h>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <fstream>
#include <map>
#include "options.h"
#include "playfair.h"
#include "digraph.h"
#include "cipher.h"
#include "encoder.h"
#include "decoder.h"

#define QUIT_MESSAGE 9000
#define RUN_MESSAGE 9001
#define VALIDATE_MESSAGE 9002
#define ABOUT_MESSAGE 9003

#define CHILD_KEYWORD 8999
#define CHILD_IGNORE 8998
#define CHILD_INPUT 8997
#define CHILD_OUTPUT 8996
#define CHILD_MODE 8995
#define CHILD_ENCODE 8994
#define CHILD_DECODE 8993
#define CHILD_TEXT_KEYWORD 8992
#define CHILD_TEXT_IGNORE 8991
#define CHILD_TEXT_INPUT 8990
#define CHILD_TEXT_OUTPUT 8989

#define ERR_IGNORE_PRESENT 32
#define ERR_BAD_KEYWORD 16
#define ERR_CANNOT_OUTPUT 8
#define ERR_NONALPHA_IGNORE 4
#define ERR_BAD_INPUT 2
#define ERR_MISSING_FIELD 1

using namespace std;

//HELPER FUNCTIONS
string insert_num(const string s, int num); //If able, insert number in place of first seen %i.
void clear_argv(char ** old); //Deletes both raw and processed argvs.
bool is_ignore_in(char * input, char ignore);
bool is_bad_input(char * input);
bool is_bad_output(char * output);
bool is_bad_keyword(char * keyword, char ignore);
 
//MESSAGE PROCESSING
LRESULT CALLBACK main_procedure(HWND main_wnd_instance, UINT msg, WPARAM param_w, LPARAM param_l);
int message_loop();

//ERROR HANDLING
bool check_error();
void gui_error();
void field_errors(HWND main_wnd_instance, int error_state);

//INITIALIZATION
ATOM reg_main(HINSTANCE app_instance);
HWND init_main_window(HINSTANCE app_instance);
HMENU init_main_menu(HWND main_wnd_instance);
//Edit Boxes
HWND init_keyword(HWND main_wnd_instance);
HWND init_ignore(HWND main_wnd_instance);
HWND init_input(HWND main_wnd_instance);
HWND init_output(HWND main_wnd_instance);
//Radio Buttons
HWND init_mode(HWND main_wnd_instance);
HWND init_encode(HWND main_wnd_instance);
HWND init_decode(HWND main_wnd_instance);
//Statics
HWND init_text_keyword(HWND main_wnd_instance);
HWND init_text_ignore(HWND main_wnd_instance);
HWND init_text_input(HWND main_wnd_instance);
HWND init_text_output(HWND main_wnd_instance);

//FUNCTIONALITY
char** get_argv(HWND main_wnd_instance); //Creates matrix containing all arguments for operations in raw fmt.
char** fix_argv(char ** raw); //Take the gotten argv and process it so that options class will not fail.
int validate(char ** raw, char ** processed); //Provide an integer that binarily contains errors.
//Fix validate.
#endif