What is Fair Play?

Fair Play is a GUI application capable of encoding and decoding text files with a playfair cipher written for Windows. While the playfair cipher is not the most secure cipher system, under certain circumstances it may prove difficult to crack (little source text, few obvious phrases, unknown settings, intruder doesn't know cipher, etc). Even so, if you intend to use Fair Play, the programmer would like to caution you that there are much more secure ciphers out there. 

What is a playfair cipher?

A playfair cipher is a digraph substitution cipher which has limitations depending on which version is being used. Some versions allow all 26 letters of the alphabet while others allow only 25. Regardless, the cipher involves a 5x5 matrix in which the letters of the latin alphabet are placed. They letters are placed in from left-to-right, top-to-bottom until the matrix is full. A play fair cipher will change the order in which the letters are placed in by means of a keyword. The keyword must be full of unique letters (words like noon, good, happy, etc will not work because of having two of one letter). The letters of the keyword are placed in the matrix first, followed by the rest of the alphabet starting from A to Z. 

How does Fair Play work?

When opened, the program will display keyword, input, output, and ignore text fields.

Platform and Compilation Details

Fair Play was written in C++ on Windows 7 for Windows operating systems. It has been compiled on the MinGW port of gcc with the following line: 

g++ main.cpp gui.cpp playfair.cpp options.cpp cipher.cpp encoder.cpp decoder.cpp digraph.cpp
 -mwindows -O3 -o playfair.exe -static-libstdc++ -static-libgcc
 
 You are advised to try this line to decrease the size of Fair Play:
 
 g++ main.cpp gui.cpp playfair.cpp options.cpp cipher.cpp encoder.cpp decoder.cpp digraph.cpp
 -mwindows -O3 -o playfair.exe
