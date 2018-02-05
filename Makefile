BINPATH = bin/
OBJSPATH = obj/
SRCPATH = src/

MkO = $(OBJSPATH)$(1).o
MkC = $(SRCPATH)$(1).cpp
MkH = $(SRCPATH)$(1).hpp

RUPEEOBJS = $(call MkO,checker) \
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

CHECKOBJS = $(call MkO,check-core) \
		$(call MkO,check-lexer) \
		$(call MkO,check-hashtable) \
		$(call MkO,check-proof) \
		$(call MkO,check-database) \
		$(call MkO,check-parser)

LD = g++

LDFLAGS = -o

CPP = g++

CPPFLAGS  = -g -O2 -Wall -std=c++17 -c

RUPEEBIN = $(BINPATH)rupee

LRATBIN = $(BINPATH)lratcheck

SICKBIN = $(BINPATH)sickcheck

BRATTODRATBIN = $(BINPATH)brattodrat

all : directories $(RUPEEOBJS) $(call MkO,brattodrat) $(CHECKOBJS) $(call MkO,check-lratchecker) $(call MkO,check-sickchecker)
	@echo "\n"
	@$(LD) $(RUPEEOBJS) $(LIBS) $(LDFLAGS) $(RUPEEBIN)
	@echo "Binary created in $(RUPEEBIN)\n"
	@$(LD) $(call MkO,brattodrat) $(LIBS) $(LDFLAGS) $(BRATTODRATBIN)
	@echo "Binary created in $(BRATTODRATBIN)\n"
	@$(LD) $(CHECKOBJS) $(call MkO,check-lratchecker) $(LIBS) $(LDFLAGS) $(LRATBIN)
	@echo "Binary created in $(LRATBIN)\n"
	@$(LD) $(CHECKOBJS) $(call MkO,check-sickchecker) $(LIBS) $(LDFLAGS) $(SICKBIN)
	@echo "Binary created in $(SICKBIN)\n"
	@echo "\n"

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

$(call MkO,brattodrat) : $(call MkC,brattodrat)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,brattodrat) -o $(call MkO,brattodrat)

$(call MkO,check-core) : $(call MkH,check/core) $(call MkC,check/core)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,check/core) -o $(call MkO,check-core)

$(call MkO,check-database) : $(call MkH,check/core) $(call MkH,check/database) $(call MkC,check/database)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,check/database) -o $(call MkO,check-database)

$(call MkO,check-lexer) : $(call MkH,check/core) $(call MkH,check/lexer) $(call MkC,check/lexer)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,check/lexer) -o $(call MkO,check-lexer)

$(call MkO,check-proof) : $(call MkH,check/core) $(call MkH,check/proof) $(call MkC,check/proof)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,check/proof) -o $(call MkO,check-proof)

$(call MkO,check-hashtable) : $(call MkH,check/core)  $(call MkH,check/database) $(call MkH,check/hashtable) $(call MkC,check/hashtable)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,check/hashtable) -o $(call MkO,check-hashtable)

$(call MkO,check-parser) : $(call MkH,check/core) $(call MkH,check/database) $(call MkH,check/lexer)  $(call MkH,check/hashtable) $(call MkH,check/proof) $(call MkH,check/parser) $(call MkC,check/parser)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,check/parser) -o $(call MkO,check-parser)

$(call MkO,check-lratchecker) : $(call MkH,check/core) $(call MkH,check/database) $(call MkH,check/lexer) $(call MkH,check/parser) $(call MkH,check/lratchecker) $(call MkC,check/lratchecker)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,check/lratchecker) -o $(call MkO,check-lratchecker)

$(call MkO,check-sickchecker) : $(call MkH,check/core) $(call MkH,check/database) $(call MkH,check/lexer) $(call MkH,check/parser) $(call MkH,check/sickchecker) $(call MkC,check/sickchecker)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,check/sickchecker) -o $(call MkO,check-sickchecker)

# $(call MkO,lratcheck) : $(call MkC,lratcheck)
# 	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,lratcheck) -o $(call MkO,lratcheck)

# $(call MkO,sickcheck) : $(call MkC,sickcheck)
# 	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,sickcheck) -o $(call MkO,sickcheck)

# $(call MkO,sickcheckbin) : $(call MkC,sickcheckbin)
# 	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,sickcheckbin) -o $(call MkO,sickcheckbin)

# $(call MkO,drattobrat) : $(call MkC,drattobrat)
# 	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,drattobrat) -o $(call MkO,drattobrat)

# $(call MkO,brattodrat) : $(call MkC,brattodrat)
# 	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,brattodrat) -o $(call MkO,brattodrat)

clean :
	rm -rfv $(RUPEEOBJS) $(CHECKOBJS) $(MkO,drattobrat) $(RUPEEBIN) $(LRATBIN) $(SICKBIN) $(DRATTOBRATBIN)

directories :
	@mkdir -p bin
	@mkdir -p obj
