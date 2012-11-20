#ifndef INC_OO_H
#define INC_OO_H
#define str__tabM (15000+4000)
  // +4000 because it might happen that an object has a lot of
  // key/val pairs or refroles which are not stored already;
#define str__tabstrM 250  // must be < row size of str__rab[]
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
  } str_info_t;


typedef struct {  // members may not be accessed from external
  #if read_GZ==0
    int fd;  // file descriptor
    off_t jumppos;  // position to jump to; -1: invalid
  #elif read_GZ==1
    FILE* fi;  // file stream
    off_t jumppos;  // position to jump to; -1: invalid
  #else
    gzFile fi;  // gzip file stream
    #if __WIN32__
      z_off64_t jumppos;  // position to jump to; -1: invalid
    #else
      z_off_t jumppos;  // position to jump to; -1: invalid
    #endif
  #endif
  int64_t counter;
    // byte counter to get the read position in input file;
  char filename[300];
  bool isstdin;  // is standard input
  bool eof;  // we are at the end of input file
  byte* bufp;  // pointer in buf[]
  byte* bufe;  // pointer to the end of valid input in buf[]
  uint64_t bufferstart;
    // dummy variable which marks the start of the read buffer
    // concatenated  with this instance of read info structure;
  } read_info_t;

typedef struct {
  read_info_t* ri;  // file handle for input files
  read_info_t* riph;  // file handle for input files;
    // this is a copy of .ri because it may be necessary to reaccess
    // a file which has already been logically closed;
    // used by the procedures oo__rewind() and oo__closeall();
  int format;  // input file format;
    // ==-9: unknown; ==0: o5m; ==10: xml; ==-1: pbf;
  str_info_t* str;  // string unit handle (if o5m format)
  uint64_t tyid;  // type/id of last read osm object of this file
  uint32_t hisver;  // OSM object version; needed for creating diff file
  const char* filename;
  bool endoffile;
  int deleteobject;  // replacement for .osc <delete> tag
    // 0: not to delete; 1: delete this object; 2: delete from now on;
  int deleteobjectjump;  // same as before but as save value for jumps
    // 0: not to delete; 1: delete this object; 2: delete from now on;
  int64_t o5id;  // for o5m delta coding
  int32_t o5lon,o5lat;  // for o5m delta coding
  int64_t o5histime;  // for o5m delta coding
  int64_t o5hiscset;  // for o5m delta coding
  int64_t o5rid[3];  // for o5m delta coding
  } oo__if_t;

#endif
