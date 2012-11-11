all : osmconvert 
#a.osm

osmconvert.o : osmconvert.c
	g++ -std=c++0x osmconvert.c -c -o osmconvert.o

osmconvert : osmconvert.o
	g++   osmconvert.o -lz -o osmconvert

a.osm : osmconvert a.pbf
	./osmconvert --drop-history a.pbf >a.osm