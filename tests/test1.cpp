#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>

class MyTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE( MyTest );
  CPPUNIT_TEST( testVector);
  CPPUNIT_TEST_SUITE_END();
public:
  void testVector()
  {
    //return 1;
  }
};

int main( int argc, char **argv)
{
  CppUnit::TextUi::TestRunner runner;
  runner.addTest( MyTest::suite() );
  runner.run();
  return 0;
}
