
str_info_t* str_open();

class Str {
// this module provides procedures for conversions from
// strings which have been stored in data stream objects to
// c-formatted strings;
// as usual, all identifiers of a module have the same prefix,
// in this case 'str'; one underline will follow in case of a
// global accessible object, two underlines in case of objects
// which are not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----



  const int _tabM = (15000+4000);
  // +4000 because it might happen that an object has a lot of
  // key/val pairs or refroles which are not stored already;
  const int _tabstrM = 250;  // must be < row size of str__rab[]

  typedef struct str__info_struct {
    // members of this structure must not be accessed
    // from outside this module;
    char tab[str__tabM][256];
    // string table; see o5m documentation;
    // row length must be at least str__tabstrM+2;
    // each row contains a double string; each of the two strings
    // is terminated by a zero byte, the logical lengths must not
    // exceed str__tabstrM bytes in total;
    // the first str__tabM lines of this array are used as
    // input buffer for strings;
    int tabi;  // index of last entered element in string table;
    int tabn;  // number of valid strings in string table;
    struct str__info_struct* prev;  // address of previous unit;
  } info_t;

  str_info_t* _infop;

  void _end();
  // clean-up this module;

public:
  class String {

  public:
    bool operator != (String &s);

  };

  str_info_t* open();
  // open an new string client unit;
  // this will allow us to process multiple o5m input files;
  // return: handle of the new unit;
  //         ==NULL: error;
  // you do not need to care about closing the unit(s);

  void doswitch(str_info_t* sh);
  // switch to another string unit
  // sh: string unit handle;

  void reset();
    // clear string table;
    // must be called before any other procedure of this module
    // and may be called every time the string processing shall
    // be restarted;

  void read(bytev_t::iterator & pp, charv_t::iterator & p2); // single string
  void read(charv_t::iterator & pp, Str::String & s); // single string
  void read(charv_t::iterator & pp, Str::String & k,Str::String & v); // single string

  void read(bytev_t::iterator & pp, Str::String & k,Str::String & v); // single string

  void read(charv_t::iterator & pp, char  *& s1p); // single string
  void read(charv_t::iterator & pp, char  *& s1p, char  *& s2p); // key pair
  void read(charv_t::iterator & pp, char  *& s1p, const char  *& s2p); // key pair
    // read an o5m formatted string (pair), e.g. key/val, from
    // standard input buffer;
    // if got a string reference, resolve it, using an internal
    // string table;
    // no reference is used if the strings are longer than
    // 250 characters in total (252 including terminators);
    // pp: address of a buffer pointer;
    //     this pointer will be incremented by the number of bytes
    //     the converted protobuf element consumes;
    // s2p: ==NULL: read not a string pair but a single string;
    // return:
    // *s1p,*s2p: pointers to the strings which have been read;
    
    
  //void read(byte** pp,char** s1p,char** s2p);
    // read an o5m formatted string (pair), e.g. key/val, from
    // standard input buffer;
    // if got a string reference, resolve it, using an internal
    // string table;
    // no reference is used if the strings are longer than
    // 250 characters in total (252 including terminators);
    // pp: address of a buffer pointer;
    //     this pointer will be incremented by the number of bytes
    //     the converted protobuf element consumes;
    // s2p: ==NULL: read not a string pair but a single string;
    // return:
    // *s1p,*s2p: pointers to the strings which have been read;
  
  
};
