#ifndef INC_READ_HPP
#define INC_READ_HPP
//class read_info_t;
#include "oo.h"
#include <vector>

class Format {
public:
  // type of file?

};

const int _strM = (4*1024*1024);

template <class T> class simple_vec {
public:
  class iterator {
  public:
    simple_vec<T> & operator = (simple_vec<T> & t);
    T & operator *();
    iterator & operator ++(int);
    bool operator < (iterator);
    bool operator >= (iterator);
    bool operator != (iterator);
    bool operator ! ();
    operator bool ();


    iterator limit (uint32_t size); // create a new iterator that is +size of this one, used for reading fixed sized items.
    
  };
  iterator end();
  iterator begin();
  int size();
  //   operator const char *();// stringify
  //operator  char *();// stringify
  void push_back(T & p);
};

template <class T> int strvcmp(const char*, simple_vec<T>&);
typedef simple_vec<int64_t> refid_t;  // ids of referenced object
typedef simple_vec<byte> bytev_t;  // bytevector
typedef simple_vec<char> charv_t;  // bytevector
typedef bytev_t reftype_t;  // reftype
typedef simple_vec<int32_t> coord_t;  // ids of referenced object
typedef simple_vec<coord_t> coord2_t;  // ids of referenced object
typedef simple_vec<char> charv_t;  // ids of referenced object
typedef simple_vec<charv_t> charv2_t;  // refrole



void runassign(charv2_t::iterator & a, charv_t::iterator & b);

class PBF 
{

public:


  enum pb_type {
    osm_type =2,
    osm_bit = 3,
    pbf_type=8,
  }; 

  int32_t bbx1,
    bby1,
    bbx2,
    bby2;

  bool    bbvalid;
  uint64_t filetimestamp;
  
  int type= -9;  // type of the object which has been read;
  // 0: node; 1: way; 2: relation; 8: header;
  // -1: end of file; <= -10: error;
  int64_t id= 0;  // id of read object
  int32_t lon= 0,lat= 0;  // coordinates of read node
  int32_t hisver= 0;
  int64_t histime= 0;
  int64_t hiscset= 0;
  uint32_t hisuid= 0;
  const char* hisuser= "";
 int32_t hisvis= -1;  // (default for 'unknown')

  //
// for string primitive group table

  // maximum number of strings within each block
  char* _str[_strM];  // string table
 char** _stre= _str;  // end of data in str[]
 char** _stree= _str+_strM;  // end of str[]
 int _strm= 0;
// for tags of densnodes (start and end address)
 byte* _nodetags= NULL,*_nodetagse= NULL;  // node tag pairs
// for noderefs and tags of ways (start and end address each)
 byte* _waynode= NULL,*_waynodee= NULL;
 byte* _waykey= NULL,*_waykeye= NULL;
 byte* _wayval= NULL,*_wayvale= NULL;
// for refs and tags of relations (start and end address each)
charv2_t::iterator _relrefrole,_relrefrolee;
 byte* _relrefid= NULL,*_relrefide= NULL;
 byte* _relreftype= NULL,*_relreftypee= NULL;
 byte* _relval= NULL,*_relvale= NULL;



  void intjump(byte **);
  void input(bool b);
  void ini();

  //
  //  int noderef(int64_t* refidp,int refmax);
  refid_t::iterator  noderef(refid_t& refidp,size_t s);
  int ref(int64_t* refidp,byte* reftypep,char** refrolep,int refmax);
  int ref(refid_t&, reftype_t&, charv2_t&, size_t);
  //  int ref (refid_t&, reftype_t&, charv2_t&, size_t&);
  int keyval(charv2_t & keyp,charv2_t & valp);
};


class Read {
public:
  bytev_t::iterator & bufpbv();

  char * bufp();
  char * bufp(const char * p); // set the pointer?
  char * bufp(byte * p); // set the pointer?

  char * bufp(bytev_t::iterator, int l);

  char bufc(int c); // get one char
  byte * bufpb();
  char * bufpinc();// ++
  char * bufpinc(int len);// ++
  void bufpsetnull(); // set the current value to 0
  const char * bufe();
  const byte * bufeb();
  Format format;
  PBF pbf;
  bool setjump();
  void do_switch(read_info_t* filehandle);
  read_info_t* infop;
  void lockjump();
  bool input();
};

void posr_processing(int * maxrewind_posr,coord2_t & refxy);
refid_t::iterator calcendref(refid_t & a, int b);
reftype_t::iterator calcendtype(reftype_t & a, int b);
charv2_t::iterator calcendrole(charv2_t & a, int b);
void str_read(byte ** bufp, charv_t::iterator & sp,  const char ** hisuserc);
void str_read(byte ** bufp, charv2_t::iterator & sp); // null
void str_read(byte ** bufp, charv2_t::iterator & sp,charv2_t::iterator & vp); // null
void str_read(byte ** bufp, charv_t::iterator & sp); // null
void setrr(charv_t::iterator & , const char *v);
void str_read(byte**, char**, const char**);
//void str_read(byte ** bufp, charv_t::iterator & sp,  const char ** hisuserc);
//void str_read(byte ** bufp, charv_t::iterator & sp,  const char* hisuserc);
int strvcmp(charv_t & c , const char*);
int strvcmp(charv_t & c , charv_t::iterator & sp);

#endif
