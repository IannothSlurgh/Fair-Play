------------------
What is Fair Play?
------------------

Fair Play is a GUI application capable of encoding and decoding text files with a playfair cipher written for Windows. While the playfair cipher is not the most secure cipher system, under certain circumstances it may prove difficult to crack (little source text, few obvious phrases, unknown settings, intruder doesn't know cipher, etc). Even so, if you intend to use Fair Play, the programmer would like to caution you that there are much more secure ciphers out there. 

-------------------------
What is a playfair cipher?
-------------------------

A playfair cipher is a digraph substitution cipher which has limitations depending on which version is being used. Some versions allow all 26 letters of the alphabet while others allow only 25. Regardless, the cipher involves a 5x5 matrix in which the letters of the latin alphabet are placed. The letters are placed in from left-to-right, top-to-bottom until the matrix is full. A playfair cipher will change the order in which the letters are placed in by means of a keyword. The keyword must be full of unique letters (words like noon, good, happy, etc will not work because of having two of one letter). The letters of the keyword are placed in the matrix first, followed by the rest of the alphabet starting from A to Z. The text that is to be encoded is then chopped up into two letter groups known as digraphs. Each digraph is altered based on the position of its two letters relative to one another in the matrix.

------------------------
What is a valid keyword?
------------------------

The word must be only latin letters (ASCII compliant), not have any duplicate letters, and lack non-letter characters.

----------------------------
What is the ignore field for?
----------------------------

Fair Play currently only allows messages with 25 of the 26 latin letters. The user must pick which letter is excluded.

-----------------------------------
Where should my input files reside?
-----------------------------------

The current version uploaded here requires that input files be in the same directory as the executeable. Similarly, Fair Play will put the output file to the directory in which its executeable resides.

----------------------------------------------------------------------------------------------------
When I decode a text file with the right settings and keyword, oddly placed Q's, X's, or Z's appear.
----------------------------------------------------------------------------------------------------

This is a normal, but somewhat annoying aspect of playfair ciphers. Fair Play doesn't have a good way to know which Q's, X's, or Z's were created by the encoding, and which ones were part of the original text. Again, this is normal. For more details, go look up the rules used for encoding in a Playfair cipher.

--------------------------
What, no case sensitivity?
--------------------------

No. No case sensitivity.

--------------------------------
Platform and Compilation Details
--------------------------------

Fair Play was written in C++ on Windows 7 for Windows XP and above (there is a fair possibility it may work on 2000 also). The current stable version (1.0) has been compiled on the MinGW port of gcc with the following line: 

g++ main.cpp gui.cpp playfair.cpp options.cpp cipher.cpp encoder.cpp decoder.cpp digraph.cpp
 -mwindows -O3 -o playfair.exe -static-libstdc++ -static-libgcc
 
 You are advised to try this line to decrease the size of Fair Play:
 
 g++ main.cpp gui.cpp playfair.cpp options.cpp cipher.cpp encoder.cpp decoder.cpp digraph.cpp
 -mwindows -O3 -o playfair.exe
 
Version 2.0 includes a windows resource file which must be compiled in the gcc resource manager.

For 2.0, use the following lines.

windres resource.rc resource.o

g++ main.cpp gui2.cpp playfair_win.cpp options_win.cpp cipher.cpp encoder.cpp decoder.cpp digraph.cpp resource.o -mwindows -O3 -o fairplay.exe -static-libstdc++ -static-libgcc

If possible, exclude the -static-libstdc++ -static-libgcc arguments.
