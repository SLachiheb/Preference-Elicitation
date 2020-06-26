#------------------------------------------------------------
#
# Please define an environnement variable PATHTUTOMIP
# equal to the XXX directory where Lemon,Graphviz and Cplex have been installed
# In order to these:
# edit ~/.bashrc add "export PATHTUTOMIP=XXX"
# close your xterm and star new ones
#
#------------------------------------------------------------

CPLEXDIR      = /home/sarah/MAOA/cplex-12.7.1/cplex
CONCERTDIR    = /home/sarah/MAOA/cplex-12.7.1/concert
CONCORDE      =/home/sarah/STAGE/concorde
QSOPT         =/home/sarah/STAGE/qsopt64

SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic


# ---------------------------------------------------------------------
# Compiler selection
# ---------------------------------------------------------------------

CCC = g++ -O0


# ---------------------------------------------------------------------
# Compiler options
# ---------------------------------------------------------------------

CCOPT = -g -w -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD -Wno-ignored-attributes

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR)


CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include
LEMONINCDIR   = $(LEMONDIR)/include

CCLNFLAGS = -lconcert -lilocplex -lcplex -lm -lpthread -lconcorde -lqsopt -lm -lpthread -ldl

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) -I$(LEMONINCDIR) -I$(QSOPT) -I$(CONCORDE) -L$(CONCORDE) -L$(QSOPT)


# ---------------------------------------------------------------------
# Project
# ---------------------------------------------------------------------

all: executable

main.o : main.cpp
	$(CCC) -c $(CCFLAGS) $(CCLNDIRS) -c main.cpp -o main.o

CSS.o : CSS.cpp CSS.h
	$(CCC) -c $(CCFLAGS) $(CCLNDIRS) -c CSS.cpp -o CSS.o

executable : main.o CSS.o
		$(CCC) $(CCFLAGS) $(CCLNDIRS) main.o CSS.o $(CCLNFLAGS) -o executable

clean:
	rm -f *.o executable
