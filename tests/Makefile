
all :
	g++ -L/usr/local/lib    -I /usr/local/include/ test1.cpp -lcppunit -ldl

testutil : ../util.o testutil.cpp
	g++ -L/usr/local/lib    -I /usr/local/include/ testutil.cpp ../util.o -lcppunit -ldl  -o testutil