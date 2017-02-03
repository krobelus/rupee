OBJS = $(OBJSPATH)assignment.o \
		$(OBJSPATH)chain.o \
		$(OBJSPATH)database.o \
		$(OBJSPATH)extra.o \
		$(OBJSPATH)hashtable.o \
		$(OBJSPATH)parser.o \
		$(OBJSPATH)proof.o \
		$(OBJSPATH)reprocess.o \
		$(OBJSPATH)resolvent.o \
		$(OBJSPATH)solver.o \
		$(OBJSPATH)watchlist.o \
		$(OBJSPATH)micetrim.o

###  g++ -std=c++11 assignment.cpp chain.cpp extra.cpp hashtable.cpp parser.cpp proof.cpp watchlist.cpp database.cpp micetrim.cpp -lboost_regex -o micetrim

OBJSPATH = objs/

# Linker
LD = g++

# Linker flags
LDFLAGS = -o

# Libraries
LIBS = -lboost_regex

# C++ compiler
CPP = g++

# compiler flags:
#  -g			adds debugging information to the executable file
#  -Wall		turns on most, but not all, compiler warnings
#  -O2			turns on optimizations
#  -std=c++11	uses C++11 as a language
CPPFLAGS  = -g -Wall -std=c++11 -c

#output name
EXECUTABLE = micetrim

$(EXECUTABLE) : $(OBJS)
	$(LD) $(OBJS) $(LIBS) $(LDFLAGS) $(EXECUTABLE)

$(OBJSPATH)assignment.o : assignment.cpp assignment.hpp database.hpp watchlist.hpp chain.hpp reprocess.hpp extra.hpp
	$(CPP) $(LIBS) $(CPPFLAGS) assignment.cpp -o $(OBJSPATH)assignment.o

$(OBJSPATH)chain.o : chain.cpp chain.hpp assignment.hpp database.hpp watchlist.hpp extra.hpp
	$(CPP) $(LIBS) $(CPPFLAGS) chain.cpp -o $(OBJSPATH)chain.o

$(OBJSPATH)database.o : database.cpp database.hpp extra.hpp watchlist.hpp
	$(CPP) $(LIBS) $(CPPFLAGS) database.cpp -o $(OBJSPATH)database.o

$(OBJSPATH)extra.o : extra.cpp extra.hpp database.hpp assignment.hpp chain.hpp reprocess.hpp watchlist.hpp
	$(CPP) $(LIBS) $(CPPFLAGS) extra.cpp -o $(OBJSPATH)extra.o

$(OBJSPATH)hashtable.o : hashtable.cpp hashtable.hpp extra.hpp database.hpp
	$(CPP) $(LIBS) $(CPPFLAGS) hashtable.cpp -o $(OBJSPATH)hashtable.o

$(OBJSPATH)parser.o : parser.cpp parser.hpp extra.hpp database.hpp proof.hpp hashtable.hpp assignment.hpp solver.hpp chain.hpp reprocess.hpp resolvent.hpp watchlist.hpp
	$(CPP) $(LIBS) $(CPPFLAGS) parser.cpp -o $(OBJSPATH)parser.o

$(OBJSPATH)proof.o : proof.cpp proof.hpp extra.hpp database.hpp
	$(CPP) $(LIBS) $(CPPFLAGS) proof.cpp -o $(OBJSPATH)proof.o

$(OBJSPATH)reprocess.o : reprocess.cpp reprocess.hpp assignment.hpp database.hpp watchlist.hpp chain.hpp extra.hpp
	$(CPP) $(LIBS) $(CPPFLAGS) reprocess.cpp -o $(OBJSPATH)reprocess.o

$(OBJSPATH)resolvent.o : resolvent.cpp resolvent.hpp database.hpp watchlist.hpp extra.hpp
	$(CPP) $(LIBS) $(CPPFLAGS) resolvent.cpp -o $(OBJSPATH)resolvent.o

$(OBJSPATH)solver.o : solver.cpp solver.hpp assignment.hpp chain.hpp proof.hpp reprocess.hpp watchlist.hpp resolvent.hpp database.hpp extra.hpp solver.hpp
	$(CPP) $(LIBS) $(CPPFLAGS) solver.cpp -o $(OBJSPATH)solver.o

$(OBJSPATH)watchlist.o : watchlist.cpp watchlist.hpp extra.hpp database.hpp assignment.hpp chain.hpp reprocess.hpp
	$(CPP) $(LIBS) $(CPPFLAGS) watchlist.cpp -o $(OBJSPATH)watchlist.o

$(OBJSPATH)micetrim.o : micetrim.cpp
	$(CPP) $(LIBS) $(CPPFLAGS) micetrim.cpp -o $(OBJSPATH)micetrim.o

clean :
	rm $(OBJS) $(EXECUTABLE)
