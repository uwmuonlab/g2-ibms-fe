#####################################################################
#
#  Name:         Makefile
#  Created by:   Matthias Smith
#
#  Contents:     Makefile for MIDAS NMR frontends, based on
#		 a sample Makefile written by Stefan Ritt.
#
#####################################################################

TARGETS = $(patsubst src/%.cxx,bin/%,$(wildcard src/*.cxx))

#--------------------------------------------------------------------
# This is for Linux
CPPFLAGS = -fpermissive -O2
CXXFLAGS = -std=c++11

ROOTFLAGS = $(shell root-config --cflags)
ROOTLIBS = $(shell root-config --libs)

#--------------------------------------------------------------------
# The following lines define direcories. Adjust if necessary
MID_INC = $(MIDASSYS)/include
MID_LIB = $(MIDASSYS)/linux/lib

# Libraries && include flags
CPPFLAGS += -Iinclude -I/usr/include -I/usr/local/include 
CPPFLAGS += -I$(MID_INC) -I$(BOOST_INC)
CPPFLAGS += -Wl,-rpath,/usr/local/lib
LIBS += -L/usr/lib -L/usr/lib64 -L/usr/local/lib 
LIBS += -L$(MID_LIB) -L/usr/lib64/mysql 
LIBS += -lm -lz -lpthread -lodbc -lrt
LIBS += -lmidas-shared -lmysqlclient -lsqlite3 
LIBS += -lboost_system -lboost_filesystem
LIBS += -lg2fieldvme

# Set compilers
CC = gcc
CXX = g++

#-------------------------------------------------------------------
# Finally we have the actual make directives.

# Make commands

all: $(TARGETS)

bin/%: src/%.cxx $(MID_LIB)/mfe.o $(OBJECTS)
	$(CXX) -o $@ $+ $(CPPFLAGS) $(CXXFLAGS) $(ROOTFLAGS) \
	$(LIBS) $(ROOTLIBS)
	ln -sf $(shell pwd)/$@ ../bin/

clean:
	rm -f *~ $(TARGETS) $(OBJECTS)
