#include <iostream>

#include "extra.hpp"
#include "watchlist.hpp"
#include "database.hpp"

bool allocate(database& d) {
	Blablabla::log("Allocating clause database.");
	d.databasemax = Parameters::databaseSize;
	d.databasearray = (int*) malloc (d.databasemax * sizeof(int));
	d.databaseused = 0;
	if(d.databasearray == NULL) {
		Blablabla::log("Error at clause database allocation.");
		Blablabla::comment("Memory management error.");
		return false;
	} else {
		return true;
	}
}

void reallocate() {
    Blablabla::log("Reallocating clause database.");
    d.databasemax = (d.databasemax * 3) >> 1;
    d.databasearray = (int*) realloc (d.databasearray, d.databasemax * sizeof(int));
	(int*) realloc (b.bufferarray, b.buffermax * sizeof(int));
	if(b.bufferarray == NULL) {
		Blablabla::log("Error at parse buffer reallocation.");
		Blablabla::comment("Memory management error.");
		return false;
	} else {
		return true;
	}
	std::cout << "Reallocating clause database." << std::endl;
	max = Tools::leap(max);
	db = (int*) realloc (db, max * sizeof(int));
}
