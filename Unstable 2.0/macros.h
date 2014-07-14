#ifndef MACROS_GUARD
#define MACROS_GUARD

//options_win error code macros.

#define MISSING_MODE 0x1;
#define MISSING_KEYWORD 0x2;
#define MISSING_IGNORE 0x4;
#define NONALPHA_KEYWORD 0x200;
#define IGNORE_IN_KEYWORD 0x400;
#define IGNORE_IN_FILE 0x800;
#define NONALPHA_IGNORE 0x1000

#define MISSING_FIELD_BITS 4

//playfair_win error code macros.

#define BAD_DIGRAPH_TRANSLATE 0x400
#define NONALPHA_OVERFLOW 0x800

#define DIGRAPH_A_BITS 5

//gui2 messagebox macros

#define HAVE_NOT_SAVED "You have not saved your current file. Do you want to save it?"
#define MISSING_KEYWORD_TEXT "You have not entered a keyword, your options could not be applied."
#define MISSING_IGNORE_TEXT "You have not entered an ignored character, your options could not be applied."
#define BAD_KEYWORD_TEXT "Your keyword is not made only of unique letters, your options could not be applied."

#define IDD_CONFIG_CIPHER 101
#define CHILD_APPLY 8986
#define CHILD_MODE_BOX 8987
#define CHILD_TEXT_IGNORE 8988
#define CHILD_TEXT_KEYWORD 8989
#define CHILD_TEXT_DECODING 8990
#define CHILD_TEXT_ENCODING 8991
#define CHILD_ENCODING 8992
#define CHILD_DECODING 8993
#define CHILD_KEYWORD 8994
#define CHILD_IGNORE 8995

#endif