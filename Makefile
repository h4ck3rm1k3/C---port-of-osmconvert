all : osmconvert 

CXX=g++
CXXFLAGS= -save-temps -std=c++0x

#CC=g++
#CFLAGS= -save-temps -std=c++0x

osmconvert.o : osmconvert.cpp
util.o : util.cpp util.h
global.o : global.cpp

osmconvert : osmconvert.o util.o global.o process.o oo.o



a.osm : osmconvert a.pbf
	./osmconvert --drop-history a.pbf >a.osm