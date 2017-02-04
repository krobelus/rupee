BINPATH = bin/
OBJSPATH = obj/
LIBPATH = lib/
SRCPATH = src/

MkO = $(OBJSPATH)$(1).o
MkC = $(SRCPATH)$(1).cpp
MkH = $(SRCPATH)$(1).hpp

OBJS = $(call MkO,assignment) \
		$(call MkO,chain) \
		$(call MkO,database) \
		$(call MkO,extra) \
		$(call MkO,hashtable) \
		$(call MkO,parser) \
		$(call MkO,proof) \
		$(call MkO,reprocess) \
		$(call MkO,resolvent) \
		$(call MkO,solver) \
		$(call MkO,watchlist) \
		$(call MkO,main)

###  g++ -std=c++11 assignment.cpp chain.cpp $(call MkC,extra) $(call MkC,hashtable) $(call MkC,parser) $(call MkC,proof) $(call MkC,watchlist) $(call MkC,database) micetrim.cpp -lboost_regex -o micetrim

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
EXECUTABLE = $(BINPATH)rupee

$(EXECUTABLE) : directories $(OBJS)
	$(LD) $(OBJS) $(LIBS) $(LDFLAGS) $(EXECUTABLE)

$(call MkO,assignment) : $(call MkC,assignment) $(call MkH,assignment) $(call MkH,database) $(call MkH,watchlist) $(call MkH,chain) $(call MkH,reprocess) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,assignment) -o $(call MkO,assignment)

$(call MkO,chain) : $(call MkC,chain) $(call MkH,chain) $(call MkH,assignment) $(call MkH,database) $(call MkH,watchlist) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,chain) -o $(call MkO,chain)

$(call MkO,database) : $(call MkC,database) $(call MkH,database) $(call MkH,extra) $(call MkH,watchlist)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,database) -o $(call MkO,database)

$(call MkO,extra) : $(call MkC,extra) $(call MkH,extra) $(call MkH,database) $(call MkH,assignment) $(call MkH,chain) $(call MkH,reprocess) $(call MkH,watchlist)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,extra) -o $(call MkO,extra)

$(call MkO,hashtable) : $(call MkC,hashtable) $(call MkH,hashtable) $(call MkH,extra) $(call MkH,database)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,hashtable) -o $(call MkO,hashtable)

$(call MkO,parser) : $(call MkC,parser) $(call MkH,parser) $(call MkH,extra) $(call MkH,database) $(call MkH,proof) $(call MkH,hashtable) $(call MkH,assignment) $(call MkH,solver) $(call MkH,chain) $(call MkH,reprocess) $(call MkH,resolvent) $(call MkH,watchlist)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,parser) -o $(call MkO,parser)

$(call MkO,proof) : $(call MkC,proof) $(call MkH,proof) $(call MkH,extra) $(call MkH,database)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,proof) -o $(call MkO,proof)

$(call MkO,reprocess) : $(call MkC,reprocess) $(call MkH,reprocess) $(call MkH,assignment) $(call MkH,database) $(call MkH,watchlist) $(call MkH,chain) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,reprocess) -o $(call MkO,reprocess)

$(call MkO,resolvent) : $(call MkC,resolvent) $(call MkH,resolvent) $(call MkH,database) $(call MkH,watchlist) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,resolvent) -o $(call MkO,resolvent)

$(call MkO,solver) : $(call MkC,solver) $(call MkH,solver) $(call MkH,assignment) $(call MkH,chain) $(call MkH,proof) $(call MkH,reprocess) $(call MkH,watchlist) $(call MkH,resolvent) $(call MkH,database) $(call MkH,extra) $(call MkH,solver)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,solver) -o $(call MkO,solver)

$(call MkO,watchlist) : $(call MkC,watchlist) $(call MkH,watchlist) $(call MkH,extra) $(call MkH,database) $(call MkH,assignment) $(call MkH,chain) $(call MkH,reprocess)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,watchlist) -o $(call MkO,watchlist)

$(call MkO,main) : $(call MkC,main) $(call MkH,parser) $(call MkH,database) $(call MkH,proof) $(call MkH,watchlist) $(call MkH,solver) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,main) -o $(call MkO,main)

clean :
	rm $(OBJS) $(EXECUTABLE)

directories :
	mkdir -p bin
	mkdir -p obj
