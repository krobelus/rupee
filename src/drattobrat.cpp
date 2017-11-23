#include <stdlib.h>
#include <fstream>
#include <iostream>

FILE* input;
FILE* output;
bool inclause;
int parsed;
int number;
unsigned int unumber;
int remainder;

int main(int argc, char* argv[]) {
	input = std::fopen(argv[1], "r");
	output = std::fopen(argv[2], "w");

	inclause = false;
	while(true) {
		if(!inclause) {
			if((parsed = fscanf(input, " d %i ", &number)) == 1) {
				fputc(100, output);
				std::cout << "print " << 100 << std::endl;
			} else if((parsed = fscanf(input, " %i ", &number)) == 1) {
				fputc(97, output);
				std::cout << "print " << 97 << std::endl;
			} else if(parsed == EOF) {
				std::fclose(input);
				std::fclose(output);
				return 1;
			} else {
				std::cerr << "Parsing error" << std::endl;
				return 0;
			}
			inclause = true;
		} else {
			if((parsed = fscanf(input, " %i ", &number)) != 1) {
				if(parsed == EOF) {
					std::fclose(input);
					std::fclose(output);
					return 1;
				} else {
					std::cerr << "Parsing error" << std::endl;
					return 0;
				}
			}
		}
		std::cout << "read " << number << std::endl;
		if(number == 0) {
			fputc(0, output);
			std::cout << "print " << 0 << std::endl;
			inclause = false;
		} else {
			if(number > 0) {
				unumber = number * 2;
			} else {
				unumber = number * -2 + 1;
			}
			std::cout << "transform " << unumber <<std::endl;
			do {
				remainder = unumber & 127;
				std::cout << "restrict " << remainder <<std::endl;
				unumber = unumber >> 7;
				if(unumber > 0) {
					remainder |= 128;
				}
				std::cout << "rest " << unumber << std::endl;
				fputc(remainder, output);
				std::cout << "print " << remainder << std::endl;
			} while(unumber > 0);
		}
	}
}