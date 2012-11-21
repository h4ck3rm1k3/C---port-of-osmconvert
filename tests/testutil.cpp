// testutil.cpp
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include "../stdinc.hpp"
#include "../read.hpp"
#include "../util.h"
#include <stdlib.h>

//char* uint32toa(uint32_t v,char* s) {
// char* int64toa(int64_t v,char* s) {
//stpcpy0(char *dest, const char *src) {
//char* strMcpy(const char* d,const char* s ) {
//char *strmcpy(char *dest, const char *src, size_t maxlen) {
// int strzcmp(const char* s1,const char* s2) {
// int strzlcmp(const char* s1,const char* s2) {
// int strycmp(const char* s1,const char* s2) {
//bool file_exists(const char* file_name) {


class MyTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE( MyTest );
  CPPUNIT_TEST( testuint32toa);
  CPPUNIT_TEST( testint64toa);
  CPPUNIT_TEST_SUITE_END();
public:

  void testuint32toa()
  {
    uint32_t tests[] = {  0, // 0 
			  1, // 1
			  0xFFF,//2 4095
			  0xFFFFFF,  // 3 16777215 
			  0xFFFFFFF,  // 4 268435455 
			  2147483647, // 5 2147483647 
			  4294967294, // 7 4294967294 
			  0xFFFFFFFF, // 8 4294967295 
			  4294967295, // 9 4294967295 
			  -1,         // 10
			  //			  4294967299, // 11: overflow to 3
    };
    char sprintfout [64];
    char uint32toaout [64];
    int count =sizeof(tests)/sizeof(uint32_t);
    printf("going to run %d \n",count);
    for(int i =0; i< sizeof(tests)/sizeof(uint32_t); i ++) {
      sprintf(sprintfout,"%u",tests[i]);
      uint32toa(tests[i],uint32toaout);      
      CPPUNIT_ASSERT( strcmp(sprintfout,uint32toaout) ==0 );
    }
    printf("going to run randoms\n",count);
    for(int i =0; i< 100000; i ++) {
      unsigned int v= rand() % 4294967295 ;
      sprintf(sprintfout,"%u",v);
      uint32toa(v,uint32toaout);     
      CPPUNIT_ASSERT( strcmp(sprintfout,uint32toaout) ==0 );
    }
  }

  template <class T, class F> void testXtoa(T * tests, int count, T max, const char *fmt, F f)
  {
    char sprintfout [256];
    char Ttoaout [256];
    printf("going to run %d \n",count);
    for(int i =0; i< count; i ++) {
      T v=tests[i];
      sprintf(sprintfout,fmt,v);
      f(v,Ttoaout);      
      //      printf("check1 %d %s %s\n",i,sprintfout,Ttoaout);
      CPPUNIT_ASSERT( strcmp(sprintfout,Ttoaout) ==0 );
    }

    printf("going to run randoms\n",count);
    for(int i =0; i< 100000; i ++) {
      T v= rand() % max ;
      sprintf(sprintfout,fmt,v);
      f(v,Ttoaout);  
      //      printf("check2 %d %s %s\n",i,sprintfout,Ttoaout);
      CPPUNIT_ASSERT( strcmp(sprintfout,Ttoaout) ==0 );
    }
  }

  void testint64toa()
  {

    int64_t tests[] = {  0, // 0 
			  1, // 1
			  0xFFFL,//2 4095
			  0xFFFFFFL,  // 3 16777215 
			  0xFFFFFFFL,  // 4 268435455 
			  2147483647, // 5 2147483647 
			  4294967294, // 7 4294967294 
			  0xFFFFFFFF, // 8 4294967295 
			  4294967295, // 9 4294967295 
			  -1,         // 10
			  4294967299, // 11:
			  0xFFFFFFFFFL, // 12
			  0xFFFFFFFFFFL, // 13
			  0xFFFFFFFFFFFL, //14
			  0xFFFFFFFFFFFFL, // 15
			  0xFFFFFFFFFFFFFL, // 16
			  0xFFFFFFFFFFFFFFL, // 17
			 //			  0xFFFFFFFFFFFFFFFFFFL, //
			 //			  0xFFFFFFFFFFFFFFFFFFFL, //
			 //			  0xFFFFFFFFFFFFFFFFFFFFL, //
			 //			  0xFFFFFFFFFFFFFFFFFFFFFL, //
			 //			  0xFFFFFFFFFFFFFFFFFFFFFFL, //
			 // 0xFFFFFFFFFFFFFFFFFFFFFFFFL, //
    };
    int count =sizeof(tests)/sizeof(int64_t);
    int64_t max=0xFFFFFFFFFFFFFFL;
    testXtoa(tests,count,  max, "%Ld", &int64toa);
  }

};

int main( int argc, char **argv)
{
  CppUnit::TextUi::TestRunner runner;
  runner.addTest( MyTest::suite() );
  runner.run();
  return 0;
}
