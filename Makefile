all : osmconvert a.osm

osmconvert : osmconvert.c
	g++ -lz osmconvert.c -o osmconvert

a.osm : osmconvert a.pbf
	./osmconvert --drop-history a.pbf >a.osm