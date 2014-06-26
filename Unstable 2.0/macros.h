#ifndef MACROS_GUARD
#define MACROS_GUARD

//options_win error code macros.

#define MISSING_MODE 0x1;
#define MISSING_KEYWORD 0x2;
#define MISSING_IGNORE 0x4;
#define NONALPHA_KEYWORD 0x200;
#define IGNORE_IN_KEYWORD 0x400;
#define IGNORE_IN_FILE 0x800;

#define MISSING_FIELD_BITS 4

//playfair_win error code macros.

#define BAD_DIGRAPH_TRANSLATE 0x400
#define NONALPHA_OVERFLOW 0x800

#define DIGRAPH_A_BITS 5

//gui2 messagebox macros

#define HAVE_NOT_SAVED "You have not saved your current file. Do you want to save it?"

#endif