//class read_info_t;
#include "oo.h"

class Format {
public:
  // type of file?

};


class PBF 
{

public:
  enum pb_type {
    osm_type =2,
    osm_bit = 3,
    pbf_type=8,
  }; 

  int64_t id;
  int32_t hisver;
  int32_t bbx1,
    bby1,
    bbx2,
    bby2;

  bool    bbvalid;
  uint64_t filetimestamp;
  pb_type type;
  void intjump(byte **);
  void input(bool b);
  void ini();
};


class Read {
public:
  char * bufp();
  char bufc(int c); // get one char
  byte * bufpb();
  char * bufpinc();// ++
  void bufpsetnull(); // set the current value to 0

  const char * bufe();
  const byte * bufeb();

  Format format;
  PBF pbf;


  void read_switch(read_info_t* filehandle);
};
