BINPATH = bin/
OBJSPATH = obj/
LIBPATH = lib/
SRCPATH = src/

MkO = $(OBJSPATH)$(1).o
MkC = $(SRCPATH)$(1).cpp
MkH = $(SRCPATH)$(1).hpp

OBJS = $(call MkO,checker) \
		$(call MkO,clause) \
		$(call MkO,database) \
		$(call MkO,extra) \
		$(call MkO,hashtable) \
		$(call MkO,latency) \
		$(call MkO,model) \
		$(call MkO,parser) \
		$(call MkO,proof) \
		$(call MkO,recheck) \
		$(call MkO,revision) \
		$(call MkO,rupee) \
		$(call MkO,watchlist) \
		$(call MkO,witness)

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
CPPFLAGS  = -g -O2 -Wall -std=c++11 -c $(VERBOSEFLAG)

# VERBOSEFLAG = -DVERBOSE
VERBOSEFLAG =

#output name
EXECUTABLE = $(BINPATH)rupee

CHECKER = $(BINPATH)lratcheck

SICK = $(BINPATH)sickcheck

# COQLRAT = $(BINPATH)coq-lratcheck.native

$(EXECUTABLE) : directories $(OBJS) $(call MkO,lratcheck) $(call MkO,sickcheck)
	@$(LD) $(OBJS) $(LIBS) $(LDFLAGS) $(EXECUTABLE)
	@echo "\nBinary created in $(EXECUTABLE)\n"
	@$(LD) $(call MkO,lratcheck) $(LIBS) $(LDFLAGS) $(CHECKER)
	@echo "\nBinary created in $(CHECKER)\n"
	@$(LD) $(call MkO,sickcheck) $(LIBS) $(LDFLAGS) $(SICK)
	@echo "\nBinary created in $(SICK)\n"

$(call MkO,checker) : $(call MkC,checker) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,checker) -o $(call MkO,checker)

$(call MkO,clause) : $(call MkC,clause) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,clause) -o $(call MkO,clause)

$(call MkO,database) : $(call MkC,database) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,database) -o $(call MkO,database)

$(call MkO,extra) : $(call MkC,extra) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,extra) -o $(call MkO,extra)

$(call MkO,hashtable) : $(call MkC,hashtable) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,hashtable) -o $(call MkO,hashtable)

$(call MkO,latency) : $(call MkC,latency) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,latency) -o $(call MkO,latency)

$(call MkO,model) : $(call MkC,model) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,model) -o $(call MkO,model)

$(call MkO,parser) : $(call MkC,parser) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,parser) -o $(call MkO,parser)

$(call MkO,proof) : $(call MkC,proof) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,proof) -o $(call MkO,proof)

$(call MkO,revision) : $(call MkC,revision) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,revision) -o $(call MkO,revision)

$(call MkO,recheck) : $(call MkC,recheck) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,recheck) -o $(call MkO,recheck)

$(call MkO,watchlist) : $(call MkC,watchlist) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,watchlist) -o $(call MkO,watchlist)

$(call MkO,witness) : $(call MkC,witness) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,witness) -o $(call MkO,witness)

$(call MkO,rupee) : $(call MkC,rupee) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,rupee) -o $(call MkO,rupee)

$(call MkO,lratcheck) : $(call MkC,lratcheck)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,lratcheck) -o $(call MkO,lratcheck)

$(call MkO,sickcheck) : $(call MkC,sickcheck)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,sickcheck) -o $(call MkO,sickcheck)

clean :
	@rm $(OBJS) $(EXECUTABLE)

directories :
	@mkdir -p bin
	@mkdir -p obj

runexperiment:
	 cat instance-2016 | while read line ; do ./experiment.py $line ; done
