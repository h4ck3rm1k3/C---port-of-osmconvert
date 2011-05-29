// osmconvert 2011-05-27 15:30
#define VERSION "0.0L"
// (c) Markus Weber, Nuernberg
//
// compile this source with option -lz
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program; if not, see http://www.gnu.org/licenses/.
// Other licenses are available on request; please ask the author.

// just a note (for tests): -b=8.748,53.052,8.749,53.053
// -Wall -lz -std=c99 -pedantic -g
#define __STDC_CONSTANT_MACROS 1
#include <string>
using namespace std;
#define MAXLOGLEVEL 2
const char* helptext=
"\nosmconvert " VERSION "\n"
"\n"
"THIS PROGRAM IS FOR EXPERIMENTAL USE ONLY.\n"
"PLEASE EXPECT MALFUNCTION AND DATA LOSS.\n"
"SAVE YOUR DATA BEFORE STARTING THIS PROGRAM.\n"
"\n"
"This program reads a different file formats of the OpenStreetMap\n"
"project and converts the data to the selected output file format.\n"
"These formats can be read:\n"
"  .osm  .osc  .o5m  .o5c  .pbf\n"
"These formats can be written:\n"
"  .osm (default)  .osc (in compact mode)  .o5m  .o5c\n"
"\n"
"Names of input files must be specified as calling line parameters.\n"
"Use - to read from standard input. You do not need to specify the\n"
"input formats, osmconvert will recognize them by itself.\n"
"The output format is .osm by default. If you want a different format,\n"
"please specify it using the appropriate command line parameter.\n"
"\n"
"-b=<x1>,<y1>,<x2>,<y2>\n"
"        If you want to limit the geographical region, you can define\n"
"        a bounding box. To do this, enter the southwestern and the\n"
"        northeastern corners of that area. For example:\n"
"        -b=-0.5,51,0.5,52\n"
"\n"
"-B=<border_polygon>\n"
"        Alternatively to a bounding box you can use a border polygon\n"
"        to limit the geographical region.\n"
"        The format of a border polygon file can be found in the OSM\n"
"        Wiki: http://wiki.openstreetmap.org/wiki/Osmosis/\n"
"              Polygon_Filter_File_Format\n"
"        You do not need to follow strictly the format description,\n"
"        you must ensure that every line of coordinates starts with\n"
"        blanks.\n"
"\n"
"--drop-brokenrefs\n"
"        Use this option if you need to delete references to nodes\n"
"        which have been excluded because lying outside the borders.\n"
"\n"
"--drop-history\n"
"        For most applications the history tags are not needed. If you\n"
"        specify this option, no history information will be written,\n"
"        no version, changeset, user or timestamp.\n"
"\n"
"--drop-nodes\n"
"--drop-ways\n"
"--drop-relations\n"
"        According to the combination of these parameters, no members\n"
"        of the referred section will be written.\n"
"\n"
"--emulate-osmosis\n"
"--emulate-pbf2osm\n"
"        In case of .osm output format, the program will try to use\n"
"        the same data syntax as Osmosis, resp. pbf2osm.\n"
"\n"
"-h\n"
"-help\n"
"        Display this help.\n"
"\n"
"--merge-duplicates\n"
"        Some .osc files contain different versions of one object.\n"
"        Use this option to accept such duplicates on input.\n"
"\n"
"--out-osm\n"
"        Data will be written in .osm format. This is the default\n"
"        output format.\n"
"\n"
"--out-osc\n"
"        The OSM Change format will be used for output. Please note\n"
"        that osmchange will write the data in .osc compact format.\n"
"        This means that there will be no <create> and no <modify>\n"
"        tags but only <delete> tags. OSM objects which are to be\n"
"        deleted are represented by their ids only. Objects which are\n"
"        to be created or modified are not included in create or\n"
"        modify tags.\n"
"\n"
"--out-o5m\n"
"        The .o5m format will be used. This format has the same\n"
"        structure as the conventional .osm format, but the data are\n"
"        stored as binary numbers and are therefore much more compact\n"
"        than in .osm format. No packing is used, so you can pack .o5m\n"
"        files using every file packer you want, e.g. lzo, bz2, etc.\n"
"\n"
"--out-o5c\n"
"        This is the change file format of .o5m data format. All\n"
"        <delete> tags will not be performed as delete actions but\n"
"        converted into .o5c data format.\n"
"\n"
"-t=<tempfile>\n"
"        If borders are to be applied or broken references to be\n"
"        eliminated, osmconvert creates and uses two temporary files.\n"
"        This parameter defines their name prefix. The default value\n"
"        is \"osmconvert_tempfile\".\n"
"        \n"
"-v\n"
"        With activated \'verbose\' mode, some statistical and\n"
"        diagnosis data will be displayed.\n"
"\n"
"Examples\n"
"\n"
"./osmconvert europe.pbf --drop-history >europe.osm\n"
"bzcat europe.osm.bz2 |./osmconvert - |gzip >europe.osm\n"
"./osmconvert europe.pbf -B=ch.poly >switzerland.osm\n"
"./osmconvert switzerland.osm --out-o5m >switzerland.o5m\n"
"./osmconvert june_july.osc --out-o5c >june_july.o5c\n"
"./osmconvert june.o5m june_july.o5c --out-o5m >july.o5m\n"
"./osmconvert sep.osm sep_oct.osc oct_nov.osc >nov.osm\n"
"./osmconvert northamerica.osm southamerica.osm >americas.osm\n"
"\n"
"Tuning\n"
"\n"
"To speed-up the process, the program uses some main memory for a\n"
"hash table. By default, it uses 320 MiB for storing a flag for every\n"
"possible node, 60 for the way flags, and 20 relation flags.\n"
"Every byte holds the flag for 8 ID numbers, i.e., in 320 MiB the\n"
"program can store 2684 million flags. As there are less than 1300\n"
"million IDs for nodes at present (May 2011), 160 MiB would suffice.\n"
"So, for example, you can decrease the hash sizes to e.g. 160, 16 and\n"
"2 MiB using this option:\n"
"\n"
"  -h=160-16-2\n"
"\n"
"But keep in mind that the OSM database is continuously expanding. For\n"
"this reason the program-own default value is higher than shown in the\n"
"example, and it may be appropriate to increase it in the future.\n"
"If you do not want to bother with the details, you can enter the\n"
"amount of memory as a sum, and the program will divide it by itself.\n"
"For example:\n"
"\n"
"  -h=1000\n"
"\n"
"These 1000 MiB will be split in three parts: 800 for nodes, 150 for\n"
"ways, and 50 for relations.\n"
"\n"
"Because we are taking hashes, it is not necessary to provide all the\n"
"suggested memory; the program will operate with less hash memory too.\n"
"But, in this case, the border filter will be less effective, i.e.,\n"
"some ways and some relations will be left in the output file although\n"
"they should have been excluded.\n"
"The maximum value the program accepts for the hash size is 4000 MiB;\n"
"If you exceed the maximum amount of memory available on your system,\n"
"the program will try to reduce this amount and display a warning\n"
"message.\n"
"\n"
"Limitations\n"
"\n"
"When extracting a geographical region (using -b or -B), the input\n"
"file must contain the objects ordered by their type: first, all\n"
"nodes, next, all ways, followed by all relations. Within each of\n"
"these sections, the objects section must be sorted by their id in\n"
"ascending order.\n"
"\n"
"Usual .osm, .osc, .o5m, o5c and .pbf files adhere to this condition.\n"
"This means that you do not have to worry about this limitation.\n"
"osmconvert will display an error message if this sequence is broken.\n"
"\n"
"If a polygon file for borders is supplied, the maximum number of\n"
"polygon points is about 40,000.\n"
"\n"
"\n"
"Presently, this program is in an experimental state. Please expect\n"
"errors and do not use the program in productive or commercial systems.\n"
"\n"
"There is NO WARRANTY, to the extent permitted by law.\n"
"Please send any bug reports to markus.weber@gmx.com\n\n";

#define _FILE_OFFSET_BITS 64
#include <zlib.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

//typedef enum {/*false= 0,*/true= 1} bool;
typedef uint8_t byte;
typedef unsigned int uint;
#define isdig(x) isdigit((unsigned char)(x))
static int loglevel= 0;  // logging to stderr;
  // 0: no logging; 1: small logging; 2: normal logging;
  // 3: extended logging;
#define DP(f) fprintf(stderr,"Debug: " #f "\n");
#define DPv(f,...) fprintf(stderr,"Debug: " #f "\n",__VA_ARGS__);
#ifdef _WIN32
  #define NL "\r\n"  // use CR/LF as new-line sequence
#else
  #define NL "\n"  // use LF as new-line sequence
  #define O_BINARY 0
#endif



//------------------------------------------------------------
// Module Global   global variables for this program
//------------------------------------------------------------

// to distinguish global variable from local or module global
// variables, they are preceded by 'global_';

static bool global_mergeduplicates= false;  // accept duplicate objects
static bool global_drophistory= false;  // exclude history information
static bool global_dropbrokenrefs= false;  // exclude broken references
static bool global_dropnodes= false;  // exclude nodes section
static bool global_dropways= false;  // exclude ways section
static bool global_droprelations= false;  // exclude relations section
static bool global_outo5m= false;  // output shall have .o5m format
static bool global_outo5c= false;  // output shall have .o5c format
static bool global_outosc= false;  // output shall have .osc format
static bool global_emulateosmosis= false;
  // emulate osmosis compatible output
static bool global_emulatepbf2osm= false;
  // emulate pbf2osm compatible output
static const char* global_tempfilename= "osmconvert_tempfile";
  // prefix of names for temporary files
#define ONAME(i) \
  (i==0? "node": i==1? "way": i==2? "relation": "unknown object")
#define global_fileM 400  // maximum number of input files

//------------------------------------------------------------
// end   Module Global   global variables for this program
//------------------------------------------------------------


#if __WIN32__
static inline char *stpcpy(char *dest, const char *src) {
  while(*src!=0)
    *dest++= *src++;
  *dest= 0;
  return dest;
  }  // end stpcpy()
#endif

static inline char *strmcpy(char *dest, const char *src, size_t maxlen) {
  // similar to strcpy(), this procedure copies a character string;
  // here, the length is cared about, i.e. the target string will
  // be limited in case it is too long;
  // src[]: source string which is to be copied;
  // maxlen: maximum length of the destination string
  //         (including terminator null);
  // return:
  // dest[]: destination string of the copy; this is the
  //         function's return value too;
  char* d;

  if(maxlen==0)
return dest;
  d= dest;
  while(--maxlen>0 && *src!=0)
    *d++= *src++;
  *d= 0;
  return dest;
  }  // end   strmcpy()
#define strMcpy(d,s) strmcpy((d),(s),sizeof(d))

static inline char *stpmcpy(char *dest, const char *src, size_t maxlen) {
  // similar to strmcpy(), this procedure copies a character string;
  // however, it returns the address of the destination string's
  // terminating zero character;
  // this makes it easier to concatenate strings;
  char* d;

  if(maxlen==0)
return dest;
  d= dest;
  while(--maxlen>0 && *src!=0)
    *d++= *src++;
  *d= 0;
  return d;
  }  // end stpmcpy()
#define stpMcpy(d,s) stpmcpy(d,s,sizeof(d))

static inline int strzcmp(const char* s1,const char* s2) {
  // similar to strcmp(), this procedure compares two character strings;
  // here, the number of characters which are to be compared is limited
  // to the length of the second string;
  // i.e., this procedure can be used to identify a short string s2
  // within a long string s1;
  // s1[]: first string;
  // s2[]: string to compare with the first string;
  // return:
  // 0: both strings are identical; the first string may be longer than
  //    the second;
  // -1: the first string is alphabetical smaller than the second;
  // 1: the first string is alphabetical greater than the second;
  while(*s1==*s2 && *s1!=0) { s1++; s2++; }
  if(*s2==0)
    return 0;
  return *(unsigned char*)s1 < *(unsigned char*)s2? -1: 1;
  }  // end   strzcmp()

static inline int strzlcmp(const char* s1,const char* s2) {
  // similar to strzcmp(), this procedure compares two character strings;
  // and accepts the first string to be longer than the second;
  // other than strzcmp(), this procedure returns the length of s2[] in
  // case both string contents are identical, and returns 0 otherwise;
  // s1[]: first string;
  // s2[]: string to compare with the first string;
  // return:
  // >0: both strings are identical, the length of the second string is
  //     returned; the first string may be longer than the second;
  // 0: the string contents are not identical;
  const char* s2a;

  s2a= s2;
  while(*s1==*s2 && *s1!=0) { s1++; s2++; }
  if(*s2==0)
    return s2-s2a;
  return 0;
  }  // end   strzlcmp()



//------------------------------------------------------------
// Module pbf_   protobuf conversions module
//------------------------------------------------------------

// this module provides procedures for conversions from
// protobuf formats to regular numbers;
// as usual, all identifiers of a module have the same prefix,
// in this case 'pbf'; an underline will follow in case of a
// global accessible object, two underlines in case of objects
// which are not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----
// many procedures have a parameter 'pp'; here, the address of
// a buffer pointer is expected; this pointer will be incremented
// by the number of bytes the converted protobuf element consumes;

//------------------------------------------------------------

static inline uint32_t pbf_uint32(byte** pp) {
  // get the value of an unsigned integer;
  // pp: see module header;
  byte* p;
  uint32_t i;
  uint32_t fac;

  p= *pp;
  i= *p;
  if((*p & 0x80)==0) {  // just one byte
    (*pp)++;
return i;
    }
  i&= 0x7f;
  fac= 0x80;
  while(*++p & 0x80) {  // more byte(s) will follow
    i+= (*p & 0x7f)*fac;
    fac<<= 7;
    }
  i+= *p++ *fac;
  *pp= p;
  return i;
  }  // end   pbf_uint32()

static inline int32_t pbf_sint32(byte** pp) {
  // get the value of an unsigned integer;
  // pp: see module header;
  byte* p;
  int32_t i;
  int32_t fac;
  int sig;

  p= *pp;
  i= *p;
  if((*p & 0x80)==0) {  // just one byte
    (*pp)++;
    if(i & 1)  // negative
return -1-(i>>1);
    else
return i>>1;
    }
  sig= i & 1;
  i= (i & 0x7e)>>1;
  fac= 0x40;
  while(*++p & 0x80) {  // more byte(s) will follow
    i+= (*p & 0x7f)*fac;
    fac<<= 7;
    }
  i+= *p++ *fac;
  *pp= p;
    if(sig)  // negative
return -1-i;
    else
return i;
  }  // end   pbf_sint32()

static inline uint64_t pbf_uint64(byte** pp) {
  // get the value of an unsigned integer;
  // pp: see module header;
  byte* p;
  uint64_t i;
  uint64_t fac;

  p= *pp;
  i= *p;
  if((*p & 0x80)==0) {  // just one byte
    (*pp)++;
return i;
    }
  i&= 0x7f;
  fac= 0x80;
  while(*++p & 0x80) {  // more byte(s) will follow
    i+= (*p & 0x7f)*fac;
    fac<<= 7;
    }
  i+= *p++ *fac;
  *pp= p;
  return i;
  }  // end   pbf_uint64()

static inline int64_t pbf_sint64(byte** pp) {
  // get the value of a signed integer;
  // pp: see module header;
  byte* p;
  int64_t i;
  int64_t fac;
  int sig;

  p= *pp;
  i= *p;
  if((*p & 0x80)==0) {  // just one byte
    (*pp)++;
    if(i & 1)  // negative
return -1-(i>>1);
    else
return i>>1;
    }
  sig= i & 1;
  i= (i & 0x7e)>>1;
  fac= 0x40;
  while(*++p & 0x80) {  // more byte(s) will follow
    i+= (*p & 0x7f)*fac;
    fac<<= 7;
    }
  i+= *p++ *fac;
  *pp= p;
    if(sig)  // negative
return -1-i;
    else
return i;
  }  // end   pbf_sint64()

static inline bool pbf_jump(byte** pp) {
  // jump over a protobuf formatted element - no matter
  // which kind of element;
  // pp: see module header;
  // return: the data do not meet protobuf specifications (error);
  byte* p;
  int type;

  p= *pp;
  type= *p & 0x07;
  switch(type) {  // protobuf type
  case 0:  // Varint
    while(*p & 80) p++; p++;  // jump over id
    while(*p & 80) p++; p++;  // jump over data
    break;
  case 1: // fixed 64 bit;
    while(*p & 80) p++; p++;  // jump over id
    p+= 4;  // jump over data
    break;
  case 2:  // String
    while(*p & 80) p++; p++;  // jump over id
    p+= pbf_uint32(&p);  // jump over string contents
    break;
  case 5: // fixed 32 bit;
    while(*p & 80) p++; p++;  // jump over id
    p+= 2;  // jump over data
    break;
  default:  // unknown id
    fprintf(stderr,"osmconvert: Format error: 0x%02X.\n",*p);
    (*pp)++;
return true;
    }  // end   protobuf type
  *pp= p;
  return false;
  }  // end   pbf_jump()

static inline void pbf_intjump(byte** pp) {
  // jump over a protobuf formatted integer;
  // pp: see module header;
  // we do not care about a possibly existing identifier,
  // therefore as the start address *pp the address of the
  // integer value is expected;
  byte* p;

  p= *pp;
  while(*p & 0x80) p++; p++;
  *pp= p;
  }  // end   pbf_intjump()

//------------------------------------------------------------
// end   Module pbf_   protobuf conversions module
//------------------------------------------------------------



//------------------------------------------------------------
// Module hash_   OSM hash module
//------------------------------------------------------------

// this module provides three hash tables with default sizes
// of 320, 60 and 20 MB;
// the procedures hash_seti() and hash_geti() allow bitwise
// access to these tables;
// as usual, all identifiers of a module have the same prefix,
// in this case 'hash'; an underline will follow in case of a
// global accessible object, two underlines in case of objects
// which are not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----

static bool hash__initialized= false;
#define hash__M 3
static unsigned char* hash__mem[hash__M]= {NULL,NULL,NULL};
  // start of the hash fields for each object type (node, way, relation);
static uint32_t hash__max[hash__M]= {0,0,0};
  // size of the hash fields for each object type (node, way, relation);
static int hash__error_number= 0;
  // 1: object too large

static void hash__end() {
  // clean-up for hash module;
  // will be called at program's end;
  int o;  // object type

  for(o= 0;o<hash__M;o++) {
    hash__max[o]= 0;
    if(hash__mem[o]!=NULL) {
      free(hash__mem[o]); hash__mem[o]= NULL; }
    }
  hash__initialized= false;
  }  // end   hash__end()

//------------------------------------------------------------

static int hash_ini(int n,int w,int r) {
  // initializes the hash module;
  // n: amount of memory which is to be allocated for nodes;
  // w: amount of memory which is to be allocated for ways;
  // r: amount of memory which is to be allocated for relations;
  // range for all input parameters: 1..4000, unit: MiB;
  // the second and any further call of this procedure will be ignored;
  // return: 0: initialization has been successful (enough memory);
  //         1: memory request had to been reduced to fit the system's
  //            resources (warning);
  //         2: memory request was unsuccessful (error);
  // general note concerning OSM database:
  // number of objects at Oct 2010: 950M nodes, 82M ways, 1.3M relations;
  int o;  // object type
  bool warning,error;

  warning= error= false;
  if(hash__initialized)  // already initialized
    return 0;  // ignore the call of this procedure
  // check parameters and store the values
  #define D(x,o) if(x<1) x= 1; else if(x>4000) x= 4000; \
    hash__max[o]= x*(1024*1024);
  D(n,0) D(w,1) D(r,2)
  #undef D
  // allocate memory for each hash table
  for(o= 0;o<hash__M;o++) {  // for each hash table
    do {
      hash__mem[o]= (unsigned char*)malloc(hash__max[o]);
      if(hash__mem[o]!=NULL) {  // allocation successful
        memset(hash__mem[o],0,hash__max[o]);  // clear all flags
    break;
        }
      // here: allocation unsuccessful
      // reduce amount by 50%
      hash__max[o]/=2;
      warning= true;
        // memorize that the user should be warned about this reduction
      // try to allocate the reduced amount of memory
      } while(hash__max[o]>=1024);
    if(hash__mem[o]==NULL)  // allocation unsuccessful at all
      error= true;  // memorize that the program should be aborted
    }  // end   for each hash table
  atexit(hash__end);  // chain-in the clean-up procedure
  if(!error) hash__initialized= true;
  return error? 2: warning? 1: 0;
  }  // end   hash_ini()

static inline void hash_seti(int o,int64_t idi) {
  // set a flag for a specific object type and ID;
  // o: object type; 0: node; 1: way; 2: relation;
  //    caution: due to performance reasons the boundaries
  //    are not checked;
  // id: id of the object;
  unsigned char* mem;  // address of byte in hash table
  unsigned int ido;  // bit offset to idi;

  if(!hash__initialized) return;  // error prevention
  idi+= hash__max[o]<<3;  // consider small negative numbers
  ido= idi&0x7;  // extract bit number (0..7)
  idi>>=3;  // calculate byte offset
  idi%= hash__max[o];  // consider length of hash table
  mem= hash__mem[o];  // get start address of hash table
  mem+= idi;  // calculate address of the byte
  *mem|= (1<<ido);  // set bit
  }  // end   hash_seti()

static inline bool hash_geti(int o,int64_t idi) {
  // get the status of a flag for a specific object type and ID;
  // (same as previous procedure, but id must be given as number);
  // o: object type; 0: node; 1: way; 2: relation;  caution:
  //    due to performance reasons the boundaries are not checked;
  // id: id of the object;
  unsigned char* mem;
  unsigned int ido;  // bit offset to idi;
  bool flag;

  if(!hash__initialized) return 1;  // error prevention
  idi+= hash__max[o]<<3;  // consider small negative numbers
  ido= idi&0x7;  // extract bit number (0..7)
  idi>>=3;  // calculate byte offset
  idi%= hash__max[o];  // consider length of hash table
  mem= hash__mem[o];  // get start address of hash table
  mem+= idi;  // calculate address of the byte
  flag= (*mem&(1<<ido))!=0;  // get status of the addressed bit
  return flag;
  }  // end   hash_geti();

static int hash_queryerror() {
  // determine if an error has occurred;
  return hash__error_number;
  }  // end   hash_queryerror()

//------------------------------------------------------------
// end   Module hash_   OSM hash module
//------------------------------------------------------------



//------------------------------------------------------------
// Module border_   OSM border module
//------------------------------------------------------------

// this module provides procedures for reading the border file
// (.poly) and determine if a point lies inside or outside the
// border polygon;
// as usual, all identifiers of a module have the same prefix,
// in this case 'border'; an underline will follow in case of a
// global accessible object, two underlines in case of objects
// which are not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----

static const int32_t border__nil= 2000000000L;
static int32_t border__bx1= 2000000000L,border__by1,
  border__bx2,border__by2;
  // in case of a border box:
  // coordinates of southwest and northeast corner;
// in case of a border polygon:
// for the border polygon, every edge is stored in a list;
// to speed-up the inside/outside question we need to sort the edges
// by x1; subsequently, for every edge there must be stored references
// which refer to all that edges which overlap horizontally with
// that region between x1 and the next higher x1 (x1 of the next edge
// in the sorted list);
#define border__edge_M 60004
typedef struct border__edge_t {
  int32_t x1,y1,x2,y2;  // coordinates of the edge; always: x1<x2;
  struct border__chain_t* chain;
  } border__edge_t;
  // the last element in this list will have x1==border__nil;
static border__edge_t* border__edge;
static int border__edge_n= 0;  // number of elements in border__edge[0]
#define border__chain_M (border__edge_M*8)
typedef struct border__chain_t {
  border__edge_t* edge;
  struct border__chain_t* next;
  } border__chain_t;
  // the last element in this list will have edge==NULL;
  // the last element of each chain will be terminated with next==NULL;
static border__chain_t* border__chain;

static void border__end(void) {
  // close this module;
  // this procedure has no parameters because we want to be able
  // to call it via atexit();

  if(border__edge!=NULL)
    free(border__edge);
  border__edge= NULL;
  border__edge_n= 0;
  if(border__chain!=NULL)
    free(border__chain);
  border__chain= NULL;
  }  // end   border__end()

static inline bool border__ini(void) {
  // initialize this module;
  // you may call this procedure repeatedly; only the first call
  // will have effect; subsequent calls will be ignored;
  // return: ==true: success, or the call has been ignored;
  //         ==false: an error occurred during initialization;
  static bool firstrun= true;

  if(firstrun) {
    firstrun= false;
    atexit(border__end);
    border__edge= (border__edge_t*)
      malloc((border__edge_M+4)*sizeof(border__edge_t));
    if(border__edge==NULL)
return false;
    border__chain= (border__chain_t*)
      malloc((border__chain_M+4)*sizeof(border__chain_t));
    if(border__chain==NULL)
return false;
    }
  return true;
  }  // end   border__ini()

static int border__qsort_edge(const void* a,const void* b) {
  // edge comparison for qsort()
  int32_t ax,bx;

  ax= ((border__edge_t*)a)->x1;
  bx= ((border__edge_t*)b)->x1;
  if(ax>bx)
return 1;
  if(ax==bx)
return 0;
  return -1;
  }  // end   border__qsort()

//------------------------------------------------------------

static bool border_active= false;  // borders are to be considered;
  // this variable must not be written from outside of the module;

static bool border_box(const char* s) {
  // read coordinates of a border box;
  // s[]: coordinates as a string; example: "11,49,11.3,50"
  // return: success;
  double x1f,y1f;  // coordinates of southwestern corner
  double x2f,y2f;  // coordinates of northeastern corner
  int r;

  x1f= y1f= x2f= y2f= 200.1;
  r= sscanf(s,"%lG,%lG,%lG,%lG",&x1f,&y1f,&x2f,&y2f);
  if(r!=4 || x1f<-180.1 || x1f>180.1 || y1f<-90.1 || y1f>90.1 ||
      x2f<-180.1 || x2f>180.1 || y2f<-90.1 || y2f>90.1)
return false;
  border_active=true;
  border__bx1= x1f*10000000L;  // convert floatingpoint to fixpoint
  border__by1= y1f*10000000L;
  border__bx2= x2f*10000000L;
  border__by2= y2f*10000000L;
  return true;
  }  // end   border_box()

static bool border_file(const char* fn) {
  // read border polygon file, store the coordinates, and determine
  // an enclosing border box to speed-up the calculations;
  // fn[]: file name;
  // return: success;
  static int32_t nil;

  if(!border__ini())
return false;
  nil= border__nil;

  /* get border polygon */ {
    border__edge_t* bep;  // growing pointer in border__edge[]
    border__edge_t* bee;  // memory end of border__edge[]
    FILE* fi;
    char s[80],*sp;
    int32_t x0,y0;  // coordinate of the first point in a section;
      // this is used to close an unclosed polygon;
    int32_t x1,y1;  // last coordinates
    int32_t x,y;

    border__edge[0].x1= nil;
    fi= fopen(fn,"rb");
    if(fi==NULL)
return false;
    bee= border__edge+(border__edge_M-2);
    bep= border__edge;
    x0= nil;  // (sign that there is no first coordinate at the moment)
    x1= nil;  // (sign that there is no last coordinate at the moment)
    for(;;) {  // for every line in border file
      s[0]= 0;
      sp= fgets(s,sizeof(s),fi);
      if(bep>=bee)
return false;
      if(s[0]!=' ' && s[0]!='\t') {  // not inside a section
        if(x0!=nil && x1!=nil && (x1!=x0 || y1!=y0)) {
            // last polygon was not closed
          if(x1==x0) {  // the edge would be vertical
            // we have to insert an additional edge
            x0+= 3;
            if(x0>x1)
              { bep->x1= x1; bep->y1= y1; bep->x2= x0; bep->y2= y0; }
            else
              { bep->x1= x0; bep->y1= y0; bep->x2= x1; bep->y2= y1; }
            bep->chain= NULL;
            if(loglevel>=1)
              fprintf(stderr,
                "+ %i %i,%i,%i,%i\n",
                (int)(bep-border__edge),
                bep->x1,bep->y1,bep->x2,bep->y2);
            bep++;
            x1= x0; y1= y0;
            x0-= 3;
            }  // the edge would be vertical
          // close the polygon
          if(x0>x1)
            { bep->x1= x1; bep->y1= y1; bep->x2= x0; bep->y2= y0; }
          else
            { bep->x1= x0; bep->y1= y0; bep->x2= x1; bep->y2= y1; }
          bep->chain= NULL;
          if(loglevel>=1)
            fprintf(stderr,
              "c %i %i,%i,%i,%i\n",
              (int)(bep-border__edge),bep->x1,bep->y1,bep->x2,bep->y2);
          bep++;
          }  // end   last polygon was not closed
        x0= x1= nil;
        }  // end   not inside a section
      else {  // inside a section
        double xf,yf;

        xf= yf= 200.1;
        sscanf(s+1,"%lG %lG",&xf,&yf);
        if(xf<-180.1 || xf>180.1 || yf<-90.1 || yf>90.1) x= nil;
        else
          { x= xf*10000000+0.5; y= yf*10000000+0.5; }
        if(x!=nil) {  // data plausible
          if(x1!=nil) {  // there is a preceding coordinate
            if(x==x1) x+= 2;  // do not accept exact north-south
              // lines, because then we may not be able to determine
              // if a point lies inside or outside the polygon;
            if(x>x1)
              { bep->x1= x1; bep->y1= y1; bep->x2= x; bep->y2= y; }
            else
              { bep->x1= x; bep->y1= y; bep->x2= x1; bep->y2= y1; }
            bep->chain= NULL;
            if(loglevel>=1)
              fprintf(stderr,
                "- %i %i,%i,%i,%i\n",
                (int)(bep-border__edge),
                bep->x1,bep->y1,bep->x2,bep->y2);
            bep++;
            }  // end   there is a preceding coordinate
          x1= x; y1= y;
          if(x0==nil)
            { x0= x; y0= y; }
          }  // end   data plausible
        }  // end   inside a section
      if(sp==NULL)  // end of border file
    break;
      }  // end   for every line in border file
    bep->x1= nil;  // set terminator of edge list
    border__edge_n= bep-border__edge;  // set number of edges
    }  // end   get border polygon

  // sort edges ascending by x1 value
  if(loglevel>=1)
    fprintf(stderr,"Border polygons: %i. Now sorting.\n",
      border__edge_n);
  qsort(border__edge,border__edge_n,sizeof(border__edge_t),
    border__qsort_edge);

  /* generate chains for each edge */ {
    int32_t x2;
    border__chain_t* bcp;  // growing pointer in chain storage
    border__edge_t* bep;  // pointer in border__edge[]
    border__edge_t* bep2;  // referenced edge
    border__chain_t* bcp2;  // chain of referenced edge;

    bep= border__edge;
    bcp= border__chain;
    while(bep->x1!=nil) {  // for each edge in list
      if(loglevel>=1)
        fprintf(stderr,
          "> %i %i,%i,%i,%i\n",
          (int)(bep-border__edge),bep->x1,bep->y1,bep->x2,bep->y2);
      /*x1= bep->x1;*/ x2= bep->x2;
      bep2= bep;
      while(bep2>border__edge && (bep2-1)->x1==bep2->x1) bep2--;
        // we must examine previous edges having same x1 too;
      while(bep2->x1!=nil && bep2->x1 <= x2) {
          // for each following overlapping edge in list
        if(bep2==bep) {  // own edge
          bep2++;  // (needs not to be chained to itself)
      continue;
          }
        if(bcp>=border__chain+border__chain_M)
            // no more space in chain storage
return false;
        if(loglevel>=2)
          fprintf(stderr,"+ add to chain of %i\n",
            (int)(bep2-border__edge));
        bcp2= bep2->chain;
        if(bcp2==NULL)  // no chain yet
          bep2->chain= bcp;  // add first chain link
        else {  // edge already has a chain
          // go to the chain's end and add new chain link there

          while(bcp2->next!=NULL) bcp2= bcp2->next;
          bcp2->next= bcp;
          }  // end   edge already has a chain
        bcp->edge= bep;
          // add source edge to chain of overlapping edges
        bcp->next= NULL;  // new chain termination
        bcp++;
        bep2++;
        }  // for each following overlapping  edge in list
      bep++;
      }  // end   for each edge in list
    }  // end   generate chains for each edge

  // test output
  if(loglevel>=2) {
    border__edge_t* bep,*bep2;  // pointers in border__edge[]
    border__chain_t* bcp;  // pointer in chain storage

    fprintf(stderr,"Chains:\n");
    bep= border__edge;
    while(bep->x1!=nil) {  // for each edge in list
      fprintf(stderr,
        "> %i %i,%i,%i,%i\n",
        (int)(bep-border__edge),bep->x1,bep->y1,bep->x2,bep->y2);
      bcp= bep->chain;
      while(bcp!=NULL) {  // for each chain link in edge
        bep2= bcp->edge;
        fprintf(stderr,
          "  %i %i,%i,%i,%i\n",
          (int)(bep2-border__edge),
          bep2->x1,bep2->y1,bep2->x2,bep2->y2);
        bcp= bcp->next;
        }  // end   for each chain link in edge
      bep++;
      }  // end   for each edge in list
    }  // end   test output

  /* determine enclosing border box */ {
    border__edge_t* bep;  // pointer in border__edge[]

    border__bx1= border__edge[0].x1;
    border__bx2= -2000000000L;  // (default)
    border__by1= 2000000000L; border__by2= -2000000000L;  // (default)
    bep= border__edge;
    while(bep->x1!=nil) {  // for each coordinate of the polygon
      if(bep->x2>border__bx2) border__bx2= bep->x2;
      if(bep->y1<border__by1) border__by1= bep->y1;
      if(bep->y2<border__by1) border__by1= bep->y2;
      if(bep->y1>border__by2) border__by2= bep->y1;
      if(bep->y2>border__by2) border__by2= bep->y2;
      bep++;
      }  // end   for each coordinate of the polygon
    }  // end   determine enclosing border box
  border_active=true;
  if(loglevel>=1)
    fprintf(stderr,"End of border initialization.\n");
  return true;
  }  // end   border_file()

static bool border_queryinside(int32_t x,int32_t y) {
  // determine if the given coordinate lies inside or outside the
  // border polygon(s);
  // x,y: coordinates of the given point in 0.0000001 degrees;
  // return: point lies inside the border polygon(s);
  static int32_t nil;

  nil= border__nil;

  #if MAXLOGLEVEL>=3
  if(loglevel>=3)
    fprintf(stderr,"# %i,%i\n",x,y);
  #endif
  // first, consider border box (if any)
  if(border__bx1!=nil) {  // there is a border box
    if(x<border__bx1 || x>border__bx2 ||
        y<border__by1 || y>border__by2)
        // point lies outside the border box
return false;
    }  // end   there is a border box

  /* second, consider border polygon (if any) */ {
    border__edge_t* bep;  // pointer in border__edge[]
    border__chain_t* bcp;  // pointer in border__chain[]
    int cross;  // number of the crossings a line from the point
      // to the north pole would have ageinst the border lines
      // in border__coord[][];

    if(border__edge==NULL)
return true;
    cross= 0;

    /* binary-search the edge with the closest x1 */ {
      int i,i1,i2;  // iteration indexes

      i1= 0; i2= border__edge_n;
      while(i2>i1+1) {
        i= (i1+i2)/2;
        bep= border__edge+i;
//fprintf(stderr,"s %i %i %i   %i\n",i1,i,i2,bep->x1); ///
        if(bep->x1 > x) i2= i;
        else i1= i;
//fprintf(stderr,"  %i %i %i\n",i1,i,i2); ///
        }
      bep= border__edge+i1;
      }  // end   binary-search the edge with the closest x1

    bcp= NULL;
      // (default, because we want to examine the own edge first)
    for(;;) {  // for own edge and each edge in chain
      if(bep->x1 <= x && bep->x2 > x) {  // point lies inside x-range
        if(bep->y1 > y && bep->y2 > y) {
            // line lies completely north of point
          cross++;
          #if MAXLOGLEVEL>=3
          if(loglevel>=3)
            fprintf(stderr,"= %i %i,%i,%i,%i\n",
              (int)(bep-border__edge),bep->x1,bep->y1,bep->x2,bep->y2);
          #endif
          }
        else if(bep->y1 > y || bep->y2 > y) {
            // one line end lies north of point
          if( (int64_t)(y-bep->y1)*(int64_t)(bep->x2-bep->x1) <
              (int64_t)(x-bep->x1)*(int64_t)(bep->y2-bep->y1) ) {
              // point lies south of the line
            cross++;
            #if MAXLOGLEVEL>=3
            if(loglevel>=3)
              fprintf(stderr,"/ %i %i,%i,%i,%i\n",
                (int)(bep-border__edge),
                bep->x1,bep->y1,bep->x2,bep->y2);
            #endif
            }
          #if MAXLOGLEVEL>=3
          else if(loglevel>=3)
            fprintf(stderr,". %i %i,%i,%i,%i\n",
              (int)(bep-border__edge),
              bep->x1,bep->y1,bep->x2,bep->y2);
          #endif
          }  // end   one line end north of point
        #if MAXLOGLEVEL>=3
        else if(loglevel>=3)
            fprintf(stderr,"_ %i %i,%i,%i,%i\n",
              (int)(bep-border__edge),bep->x1,bep->y1,bep->x2,bep->y2);
        #endif
        }  // end   point lies inside x-range
      if(bcp==NULL)  // chain has not been examined
        bcp= bep->chain;  // get the first chain link
      else
        bcp= bcp->next;  // get the next chain link
      if(bcp==NULL)  // no more chain links
    break;
      bep= bcp->edge;
      }  // end   for own edge and each edge in chain
//if(loglevel>=3) fprintf(stderr,"# %i,%i cross %i\n",x,y,cross);
return (cross&1)!=0;  // odd number of crossings
    }  // end   second, consider border polygon (if any)
  }  // end   border_queryinside()

static void border_querybox(int32_t* x1,int32_t* y1,
    int32_t* x2,int32_t* y2) {
  // get the values of a previously defined border box;
  // border_box() or border_file() must have been called;
  // return values are valid only if border_active==true;
  // x1,y1;  // coordinates of southwestern corner;
  // x2,y2;  // coordinates of northeastern corner;
  if(!border_active)
    *x1= *y1= *x2= *y2= 0;
  else {
    *x1= border__bx1;
    *y1= border__by1;
    *x2= border__bx2;
    *y2= border__by2;
    }
  }  // end   border_querybox()

//------------------------------------------------------------
// end Module border_   OSM border module
//------------------------------------------------------------



//------------------------------------------------------------
// Module read_   OSM file read module
//------------------------------------------------------------

// this module provides procedures for buffered reading of
// standard input;
// as usual, all identifiers of a module have the same prefix,
// in this case 'read'; an underline will follow in case of a
// global accessible object, two underlines in case of objects
// which are not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----

#define read_PREFETCH ((32+3)*1024*1024)
  // number of bytes which will be available in the buffer after
  // every call of read_input();
  // (important for reading .pbf files:
  //  size must be greater than pb__blockM)
#define read__bufM (read_PREFETCH*5)  // length of the buffer;
typedef struct {  // members may not be accessed from external
  int fd;  // file descriptor
  bool eof;  // we are at the end of input file
  byte* bufp;  // pointer in buf[]
  byte* bufe;  // pointer to the end of valid input in buf[]
  uint64_t bufferstart;
    // dummy variable which marks the start of the read buffer
    // concatenated  with this instance of read info structure;
  } read_info_t;
static int64_t read__counter= 0;
  // byte counter to get the read position in input file;
  // this is an overall-sum of all input files;

//------------------------------------------------------------

static read_info_t* read_infop= NULL;
  // presently used read info structure, i.e. file handle
#define read__buf ((byte*)&read_infop->bufferstart)
  // start address of the file's input buffer
static byte* read_bufp= NULL;  // may be incremented by external
  // up to the number of read_PREFETCH bytes before read_input() is
  // called again;
static byte* read_bufe= NULL;  // may not be changed from external

static int read_open(const char* filename) {
  // open an input file;
  // filename[]: path and name of input file;
  //             ==NULL: standard input;
  // return: 0: ok; !=0: error;
  // read_infop: handle of the file;
  // note that you should close ever opened file with read_close()
  // before the program ends;

  // save status of presently processed input file (if any)
  if(read_infop!=NULL) {
    read_infop->bufp= read_bufp;
    read_infop->bufp= read_bufe;
    }

  // get memory space for file information and input buffer
  read_infop= (read_info_t*)malloc(sizeof(read_info_t)+read__bufM);
  if(read_infop==NULL) {
    fprintf(stderr,"osmconvert Error: could not get "
      "%i bytes of memory.\n",read__bufM);
return 1;
    }

  // initialize read info structure
  read_infop->fd= 0;  // (default) standard input
  read_infop->eof= false;  // we are at the end of input file
  read_infop->bufp= read_infop->bufe= read__buf;  // pointer in buf[]
    // pointer to the end of valid input in buf[]

  // set modul-global variables which are associated with this file
  read_bufp= read_infop->bufp;
  read_bufe= read_infop->bufe;

  // open the file
  if(loglevel>=2)
    fprintf(stderr,"Read-opening: %s",
      filename==NULL? "stdin": filename);
  if(filename==NULL)  // stdin shall be opened
    read_infop->fd= 0;
  else if(filename!=NULL) {  // a real file shall be opened
    read_infop->fd= open(filename,O_RDONLY|O_BINARY);
    if(read_infop->fd<0) {
      if(loglevel>=2)
        fprintf(stderr," -> failed\n");
      fprintf(stderr,
        "osmconvert Error: could not open input file: %.80s\n",
        filename==NULL? "standard input": filename);
      free(read_infop); read_infop= NULL;
      read_bufp= read_bufe= NULL;
return 1;
      }
    }  // end   a real file shall be opened
  if(loglevel>=2)
    fprintf(stderr," -> FD %i\n",read_infop->fd);
return 0;
  }  // end   read_open()

static void read_close(read_info_t* x) {
  // close an opened file;
  // read_infop: handle of the file which is to close;
  int fd;

  if(read_infop==NULL)  // handle not valid;
return;
  fd= read_infop->fd;
  if(loglevel>=2)
    fprintf(stderr,"Read-closing FD: %i\n",fd);
  if(fd>0)  // not standard input
    close(fd);
  free(read_infop); read_infop= NULL;
  read_bufp= read_bufe= NULL;
  }  // end   read_close()

static inline bool read_input() {
  // read data from standard input file, use an internal buffer;
  // make data available at read_bufp;
  // read_open() must have been called before calling this procedure;
  // return: there are no (more) bytes to read;
  // read_bufp: start of next bytes available;
  //            may be incremented by the caller, up to read_bufe;
  // read_bufe: end of bytes in buffer;
  //            must not be changed by the caller;
  // after having called this procedure, the caller may rely on
  // having available at least read_PREFETCH bytes at address
  // read_bufp - with one exception: if there are not enough bytes
  // left to read from standard input, every byte after the end of
  // the reminding part of the file in the buffer will be set to
  // 0x00 - up to read_bufp+read_PREFETCH;
  int l,r;

  if(read_bufp+read_PREFETCH>=read_bufe) {  // read buffer is too low
    if(!read_infop->eof) {  // still bytes in the file
      if(read_bufe>read_bufp) {  // bytes remaining in buffer
        memmove(read__buf,read_bufp,read_bufe-read_bufp);
          // move remaining bytes to start of buffer
        read_bufe= read__buf+(read_bufe-read_bufp);
          // protect the remaining bytes at buffer start
        }
      else  // no remaining bytes in buffer
        read_bufe= read__buf;  // no bytes remaining to protect
        // add read bytes to debug counter
      read_bufp= read__buf;
      do {  // while buffer has not been filled
        l= (read__buf+read__bufM)-read_bufe-4;
          // number of bytes to read
        r= read(read_infop->fd,read_bufe,l);
        if(r<=0) {  // no more bytes in the file
          read_infop->eof= true;
            // memorize that there we are at end of file
          l= (read__buf+read__bufM)-read_bufe;
            // reminding space in buffer
          if(l>read_PREFETCH) l= read_PREFETCH;
          memset(read_bufe,l,0);
            // set reminding space up to prefetch bytes in buffer to 0
      break;
          }
        read__counter+= r;
        read_bufe+= r;  // set new mark for end of data
        read_bufe[0]= 0; read_bufe[1]= 0;  // set 4 null-terminators
        read_bufe[2]= 0; read_bufe[3]= 0;
        } while(r<l);  // end   while buffer has not been filled
      }  // end   still bytes to read
    }  // end   read buffer is too low
  return read_infop->eof && read_bufp>=read_bufe;
  }  // end   read__input()

static void read_switch(read_info_t* filehandle) {
  // switch to another already opened file;
  // filehandle: handle of the file which shall be switched to;

  // first, save status of presently processed input file
  if(read_infop!=NULL) {
    read_infop->bufp= read_bufp;
    read_infop->bufe= read_bufe;
    }
  // switch to new file information
  read_infop= filehandle;
  read_bufp= read_infop->bufp;
  read_bufe= read_infop->bufe;
  read_input();
  }  // end   read_switch()

static inline uint64_t read_count() {
  // determine the number of bytes which have been read;
  return read__counter-(read_bufe-read_bufp);
  }

//------------------------------------------------------------
// end Module read_   OSM file read module
//------------------------------------------------------------



//------------------------------------------------------------
// Module write_   write module
//------------------------------------------------------------

// this module provides a procedure which writes a byte to
// standard output;
// as usual, all identifiers of a module have the same prefix,
// in this case 'write'; an underline will follow in case of a
// global accessible object, two underlines in case of objects
// which are not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----

static const char* write__filename= NULL;
  // last name of the file; ==NULL: standard output;

#include <stdint.h>
static char write__buf[UINT64_C(16000000)];
static char* write__bufe= write__buf+sizeof(write__buf);
  // (const) water mark for buffer filled 100%
static char* write__bufp= write__buf;
static int write__fd= 1;  // (initially standard output)
static inline void write_flush();

static void write__close() {
  // close the last opened file;
  if(loglevel>=2)
    fprintf(stderr,"Write-closing FD: %i\n",write__fd);
  write_flush();
  if(write__fd>1) {  // not standard output
    close(write__fd);
    write__fd= 1;
    }
  }  // end   read__close()

static void write__end() {
  // terminate the services of this module;
  write__close();
  if(loglevel<2) if(write__filename!=NULL) unlink(write__filename);
    // real output will be written via standard output; if the written
    // file has a name, we assume that it was just a temporary file;
  }  // end   read__end()

//------------------------------------------------------------

static bool write_testmode= false;  // no standard output

static inline void write_flush() {
  if(write__bufp>write__buf && !write_testmode)
      // at least one byte in buffer AND not test mode
    write(write__fd,write__buf,write__bufp-write__buf);
  write__bufp= write__buf;
  }  // end   write_flush();

static int write_newfile(const char* filename) {
  // change to another (temporary) output file;
  // filename: new name of the output file;
  //           this string must be accessible until program end
  //           because the name will be needed to delete the file;
  //           ==NULL: standard output;
  // the previous output file is closed by this procedure, unless
  // it is standard output;
  // return: 0: OK; !=0: error;
  static bool firstrun= true;

  write__close();
  if(loglevel>=2)
    fprintf(stderr,"Write-opening: %s\n",
      filename==NULL? "stdout": filename);
  if(filename==NULL)  // we are to write to standard output
    write__fd= 1;
  else {  // real file shall be opened
    write__filename= filename;
    unlink(filename);
    write__fd= open(filename,O_WRONLY|O_CREAT|0600|O_TRUNC|O_BINARY);
    if(write__fd<1) {
      fprintf(stderr,
        "osmconvert Error: could not open output file: %.80s\n",
        filename==NULL? "standard output": filename);
      write__fd= 1;
return 1;
      }
    #if !__WIN32__
    fchmod(write__fd,0600);  // (did not work at file creation time)
    #endif
    }
  if(firstrun) {
    firstrun= false;
    atexit(write__end);
    }
  return 0;
  }  // end   write_newfile()

static inline void write_char(int c) {
  // write one byte to stdout, use a buffer;
  if(write__bufp>=write__bufe) {  // the write buffer is full
    if(!write_testmode)
      write(write__fd,write__buf,write__bufp-write__buf);
    write__bufp= write__buf;
    }
  *write__bufp++= (char)c;
  }  // end   write_char();

static inline void write_mem(const byte* b,int l) {
  // write a memory area to stdout, use a buffer;
  while(--l>=0) {
    if(write__bufp>=write__bufe) {  // the write buffer is full
      if(!write_testmode)
        write(write__fd,write__buf,write__bufp-write__buf);
      write__bufp= write__buf;
      }
    *write__bufp++= (char)(*b++);
    }
  }  // end   write_mem();

static inline void write_str(const char* s) {
  // write a string to stdout, use a buffer;
  while(*s!=0) {
    if(write__bufp>=write__bufe) {  // the write buffer is full
      if(!write_testmode)
        write(write__fd,write__buf,write__bufp-write__buf);
      write__bufp= write__buf;
      }
    *write__bufp++= (char)(*s++);
    }
  }  // end   write_str();

static inline void write_xmlstr(const char* s) {
  // write an XML string to stdout, use a buffer;
  // every character which is not allowed within an XML string
  // will be replaced by the appropriate decimal sequence;
  static byte allowedchar[]= {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,  // \"&'
    0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,  // <>
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,  // {}DEL
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // 0xa0...
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  byte c,i;
  #define write__char_D(c) { \
    if(write__bufp>=write__bufe) { \
      if(!write_testmode) \
        write(write__fd,write__buf,write__bufp-write__buf); \
      write__bufp= write__buf; \
      } \
    *write__bufp++= (char)(c); }

  for(;;) {
    c= *s++;
    if(c==0)
  break;
    if(allowedchar[c]==0)  // this character may be written as is
      write__char_D(c)
    else {
      write__char_D('&') write__char_D('#') //write__char_D('x')
      i= c/100; if(i) write__char_D(i+'0')
      i= c/10%10; if(i) write__char_D(i+'0')
      write__char_D(c%10+'0')
      //write__char_D("0123456789abcdef"[c/16])
      //write__char_D("0123456789abcdef"[c%16])
      write__char_D(';')
      }
    }
  #undef write__char_D
  }  // end   write_xmlstr();

static inline void write_xmlmnstr(const char* s) {
  // write an XML string to stdout, use a buffer;
  // every character which is not allowed within an XML string
  // will be replaced by the appropriate mnemonic or decimal sequence;
  static byte allowedchar[]= {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,2,0,0,0,2,2,0,0,0,0,0,0,0,0,  // \"&'
    0,0,0,0,0,0,0,0,0,0,0,0,2,0,2,0,  // <>
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,  // {}DEL
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // 0xa0...
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  byte c,i;
  #define write__char_D(c) { \
    if(write__bufp>=write__bufe) { \
      if(!write_testmode) \
        write(write__fd,write__buf,write__bufp-write__buf); \
      write__bufp= write__buf; \
      } \
    *write__bufp++= (char)(c); }

  for(;;) {
    c= *s++;
    if(c==0)
  break;
    if(allowedchar[c]==0)  // this character may be written as is
      write__char_D(c)
    else if(allowedchar[c]==1) {
        // there is no mnemonic for this character
      write__char_D('&') write__char_D('#') //write__char_D('x')
      i= c/100; if(i) write__char_D(i+'0')
      i= c/10%10; if(i) write__char_D(i+'0')
      write__char_D(c%10+'0')
      write__char_D(';')
      }
    else {
        // there is a mnemonic for this character
      write__char_D('&')
      switch(c) {
      case '\"':
        write__char_D('q') write__char_D('u') write__char_D('o')
        write__char_D('t')
        break;
      case '&':
        write__char_D('a') write__char_D('m') write__char_D('p')
        break;
      case '\'':
        write__char_D('a') write__char_D('p') write__char_D('o')
        write__char_D('s')
        break;
      case '<':
        write__char_D('l') write__char_D('t')
        break;
      case '>':
        write__char_D('g') write__char_D('t')
        break;
      default:
        write__char_D('?')  // (should never reach here)
        }
      write__char_D(';')
      }
    }
  #undef write__char_D
  }  // end   write_xmlmnstr();

static inline void write_uint32(uint32_t v) {
  // write an unsigned 32 bit integer number to standard output;
  char s[20],*s1,*s2,c;

  s1= s;
  if(v==0)
    *s1++= '0';
  s2= s1;
  while(v>0)
    { *s2++= (v%10)+'0'; v/= 10; }
  *s2--= 0;
  while(s2>s1)
      { c= *s1; *s1= *s2; *s2= c; s1++; s2--; }
  write_str(s);
  }  // end write_uint32()

#if 0  // currently unused
static inline void write_sint32(int32_t v) {
  // write a signed 32 bit integer number to standard output;
  char s[20],*s1,*s2,c;

  s1= s;
  if(v<0)
    { *s1++= '-'; v= -v; }
  else if(v==0)
    *s1++= '0';
  s2= s1;
  while(v>0)
    { *s2++= (v%10)+'0'; v/= 10; }
  *s2--= 0;
  while(s2>s1)
    { c= *s1; *s1= *s2; *s2= c; s1++; s2--; }
  write_str(s);
  }  // end write_sint32()
#endif

static inline void write_uint64(uint64_t v) {
  // write an unsigned 64 bit integer number to standard output;
  char s[30],*s1,*s2,c;

  s1= s;
  if(v==0)
    *s1++= '0';
  s2= s1;
  while(v>0)
    { *s2++= (v%10)+'0'; v/= 10; }
  *s2--= 0;
  while(s2>s1)
      { c= *s1; *s1= *s2; *s2= c; s1++; s2--; }
  write_str(s);
  }  // end write_uint64()

static inline void write_sint64(int64_t v) {
  // write a signed 64 bit integer number to standard output;
  static char s[30],*s1,*s2,c;

  s1= s;
  if(v<0)
    { *s1++= '-'; v= -v; }
  else if(v==0)
    *s1++= '0';
  s2= s1;
  while(v>0)
    { *s2++= (v%10)+'0'; v/= 10; }
  *s2--= 0;
  while(s2>s1)
    { c= *s1; *s1= *s2; *s2= c; s1++; s2--; }
  write_str(s);
  }  // end write_sint64()

static inline void write_sfix7(int32_t v) {
  // write a signed 7 decimals fixpoint value to standard output;
  char s[20],*s1,*s2,c;
  int i;

  s1= s;
  if(v<0)
    { *s1++= '-'; v= -v; }
  s2= s1;
  i= 7;
  while((v%10)==0 && i>0)  // trailing zeroes
    { v/= 10;  i--; }
  while(--i>=0)
    { *s2++= (v%10)+'0'; v/= 10; }
  *s2++= '.';
  do
    { *s2++= (v%10)+'0'; v/= 10; }
    while(v>0);
  *s2--= 0;
  while(s2>s1)
    { c= *s1; *s1= *s2; *s2= c; s1++; s2--; }
  write_str(s);
  }  // end write_sfix7()

static inline void write_sfix7o(int32_t v) {
  // write a signed 7 decimals fixpoint value to standard output;
  // keep trailing zeroes;
  char s[20],*s1,*s2,c;
  int i;

  s1= s;
  if(v<0)
    { *s1++= '-'; v= -v; }
  s2= s1;
  i= 7;
  while(--i>=0)
    { *s2++= (v%10)+'0'; v/= 10; }
  *s2++= '.';
  do
    { *s2++= (v%10)+'0'; v/= 10; }
    while(v>0);
  *s2--= 0;
  while(s2>s1)
    { c= *s1; *s1= *s2; *s2= c; s1++; s2--; }
  write_str(s);
  }  // end write_sfix7o()

#if 0  // currently unused
static inline void write_sfix9(int64_t v) {
  // write a signed 9 decimals fixpoint value to standard output;
  char s[20],*s1,*s2,c;
  int i;

  s1= s;
  if(v<0)
    { *s1++= '-'; v= -v; }
  s2= s1;
  i= 9;
  while(--i>=0)
    { *s2++= (v%10)+'0'; v/= 10; }
  *s2++= '.';
  do
    { *s2++= (v%10)+'0'; v/= 10; }
    while(v>0);
  *s2--= 0;
  while(s2>s1)
    { c= *s1; *s1= *s2; *s2= c; s1++; s2--; }
  write_str(s);
  }  // end write_sfix9()
#endif

static inline void write_timestamp(uint64_t v) {
  // write a timestamp in OSM format, e.g.: "2010-09-30T19:23:30Z"
  time_t vtime;
  struct tm tm;
  char s[30],*sp;
  int i;

  vtime= v;
  #ifdef _WIN32
  memcpy(&tm,gmtime(&vtime),sizeof(tm));
  #else
  gmtime_r(&vtime,&tm);
  #endif
  i= tm.tm_year+1900;
  sp= s+3; *sp--= i%10+'0';
  i/=10; *sp--= i%10+'0';
  i/=10; *sp--= i%10+'0';
  i/=10; *sp= i%10+'0';
  sp+= 4; *sp++= '-';
  i= tm.tm_mon+1;
  *sp++= i/10+'0'; *sp++= i%10+'0'; *sp++= '-';
  i= tm.tm_mday;
  *sp++= i/10+'0'; *sp++= i%10+'0'; *sp++= 'T';
  i= tm.tm_hour;
  *sp++= i/10+'0'; *sp++= i%10+'0'; *sp++= ':';
  i= tm.tm_min;
  *sp++= i/10+'0'; *sp++= i%10+'0'; *sp++= ':';
  i= tm.tm_sec%60;
  *sp++= i/10+'0'; *sp++= i%10+'0'; *sp++= 'Z'; *sp= 0;
  write_str(s);
  }  // end   write_timestamp()

//------------------------------------------------------------
// end   Module write_   write module
//------------------------------------------------------------



//------------------------------------------------------------
// Module pb_   pbf read module
//------------------------------------------------------------

// this module provides procedures which read osm .pbf objects;
// it uses procedures from modules read_ and pbf_;
// as usual, all identifiers of a module have the same prefix,
// in this case 'pb'; an underline will follow in case of a
// global accessible object, two underlines in case of objects
// which are not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----

static int pb__decompress(byte* ibuf,uint isiz,byte* obuf,uint osizm,
  uint* osizp) {
  // decompress a block of data;
  // return: 0: decompression was successful;
  //         !=0: error number from zlib;
  // *osizp: size of uncompressed data;
  z_stream strm;
  int r,i;

  // initialization
  strm.zalloc= Z_NULL;
  strm.zfree= Z_NULL;
  strm.opaque= Z_NULL;
  strm.next_in= Z_NULL;
  strm.total_in= 0;
  strm.avail_out= 0;
  strm.next_out= Z_NULL;
  strm.total_out= 0;
  strm.msg= NULL;
  r= inflateInit(&strm);
  if(r!=Z_OK)
return r;
  // read data
  strm.next_in = ibuf;
  strm.avail_in= isiz;
  // decompress
  strm.next_out= obuf;
  strm.avail_out= osizm;
  r= inflate(&strm,Z_FINISH);
  if(r!=Z_OK && r!=Z_STREAM_END) {
    inflateEnd(&strm);
    *osizp= 0;
return r;
    }
  // clean-up
  inflateEnd(&strm);
  obuf+= *osizp= osizm-(i= strm.avail_out);
  // add some zero bytes
  if(i>4) i= 4;
  while(--i>=0) *obuf++= 0;
  return 0;
  }  // end   pb__decompress()

// for string primitive group table
#define pb__strM (4*1024*1024)
  // maximum number of strings within each block
static char* pb__str[pb__strM];  // string table
static char** pb__stre= pb__str;  // end of data in str[]
static char** pb__stree= pb__str+pb__strM;  // end of str[]
static int pb__strm= 0;
// for tags of densnodes (start and end address)
static byte* pb__nodetags= NULL,*pb__nodetagse= NULL;  // node tag pairs
// for noderefs and tags of ways (start and end address each)
static byte* pb__waynode= NULL,*pb__waynodee= NULL;
static byte* pb__waykey= NULL,*pb__waykeye= NULL;
static byte* pb__wayval= NULL,*pb__wayvale= NULL;
// for refs and tags of relations (start and end address each)
static byte* pb__relrefrole= NULL,*pb__relrefrolee= NULL;
static byte* pb__relrefid= NULL,*pb__relrefide= NULL;
static byte* pb__relreftype= NULL,*pb__relreftypee= NULL;
static byte* pb__relkey= NULL,*pb__relkeye= NULL;
static byte* pb__relval= NULL,*pb__relvale= NULL;

//------------------------------------------------------------

static bool pb_bbvalid= false;
  // the following bbox coordinates are valid;
static int32_t pb_bbx1,pb_bby1,pb_bbx2,pb_bby2;
  // bbox coordinates (base 10^-7);
static int pb_type= -9;  // type of the object which has been read;
  // 0: node; 1: way; 2: relation; 8: header;
  // -1: end of file; <= -10: error;
static int64_t pb_id= 0;  // id of read object
static int32_t pb_lon= 0,pb_lat= 0;  // coordinates of read node
static int32_t pb_hisver= 0;
static int64_t pb_histime= 0;
static int64_t pb_hiscset= 0;
static uint32_t pb_hisuid= 0;
static string  pb_hisuser= "";

static void pb_ini() {
  // initialize this module;
  // must be called as first procedure of this module;
  }  // end   pb_ini()

static int pb_input() {
  // read next pbf object and make it available via other
  // procedures of this mudule;
  // pb_ini() must have been called before calling this procedure;
  // return: >=0: OK; -1: end of file; <=-10: error; 
  // pb_type: type of the object which has been read;
  // in dependence of object's type the following information
  // will be available:
  // pb_bbvalid: the following bbox coordinates are valid;
  // pb_bbx1,pb_bby1,pb_bbx2,pb_bby2: bbox coordinates (base 10^-7);
  // pb_id: id of this object;
  // pb_lon: latitude in 100 nanodegree;
  // pb_lat: latitude in 100 nanodegree;
  // pb_hisver: version;
  // pb_histime: time (seconds since 1970)
  // pb_hiscset: changeset
  // pb_hisuid: uid; ==0: no user information available;
  // pb_hisuser: user name
  // subsequent to calling this procedure, the caller may call
  // the following procedures - depending on pb_type():
  // pb_noderef(), pb_ref(), pb_keyval()
  // the caller may omit these subsequent calls for ways and relations,
  // but he must not temporarily omit them for nodes;
  // if he omits such a subsequent call for one node, he must not
  // call pb_keyval() for any other of the following nodes because
  // this would result in wrong key/val data;
  #define PERR(f) \
    fprintf(stderr,"osmconvert Error: " f "\n");
    // print error message
  #define PERRv(f,...) \
    fprintf(stderr,"osmconvert Error: " f "\n",__VA_ARGS__);
    // print error message with value(s)
  #define END(r) {pb_type= (r); goto end;}
    // jump to procedure's end and provide a return code
  #define ENDE(r,f) { PERR(f) END(r) }
    // print error message, then jump to end
  #define ENDEv(r,f,...) { PERRv(f,__VA_ARGS__) END(r) }
    // print error message with value(s), then jump to end
  #define WARN(f) { static int msgn= 3; if(--msgn>=0) \
    fprintf(stderr,"osmconvert Warning: " f "\n"); }
    // print a warning message, do it maximal 3 times
  #define WARNv(f,...) { static int msgn= 3; if(--msgn>=0) \
    fprintf(stderr,"osmconvert Warning: " f "\n",__VA_ARGS__); }
    // print a warning message with value(s), do it maximal 3 times
  #define STRCHECK(s,t) { if((s)>=pb__strm) { if(s==32767) s= 0; else \
     ENDEv(__LINE__,t " string index overflow: %u->%u",pb__strm,s) } }
  int blocktype= -1;
    // -1: expected; 0: unknown; 1: Header; 2: Data;
  #define pb__blockM (32*1024*1024)  // maximum block size
  static byte zbuf[pb__blockM+1000];
  static byte* zbufp= zbuf,*zbufe= zbuf;
  static byte* groupp= zbuf,*groupe= zbuf;
    // memory area for primitive groups
  // start and end of different arrays, all used for dense nodes:
  static byte* nodeid= NULL,*nodeide= NULL;  // node ids
  static byte* nodever= NULL,*nodevere= NULL;  // versions
  static byte* nodetime= NULL,*nodetimee= NULL;  // times
  static byte* nodecset= NULL,*nodecsete= NULL;  // change sets
  static byte* nodeuid= NULL,*nodeuide= NULL;  // user ids
  static byte* nodeuser= NULL,*nodeusere= NULL;  // user names
  static byte* nodelat= NULL,*nodelate= NULL;  // node latitudes
  static byte* nodelon= NULL,*nodelone= NULL;  // node longitudes
  static uint32_t hisuser= 0;  // string index of user name (delta coded)
  static bool waycomplete= false,relcomplete= false;

  for(;;) {  // until we have a new object
  mainloop:
    if(nodeid<nodeide && nodelat<nodelate &&
        nodelon<nodelone) {  // dense nodes left
      // provide a node
      pb_id+= pbf_sint64(&nodeid);
      pb_lat+= pbf_sint32(&nodelat);
      pb_lon+= pbf_sint32(&nodelon);
      if(nodever>=nodevere || nodetime>=nodetimee ||
          nodecset>=nodecsete || nodeuid>=nodeuide ||
          nodeuser>=nodeusere)  // no history information available
        pb_hisver= 0;
      else {  // history information available
        pb_hisver= pbf_uint32(&nodever);
        pb_histime+= pbf_sint64(&nodetime);
        pb_hiscset+= pbf_sint64(&nodecset);
        pb_hisuid+= pbf_sint32(&nodeuid);
        hisuser+= pbf_sint32(&nodeuser);
        STRCHECK(hisuser,"node user")
        pb_hisuser= pb__str[hisuser];
        }  // end   history information available
      END(0)
      }  // dense nodes left
    if(waycomplete) {  // ways left
      // provide a way
      waycomplete= false;
      // (already got id and history integers)
      if(pb_hisver!=0 && hisuser>0) {
          // history information available
        STRCHECK(hisuser,"way user")
        pb_hisuser= pb__str[hisuser];
        }  // end   history information available
      END(1)
      }  // ways left
    if(relcomplete) {  // relations left
      // provide a relation
      relcomplete= false;
      // (already got id and history integers)
      if(pb_hisver!=0 && hisuser>0) {
          // history information available
        STRCHECK(hisuser,"rel user")
        pb_hisuser= pb__str[hisuser];
        }  // end   history information available
      END(2)
      }  // relations left
    if(groupp<groupe) {  // data in primitive group left
//// provide a primitive group object
      byte* bp;
      int l;

      bp= groupp;
      while(bp<groupe) {  // for each element in primitive group
        switch(bp[0]) {  // first byte of primitive group element
        case 0x0a:  // S 1, normal nodes
          ENDE(-201,"can only process dense nodes.")
  //// dense nodes
        case 0x12:  // S 2, dense nodes
          bp++;
          l= pbf_uint32(&bp);
          if(bp+l>groupe)
            ENDEv(-202,"dense nodes too large: %u",l)
          groupp= bp+l;
          /* get all node data lists */ {
            // decode dense node part of primitive group of Data block
            byte* dne;  // end of dense node memory area
            uint l;
            byte* bhise;  // end of history section in buf[]

            dne= groupp;
            while(bp<dne) {  // for every element in this loop
              switch(bp[0]) {  // first byte of element
              case 0x0a:  // S 1, ids
                bp++;
                l= pbf_uint32(&bp);
                if(bp+l>dne)
                  ENDEv(-301,"node id table too large: %u",l)
                nodeid= bp;
                nodeide= (bp+= l);
                break;
              case 0x2a:  // S 5, history - with subelements
                bp++;
                l= pbf_uint32(&bp);
                if(bp+l>dne)
                  ENDEv(-302,"node history section too large: %u",l)
                if(global_drophistory) {
                    // history information is not required
                  bp+= l;  // jump over this section and ignore it
                  break;
                  }
                bhise= bp+l;
                while(bp<bhise) {  // for each history subelement
                  switch(bp[0]) {
                      // first byte of element in history section
                  case 0x0a:  // S 1, versions
                    bp++;
                    l= pbf_uint32(&bp);
                    if(bp+l>bhise)
                      ENDEv(-303,"node version table too large: %u",l)
                    nodever= bp;
                    nodevere= (bp+= l);
                    break;
                  case 0x12:  // S 2, times
                    bp++;
                    l= pbf_uint32(&bp);
                    if(bp+l>bhise)
                      ENDEv(-304,"node time table too large: %u",l)
                    nodetime= bp;
                    nodetimee= (bp+= l);
                    break;
                  case 0x1a:  // S 3, change sets
                    bp++;
                    l= pbf_uint32(&bp);
                    if(bp+l>bhise)
                      ENDEv(-305,
                        "node change set table too large: %u",l)
                    nodecset= bp;
                    nodecsete= (bp+= l);
                    break;
                  case 0x22:  // S 4, user ids
                    bp++;
                    l= pbf_uint32(&bp);
                    if(bp+l>bhise)
                      ENDEv(-306,"node user id table too large: %u",l)
                    nodeuid= bp;
                    nodeuide= (bp+= l);
                    break;
                  case 0x2a:  // S 5, user names
                    bp++;
                    l= pbf_uint32(&bp);
                    if(bp+l>bhise)
                      ENDEv(-307,"node user name table too large: %u",l);
                    nodeuser= bp;
                    nodeusere= (bp+= l);
                    break;
                  default:
                    WARNv("node history element type unknown: "
                          "0x%02X 0x%02X.",bp[0],bp[1])
                    if(pbf_jump(&bp))
                      END(-308)
                    }  // end   first byte of element
                  }  // end   for each history subelement
                if(bp>bhise)
                  ENDE(-309,"node history format length.")
                bp= bhise;
                break;  // end   history - with subelements
              case 0x42:  // S 8, latitudes
                bp++;
                l= pbf_uint32(&bp);
                if(bp+l>dne)
                  ENDEv(-310,"node latitude table too large: %u",l)
                nodelat= bp;
                nodelate= (bp+= l);
                break;
              case 0x4a:  // S 9, longitudes
                bp++;
                l= pbf_uint32(&bp);
                if(bp+l>dne)
                  ENDEv(-311,"node longitude table too large: %u",l)
                nodelon= bp;
                nodelone= (bp+= l);
                break;
              case 0x52:  // S 10, tag pairs
                bp++;
                l= pbf_uint32(&bp);
                if(bp+l>dne)
                  ENDEv(-312,"node tag pair table too large: %u",l)
                pb__nodetags= bp;
                pb__nodetagse= (bp+= l);
                break;
              default:
                WARNv("dense node element type unknown: "
                  "0x%02X 0x%02X.",bp[0],bp[1])
                if(pbf_jump(&bp))
                  END(-313)
                }  // end   first byte of element
              if(bp>dne)
                ENDE(-314,"dense node format length.")
              }  // end   for every element in this loop
            // reset (delta-coded) variables
            pb_id= 0;
            pb_lat= pb_lon= 0;
            pb_histime= 0;
            pb_hiscset= 0;
            pb_hisuid= 0;
            hisuser= 0;
            pb_hisuser= "";
            bp= groupp;
            if(nodeid<nodeide && nodelat<nodelate && nodelon<nodelone)
                // minimum contents available
              goto mainloop;
            }  // get all node data lists
          break;
  //// ways
        case 0x1a:  // S 3, ways
          bp++;
          l= pbf_uint32(&bp);
          if(bp+l>groupe)
            ENDEv(-204,"ways too large: %u",l)
          groupp= bp+l;
          /* get way data */ {
            byte* bpe;  // end of ways memory area
            uint l;
            byte* bhise;  // end of history section in zbuf[]
            int complete;
              // flags which determine if the dataset is complete
            int hiscomplete;
              // flags which determine if the history is complete

            bpe= groupp;
            complete= hiscomplete= 0;
            while(bp<bpe) {  // for every element in this primitive group
              switch(bp[0]) {  // first byte of element
              case 0x08:  // V 1, id
                bp++;
                pb_id= pbf_uint64(&bp);
                complete|= 1;
                break;
              case 0x12:  // S 2, keys
                bp++;
                l= pbf_uint32(&bp);
                if(bp+l>bpe)
                  ENDEv(-401,"way key table too large: %u",l)
                pb__waykey= bp;
                pb__waykeye= (bp+= l);
                complete|= 2;
                break;
              case 0x1a:  // S 3, vals
                bp++;
                l= pbf_uint32(&bp);
                /* deal with strange S 3 element at data set end */ {
                  if(complete & (4|16)) {
                      // already have vals or node refs
                    WARNv("format 0x1a found: %02X",complete)
                    break;  // ignore this element
                    }
                  }
                if(bp+l>bpe)
                  ENDEv(-403,"way val table too large: %u",l)
                pb__wayval= bp;
                pb__wayvale= (bp+= l);
                complete|= 4;
                break;
              case 0x22:  // S 4, history - with subelements
                bp++;
                l= pbf_uint32(&bp);
                if(bp+l>bpe)
                  ENDEv(-404,"way history section too large: %u",l)
                if(global_drophistory) {
                    // history information is not required
                  bp+= l;  // jump over this section and ignore it
                  break;
                  }
                bhise= bp+l;
                while(bp<bhise) {  // for each history subelement
                  switch(bp[0]) {
                      // first byte of element in history section
                  case 0x08:  // V 1, version
                    bp++;
                    pb_hisver= pbf_uint32(&bp);
                    hiscomplete|= 1;
                    break;
                  case 0x10:  // V 2, timestamp
                    bp++;
                    pb_histime= pbf_uint64(&bp);
                    hiscomplete|= 2;
                    break;
                  case 0x18:  // V 3, cset
                    bp++;
                    pb_hiscset= pbf_uint64(&bp);
                    hiscomplete|= 4;
                    break;
                  case 0x20:  // V 4, uid
                    bp++;
                    pb_hisuid= pbf_uint32(&bp);
                    hiscomplete|= 8;
                    break;
                  case 0x28:  // V 5, user
                    bp++;
                    hisuser= pbf_uint32(&bp);
                    hiscomplete|= 16;
                    break;
                  default:
                    WARNv("way history element type unknown: "
                      "0x%02X 0x%02X.",bp[0],bp[1])
                    if(pbf_jump(&bp))
                      END(-408)
                    }  // end   first byte of element
                  }  // end   for each history subelement
                if(bp>bhise)
                  ENDE(-411,"way history format length.")
                bp= bhise;
                complete|= 8;
                break;  // end   history - with subelements
              case 0x42:  // S 8, node refs
                bp++;
                l= pbf_uint32(&bp);
                if(bp+l>bpe)
                  ENDEv(-412,"way noderef table too large: %u",l)
                pb__waynode= bp;
                pb__waynodee= (bp+= l);
                complete|= 16;
                break;
              default:
                WARNv("way element type unknown: "
                  "0x%02X 0x%02X 0x%02X 0x%02X + %i.",
                  bp[0],bp[1],bp[2],bp[3],complete)
                if(pbf_jump(&bp))
                  END(-421)
                }  // end   first byte of element
              if(bp>bpe)
                ENDE(-429,"way format length.")
              }  // for every element in this primitive group
            bp= groupp;
            if((hiscomplete&7)!=7)  // history information not available
              pb_hisver= 0;
            else if((hiscomplete&24)!=24)  // no user information
              pb_hisuid= 0;
            if((complete & 17)==17) {  // minimum contents available
                // (at least id and node refs)
              waycomplete= true;
              goto mainloop;
              }
            }  // get way data
          break;
  //// relations
        case 0x22:  // S 4, rels
          bp++;
          l= pbf_uint32(&bp);
          if(bp+l>groupe)
            ENDEv(-206,"rels too large: %u",l)
          groupp= bp+l;
          if(!global_droprelations) {  // relations are required
            /* get relation data */
            byte* bpe;  // end of ways memory area
            uint l;
            byte* bhise;  // end of history section in zbuf[]
            int complete;
              // flags which determine if the dataset is complete
            int hiscomplete;
              // flags which determine if the history is complete

            bpe= groupp;
            complete= hiscomplete= 0;
            while(bp<bpe) {  // for every element in this primitive group
              switch(bp[0]) {  // first byte of element
              case 0x08:  // V 1, id
                bp++;
                pb_id= pbf_uint64(&bp);
                complete|= 1;
                break;
              case 0x12:  // S 2, keys
                bp++;
                l= pbf_uint32(&bp);
                if(bp+l>bpe)
                  ENDEv(-501,"rel key table too large: %u",l)
                pb__relkey= bp;
                pb__relkeye= (bp+= l);
                complete|= 2;
                break;
              case 0x1a:  // S 3, vals
                bp++;
                l= pbf_uint32(&bp);
                if(bp+l>bpe)
                  ENDEv(-502,"rel val table too large: %u",l)
                pb__relval= bp;
                pb__relvale= (bp+= l);
                complete|= 4;
                break;
              case 0x22:  // S 4, history - with subelements
                bp++;
                l= pbf_uint32(&bp);
                if(bp+l>bpe)
                  ENDEv(-503,"rel history section too large: %u",l)
                if(global_drophistory) {
                    // history information is not required
                  bp+= l;  // jump over this section and ignore it
                  break;
                  }
                bhise= bp+l;
                while(bp<bhise) {  // for each history subelement
                  switch(bp[0]) {
                    // first byte of element in history section
                  case 0x08:  // V 1, version
                    bp++;
                    pb_hisver= pbf_uint32(&bp);
                    hiscomplete|= 1;
                    break;
                  case 0x10:  // V 2, timestamp
                    bp++;
                    pb_histime= pbf_uint64(&bp);
                    hiscomplete|= 2;
                    break;
                  case 0x18:  // V 3, cset
                    bp++;
                    pb_hiscset= pbf_uint64(&bp);
                    hiscomplete|= 4;
                    break;
                  case 0x20:  // V 4, uid
                    bp++;
                    pb_hisuid= pbf_uint32(&bp);
                    hiscomplete|= 8;
                    break;
                  case 0x28:  // V 5, user
                    bp++;
                    hisuser= pbf_uint32(&bp);
                    hiscomplete|= 16;
                    break;
                  default:
                    WARNv("rel history element type unknown: "
                      "0x%02X 0x%02X.",bp[0],bp[1])
                    if(pbf_jump(&bp))
                      END(-509)
                    }  // end   first byte of element
                  }  // end   for each history subelement
                if(bp>bhise)
                  ENDE(-510,"rel history format length.")
                bp= bhise;
                complete|= 8;
                break;  // end   history - with subelements
              case 0x42:  // S 8, refroles
                bp++;
                l= pbf_uint32(&bp);
                if(bp+l>bpe)
                  ENDEv(-511,"rel role table too large: %u",l)
                pb__relrefrole= bp;
                pb__relrefrolee= (bp+= l);
                complete|= 16;
                break;
              case 0x4a:  // S 9, refids
                bp++;
                l= pbf_uint32(&bp);
                if(bp+l>bpe)
                  ENDEv(-512,"rel id table too large: %u",l)
                pb__relrefid= bp;
                pb__relrefide= (bp+= l);
                complete|= 32;
                break;
              case 0x52:  // S 10, reftypes
                bp++;
                l= pbf_uint32(&bp);
                if(bp+l>bpe)
                  ENDEv(-513,"rel type table too large: %u",l)
                pb__relreftype= bp;
                pb__relreftypee= (bp+= l);
                complete|= 64;
                break;
              default:
                WARNv("rel element type unknown: "
                  "0x%02X 0x%02X 0x%02X 0x%02X + %i.",
                  bp[0],bp[1],bp[2],bp[3],complete)
                if(pbf_jump(&bp))
                  END(-514)
                }  // end   first byte of element
              if(bp>bpe)
                ENDE(-519,"rel format length.")
              }  // for every element in this primitive group
            bp= groupp;
            if((hiscomplete&7)!=7)  // history information not available
              pb_hisver= 0;
            else if((hiscomplete&24)!=24)  // no user information
              pb_hisuid= 0;
            if((complete & 113)==113) {  // minimum contents available
                // have at least id and refs (1|16|32|64)
              relcomplete= true;
              goto mainloop;
              }
            }  // get way data
          break;
        default:
          WARNv("primitive group element type unknown: "
                "0x%02X 0x%02X.",bp[0],bp[1])
          if(pbf_jump(&bp))
            END(-209)
          }  // end   first byte of primitive group element
        }  // end   for each element in primitive group
      }  // data in primitive group left
    if(zbufp<zbufe) {  // zbuf data left
//// provide next primitive group
      if(blocktype==1) {  // header block
        bool osmschema,densenodes;
        byte* bp;
        uint l;
        byte* bboxe;
        int64_t coord;
          // temporary, for rounding purposes sfix9 -> sfix7
        int bboxflags;

        osmschema= false;
        densenodes= false;
        bboxflags= 0;
        bp= zbufp;
        while(bp<zbufe) {  // for every element in this loop
          switch(bp[0]) {  // first byte of element
          case 0x0a:  // S 1, bbox
            bp++;
            l= pbf_uint32(&bp);
            if(l>=100)  // bbox group too large
              ENDEv(-41,"bbox group too large: %u",l)
            bboxe= bp+l;
            while(bp<bboxe) {  // for every element in bbox
              switch(bp[0]) {  // first byte of element in bbox
              case 0x08:  // V 1, minlon
                bp++;
                coord= pbf_sint64(&bp);
                if(coord<0) coord-= 99;
                pb_bbx1= coord/100;
                bboxflags|= 0x01;
                break;
              case 0x10:  // V 2, maxlon
                bp++;
                coord= pbf_sint64(&bp);
                if(coord>0) coord+= 99;
                pb_bbx2= coord/100;
                bboxflags|= 0x02;
                break;
              case 0x18:  // V 3, maxlat
                bp++;
                coord= pbf_sint64(&bp);
                if(coord>0) coord+= 99;
                pb_bby2= coord/100;
                bboxflags|= 0x04;
                break;
              case 0x20:  // V 4, minlat
                bp++;
                coord= pbf_sint64(&bp);
                if(coord<0) coord-= 99;
                pb_bby1= coord/100;
                bboxflags|= 0x08;
                break;
              default:
                WARNv("bbox element type unknown: "
                  "0x%02X 0x%02X.",bp[0],bp[1])
                if(pbf_jump(&bp))
                  END(-42)
                }  // end   first byte of element
              if(bp>bboxe)
                ENDE(-43,"bbox format length.")
              }  // end   for every element in bbox
            bp= bboxe;
            break;
          case 0x22:  // S 4, required features
            bp++;
            l= pbf_uint32(&bp);
            if(memcmp(bp-1,"\x0e""OsmSchema-V0.6",15)==0)
              osmschema= true;
            else if(memcmp(bp-1,"\x0a""DenseNodes",11)==0)
              densenodes= true;
            else  // unsupported feature
              ENDEv(-44,"unsupported feature: %.*s",l>50? 50: l,bp)
            bp+= l;
            break;
          case 0x82:  // 0x01 S 16, writing program
            if(bp[1]!=0x01) goto h_unknown;
            bp+= 2;
            l= pbf_uint32(&bp);
            bp+= l;  // (ignore this element)
            break;
          case 0x8a:  // 0x01 S 17, source
            if(bp[1]!=0x01) goto h_unknown;
            bp+= 2;
            l= pbf_uint32(&bp);
            bp+= l;  // (ignore this element)
            break;
          default:
          h_unknown:  
            WARNv("header block element type unknown: "
              "0x%02X 0x%02X.",bp[0],bp[1])
            if(pbf_jump(&bp))
              END(-45)
            }  // end   first byte of element
          if(bp>zbufe)
            ENDE(-46,"header block format length.")
          }  // end   for every element in this loop
        if(!osmschema)
          ENDE(-47,"expected feature: OsmSchema-V0.6")
        if(!densenodes)
          ENDE(-48,"expected feature: DenseNodes")
        zbufp= bp;
        pb_bbvalid= bboxflags==0x0f;
        END(8)
        }  // header block
      // here: data block
      // provide primitive groups
      /* process data block */ {
        byte* bp;
        uint l;
        static byte* bstre;  // end of string table in zbuf[]

        bp= zbufp;
        pb__stre= pb__str;
        while(bp<zbufe) {  // for every element in this loop
          switch(bp[0]) {  // first byte of element
          case 0x0a:  // S 1, string table
            bp++;
            l= pbf_uint32(&bp);
            if(bp+l>zbufe)
              ENDEv(-101,"string table too large: %u",l)
            bstre= bp+l;
            while(bp<bstre) {  // for each element in string table
              if(bp[0]==0x0a) {  // S 1, string
                *bp++= 0;  // set null terminator for previous string
                l= pbf_uint32(&bp);
                if(bp+l>bstre)  // string too large
                  ENDEv(-102,"string too large: %u",l)
                if(pb__stre>=pb__stree)
                  ENDEv(-103,"too many strings: %i",pb__strM)
                *pb__stre++= (char*)bp;
                bp+= l;
                }  // end   S 1, string
              else {  // element type unknown
                byte* p;

                WARNv("string table element type unknown: "
                  "0x%02X 0x%02X.",bp[0],bp[1])
                p= bp;
                if(pbf_jump(&bp))
                  END(-104)
                *p= 0;  // set null terminator for previous string
                }  // end   element type unknown
              }  // end   for each element in string table
            pb__strm= pb__stre-pb__str;
            bp= bstre;
            break;
          case 0x12:  // S 2, primitive group
            *bp++= 0;  // set null terminator for previous string
            l= pbf_uint32(&bp);
            if(bp+l>zbufe)
              ENDEv(-111,"primitive group too large: %u",l)
            groupp= bp;
            groupe= bp+l;
            zbufp= groupe;
  /**/goto mainloop;  // we got a new primitive group
          case 0x88:  // 0x01 V 17, nanodegrees
            if(bp[1]!=0x01) goto d_unknown;
            bp+= 2;
            l= pbf_uint32(&bp);
            if(l!=100)
              ENDEv(-121,"node nanodegrees must be 100: %u",l)
            break;
          case 0x90:  // 0x01 V 18, millisec
            if(bp[1]!=0x01) goto d_unknown;
            bp+= 2;
            l= pbf_uint32(&bp);
            if(l!=1000)
              ENDEv(-122,"node milliseconds must be 1000: %u",l)
            break;
          case 0x98:  // 0x01 V 19, latitude offset
            if(bp[1]!=0x01) goto d_unknown;
            bp+= 2;
            if(pbf_sint64(&bp)!=0)
              ENDE(-123,"cannot process latitude offsets.")
            break;
          case 0xa0:  // 0x01 V 20, longitude offset
            if(bp[1]!=0x01) goto d_unknown;
            bp+= 2;
            if(pbf_sint64(&bp)!=0)
              ENDE(-124,"cannot process longitude offsets.")
            break;
          d_unknown:
          default:
            /* block */ {
              byte* p;
              WARNv("data block element type unknown: "
                "0x%02X 0x%02X.",bp[0],bp[1])
              p= bp;
              if(pbf_jump(&bp))
                END(-125)
              *p= 0;  // set null terminator for previous string
              }  // end   block
            }  // end   first byte of element
          if(bp>zbufe)
            ENDE(-129,"data block format length.")
          }  // end   for every element in this loop
        }  // process data block
      }  // zbuf data left
//// provide new zbuf data
    /* get new zbuf data */ {
      int datasize;  // -1: expected;
      int rawsize;  // -1: expected;
      int zdata;  // -1: expected;
        // 1: encountered section with compressed data
      uint l;
      static byte zbuf[pb__blockM+1000];
      byte* p;
      int r;

      // initialization
      blocktype= datasize= rawsize= zdata= -1;

      // care for new input data
      if(read_bufp>read_bufe)
        ENDE(-11,"main format length.")
      read_input();  // get at least maximum block size
      if(read_bufp>=read_bufe)  // at end of input file
        END(-1)
      if(read_bufp[0]!=0)  // unknown id at outermost level
        ENDEv(-12,"main-element type unknown: "
          "0x%02X 0x%02X.",read_bufp[0],read_bufp[1])
      if(read_bufp[1]!=0 || read_bufp[2]!=0 ||
          read_bufp[3]<11 || read_bufp[3]>17)
        ENDEv(-13,"format blob header %i.",
          read_bufp[1]*65536+read_bufp[2]*256+read_bufp[3])
      read_bufp+= 4;

      // read new block header
      for(;;) {  // read new block
        if(blocktype<0) {  // block type expected
          if(read_bufp[0]!=0x0a)  // not id S 1
            ENDEv(-21,"block type expected at: 0x%02X.",*read_bufp)
          read_bufp++;
          if(memcmp(read_bufp,"\x09OSMHeader",10)==0) {
            blocktype= 1;
            read_bufp+= 10;
      continue;
            }
          if(memcmp(read_bufp,"\x07OSMData",8)==0) {
            blocktype= 2;
            read_bufp+= 8;
      continue;
            }
          blocktype= 0;
          l= pbf_uint32(&read_bufp);
          if(read_bufp+l>read_bufe)  // string too long
            ENDEv(-22,"block type too long: %.40s",read_bufp)
          WARNv("block type unknown: %.40s\n",read_bufp)
          read_bufp+= l;
      continue;
          }  // end   block type expected
        if(datasize<0) {  // data size expected
          if(read_bufp[0]!=0x18)  // not id V 3
            ENDEv(-23,"block data size "
              "expected at: 0x%02X.",*read_bufp)
          read_bufp++;
          datasize= pbf_uint32(&read_bufp);
          }  // end   data size expected
        if(blocktype==0) {  // block type unknown
          read_bufp+= datasize;  // jump over this block
      continue;
          }  // end   block type unknown
        if(rawsize<0) {  // raw size expected
          if(read_bufp[0]!=0x10)  // not id V 2
            ENDEv(-24,"block raw size "
              "expected at: 0x%02X.",*read_bufp)
          p= read_bufp;
          read_bufp++;
          rawsize= pbf_uint32(&read_bufp);
          datasize-= read_bufp-p;
          }  // end   raw size expected
        if(zdata<0) {  // compressed data expected
          if(read_bufp[0]!=0x1a)  // not id S 3
            ENDEv(-25,"compressed data "
              "expected at: 0x%02X.",*read_bufp)
          p= read_bufp;
          read_bufp++;
          l= pbf_uint32(&read_bufp);
          datasize-= read_bufp-p;
          if(datasize<0 || datasize>pb__blockM ||
              read_bufp+datasize>read_bufe) {
            PERRv("block data size too large: %i",datasize)
            fprintf(stderr,"Pointers: %p %p %p\n",
              read__buf,read_bufp,read_bufe);
            END(-26)
            }
          if(l!=datasize)
            ENDEv(-31,"compressed length: %i->%u.",datasize,l)
          // uncompress
          r= pb__decompress(read_bufp,l,zbuf,sizeof(zbuf),&l);
          if(r!=0)
            ENDEv(-32,"decompression failed: %i.",r)
          if(l!=rawsize)
            ENDEv(-33,"uncompressed length: %i->%u.",rawsize,l)
          zdata= 1;
          zbufp= zbuf; zbufe= zbuf+rawsize;
          pb__stre= pb__str;
          read_bufp+= datasize;
      break;
          }  // end   compressed data expected
        if(read_bufp[0]==0)  // possibly a new block start
      break;
        }  // end   read new block
      if(zbufp<zbufe)  // zbuf data available
  continue;
      // here: still no osm objects to read in zbuf[]
      ENDE(-39,"missing data in pbf block.")
      }  // get new zbuf data
    }  // until we have a new object
  end:
  return pb_type;
  }  // end pb_input()

static int pb_keyval(char** keyp,char** valp,int keyvalmax) {
  // read tags of an osm .pbf object;
  // keyp,valp: start addresses of lists in which the tags
  //            will be stored in;
  // keyvalmax: maximum number of tags which can be stored in the list;
  // return: number of key/val tags which have been read;
  // this procedure should be called after OSM object data have
  // been provided by pb_input();
  // repetitive calls are not allowed because they would result
  // in wrong key/val data;
  int key,val,n;

  n= 0;
  if(pb_type==0) {  // node
    if(pb__nodetags<pb__nodetagse &&
        (key= pbf_uint32(&pb__nodetags))!=0) {
        // there are key/val pairs for this node
      do {  // for every key/val pair
        val= pbf_uint32(&pb__nodetags);
        if(key>=pb__strm || val>=pb__strm) {
          PERRv("node key string index overflow: %u,%u>=%u",
            key,val,pb__strm)
return 0;
          }
        if(++n<=keyvalmax) {  // still space in output list
          *keyp++= pb__str[key];
          *valp++= pb__str[val];
          }  // still space in output list
        key= pbf_uint32(&pb__nodetags);
        } while(key!=0);  // end   for every key/val pair
      }  // end   there are key/val pairs for this node
    }  // node
  else if(pb_type==1) {  // way
    while(pb__waykey<pb__waykeye && pb__wayval<pb__wayvale) {
        // there are still key/val pairs for this way
      uint key,val;

      key= pbf_uint32(&pb__waykey);
      val= pbf_uint32(&pb__wayval);
      if(key>=pb__strm || val>=pb__strm) {
        PERRv("way key string index overflow: %u,%u>=%u",
          key,val,pb__strm)
return 0;
        }
      if(++n<=keyvalmax) {  // still space in output list
        *keyp++= pb__str[key];
        *valp++= pb__str[val];
        }  // still space in output list
      }  // end   there are still key/val pairs for this way
    }  // way
  else if(pb_type==2) {  // relation
    while(pb__relkey<pb__relkeye && pb__relval<pb__relvale) {
        // there are still refs for this relation
      uint key,val;

      key= pbf_uint32(&pb__relkey);
      val= pbf_uint32(&pb__relval);
      if(key>=pb__strm || val>=pb__strm) {
        PERRv("rel key string index overflow: %u,%u>=%u",
          key,val,pb__strm)
return 0;
        }
      if(++n<=keyvalmax) {  // still space in output list
        *keyp++= pb__str[key];
        *valp++= pb__str[val];
        }  // still space in output list
      }  // end   there are still refs for this relation
    }  // relation
  if(n>keyvalmax) {
    WARNv("too many key/val pairs for %s: %i>%i",
      ONAME(pb_id),n,keyvalmax)
    n= keyvalmax;
    }
  return n;
  }  // end   pb_keyval()

static int pb_noderef(int64_t* refidp,int refmax) {
  // read node references of an osm .pbf way object;
  // refidp: start addresses of lists in which the node reference's
  //         ids will be stored in;
  // refmax: maximum number of node references which can be stored
  //         in the lists;
  // return: number of node references which have been read;
  // this procedure should be called after OSM way data have
  // been provided by pb_input();
  // repetitive calls are not allowed because they would result
  // in wrong noderef data;
  int64_t noderef;
  int n;

  n= 0;
  noderef= 0;
  while(pb__waynode<pb__waynodee) {
        // there are still noderefs for this way
    noderef+= pbf_sint64(&pb__waynode);
    if(++n<=refmax) {  // still space in output list
      *refidp++= noderef;
      }  // still space in output list
    }  // there are still noderefs for this way
  if(n>refmax) {
    WARNv("too many noderefs for %s: %i>%i",ONAME(pb_id),n,refmax)
    n= refmax;
    }
  return n;
  }  // end   pb_noderef()

static int pb_ref(int64_t* refidp,
    byte* reftypep,char** refrolep,int refmax) {
  // read references of an osm .pbf object;
  // refidp: start addresses of lists in which the reference's
  //         ids will be stored in;
  // reftypep: same for their types;
  // refrolep: same for their roles;
  // refmax: maximum number of references which can be stored
  //         in the lists;
  // return: number of references which have been read;
  // this procedure should be called after OSM relation data have
  // been provided by pb_input();
  // repetitive calls are not allowed because they would result
  // in wrong ref data;
  int64_t refid;
  int n;

  n= 0;
  refid= 0;
  while(pb__relrefid<pb__relrefide && pb__relreftype<pb__relreftypee &&
      pb__relrefrole<pb__relrefrolee) {
      // there are still refs for this relation
    int reftype,refrole;

    refid+= pbf_sint64(&pb__relrefid);
    reftype= pbf_uint32(&pb__relreftype);
    refrole= pbf_uint32(&pb__relrefrole);
    if(refrole>=pb__strm) {
      PERRv("rel refrole string index overflow: %u>=%u",
        refrole,pb__strm)
return 0;
      }
    if(++n<=refmax) {  // still space in output list
      *refidp++= refid;
      *reftypep++= reftype;
      *refrolep++= pb__str[refrole];
      }  // still space in output list
    }  // end   there are still refs for this relation
  if(n>refmax) {
    WARNv("too many relrefs for %s: %i>%i",ONAME(pb_id),n,refmax)
    n= refmax;
    }
  return n;
  }  // end   pb_ref()

//------------------------------------------------------------
// end   Module pb_   pbf read module
//------------------------------------------------------------

  

//------------------------------------------------------------
// Module rr_   relref temporary module
//------------------------------------------------------------

// this module provides procedures to use a temporary file for
// storing relation's references;
// as usual, all identifiers of a module have the same prefix,
// in this case 'rr_'; an underline will follow in case of a
// global accessible object, two underlines in case of objects
// which are not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----

static char rr__filename[400]= "";
static int rr__fd= -1;  // file descriptor for temporary file
#define rr__bufM 400000
static int32_t rr__buf[rr__bufM],*rr__bufp,*rr__bufe,*rr__bufee;
  // buffer - used for write, and later for read;
static bool rr__writemode;  // buffer is used for writing

static inline void rr__flush() {
  if(!rr__writemode || rr__bufp==rr__buf)
return;
  write(rr__fd,rr__buf,(char*)rr__bufp-(char*)rr__buf);
  rr__bufp= rr__buf;
  }  // end   rr__flush()

static inline void rr__write(int32_t i) {
  // write an int to tempfile, use a buffer;
  if(rr__bufp>=rr__bufee) rr__flush();
  *rr__bufp++= i;
  }  // end   rr__write()

static void rr__end() {
  // clean-up for temporary file access;
  // will be called automatically at program end;
  if(rr__fd>2) {
    close(rr__fd);
    rr__fd= -1;
    }
  if(loglevel<2) unlink(rr__filename);
  }  // end   rr__end()

//------------------------------------------------------------

static int rr_ini(const char* filename) {
  // open a temporary file with the given name for random r/w access;
  // return: ==0: ok; !=0: error;
  strcpy(stpmcpy(rr__filename,filename,sizeof(rr__filename)-2),".0");
  if(rr__fd>=0)  // file already open
return 0;  // ignore this call
  unlink(rr__filename);
  rr__fd= open(rr__filename,O_RDWR|O_CREAT|0600|O_TRUNC|O_BINARY);
  if(rr__fd<0) {
    fprintf(stderr,
      "osmconvert Error: could not open temporary file: %.80s\n",
      rr__filename);
return 1;
    }
  #if !__WIN32__
  fchmod(rr__fd,0600);  // (did not work at file creation time)
  #endif
  atexit(rr__end);
  rr__bufee= rr__buf+rr__bufM;
  rr__bufp= rr__bufe= rr__buf;
  rr__writemode= true;
  return 0;
  }  // end   rr_ini()

static inline void rr_rel(int32_t relid) {
  // store the id of a relation in tempfile;
  rr__write(0);
  rr__write(relid);
  } // end   rr_rel()

static inline void rr_ref(int32_t refid) {
  // store the id of an interrelation reference in tempfile;
  rr__write(refid);
  } // end   rr_ref()

static int rr_rewind() {
  // rewind the file pointer;
  // return: ==0: ok; !=0: error;
  if(rr__writemode) {
    rr__flush(); rr__writemode= false; }
  if(lseek(rr__fd,0,SEEK_SET)<0) {
    fprintf(stderr,"osmconvert Error: could not rewind temporary file.\n");
return 1;
    }
  rr__bufp= rr__bufe= rr__buf;
  return 0;
  } // end   rr_rewind()

static inline int rr_read(int32_t* ip) {
  // read one integer; meaning of the values of these integers:
  // every value is an interrelation reference id, with one exception:
  // integers which follow a 0-integer directly are relation ids;
  // note that we take 32-bit-integers instead of the 64-bit-integers
  // we usually take for object ids; this is because the range of
  // relation ids will not exceed the 2^15 range in near future;
  // return: ==0: ok; !=0: eof;
  int r,r2;

  if(rr__bufp>=rr__bufe) {
    r= read(rr__fd,rr__buf,sizeof(rr__buf));
    if(r<=0)
return 1;
    rr__bufe= (int32_t*)((char*)rr__buf+r);
    if((r%4)!=0) { // odd number of bytes
      r2= read(rr__fd,rr__bufe,4-(r%4));  // request the missing bytes
      if(r2<=0)  // did not get the missing bytes
        rr__bufe= (int32_t*)((char*)rr__bufe-(r%4));
      else
        rr__bufe= (int32_t*)((char*)rr__bufe+r2);
      }
    rr__bufp= rr__buf;
    }
  *ip= *rr__bufp++;
  return 0;
  }  // end   rr_read()

//------------------------------------------------------------
// end   Module rr_   relref temporary module
//------------------------------------------------------------
  


//------------------------------------------------------------
// Module o5_   o5m conversion module
//------------------------------------------------------------

// this module provides procedures which convert data to
// o5m format;
// as usual, all identifiers of a module have the same prefix,
// in this case 'o5'; an underline will follow in case of a
// global accessible object, two underlines in case of objects
// which are not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----

static void stw_reset();

#define o5__bufM UINT64_C(5000000)
static byte* o5__buf= NULL;  // buffer for one object in .o5m format
static byte* o5__bufe= NULL;
  // (const) water mark for buffer filled nearly 100%
static byte* o5__bufp= NULL;
static byte* o5__bufr0= NULL,*o5__bufr1= NULL;
  // start end end mark of a reference area in o5__buf[];
  // ==NULL: no mark set;

// basis for delta coding
static int64_t o5_id;
static uint32_t o5_lat,o5_lon;
static int64_t o5_cset;
static int64_t o5_time;
static int64_t o5_ref[3];  // for node, way, relation

static inline void o5__resetvars(void) {
  // reset all delta coding counters;
  o5__bufp= o5__buf;
  o5__bufr0= o5__bufr1= o5__buf;
  o5_id= 0;
  o5_lat= o5_lon= 0;
  o5_cset= 0;
  o5_time= 0;
  o5_ref[0]= o5_ref[1]= o5_ref[2]= 0;
  stw_reset();
  }  // end   o5__resetvars()

static void o5__end() {
  // clean-up for o5 module;
  // will be called at program's end;
  if(o5__buf!=NULL) {
    free(o5__buf); o5__buf= NULL; }
  }  // end   o5__end()

//------------------------------------------------------------

static inline void o5_reset(void) {
  // perform and write an o5m Reset;
  o5__resetvars();
  write_char(0xff);  // write .o5m Reset
  }  // end   o5_reset()

static int o5_ini(void) {
  // initialize this module;
  // must be called before any other procedure is called;
  // return: 0: everything went ok;
  //         !=0: an error occurred;
  static bool firstrun= true;

  if(firstrun) {
    firstrun= false;
    o5__buf= (byte*)malloc(o5__bufM);
    if(o5__buf==NULL)
return 1;
    atexit(o5__end);
    o5__bufe= o5__buf+o5__bufM-400000;
    }
  o5__resetvars();
  return 0;
  }  // end   o5_ini()

static inline void o5_byte(byte b) {
  // write a single byte;
  // writing starts at position o5__bufp;
  // o5__bufp: incremented by 1;
  *o5__bufp++= b;
  }  // end   o5_byte()

static inline int o5_str(const char* s) {
  // write a zero-terminated string;
  // writing starts at position o5__bufp;
  // return: bytes written;
  // o5__bufp: increased by the number of written bytes;
  byte* p0;
  byte c;

  p0= o5__bufp;
  if(o5__bufp>=o5__bufe) {
    static int msgn= 1;
    if(--msgn>=0) {
    fprintf(stderr,"osmconvert Error: .o5m memory overflow.\n");
return 0;
      }
    }
  do *o5__bufp++= c= *s++;
    while(c!=0);
return o5__bufp-p0;
  }  // end   o5_str()

static inline int o5_uvar32buf(byte* p,uint32_t v) {
  // write an unsigned 32 bit integer as Varint into a buffer;
  // writing starts at position p;
  // return: bytes written;
  byte* p0;
  uint32_t frac;

  p0= p;
  frac= v&0x7f;
  if(frac==v) {  // just one byte
    *p++= frac;
return 1;
    }
  do {
    *p++= frac|0x80;
    v>>= 7;
    frac= v&0x7f;
    } while(frac!=v);
  *p++= frac;
return p-p0;
  }  // end   o5_uvar32buf()

static inline int o5_uvar32(uint32_t v) {
  // write an unsigned 32 bit integer as Varint;
  // writing starts at position o5__bufp;
  // return: bytes written;
  // o5__bufp: increased by the number of written bytes;
  byte* p0;
  uint32_t frac;

  if(o5__bufp>=o5__bufe) {
    static int msgn= 1;
    if(--msgn>=0) {
    fprintf(stderr,"osmconvert Error: .o5m memory overflow.\n");
return 0;
      }
    }
  p0= o5__bufp;
  frac= v&0x7f;
  if(frac==v) {  // just one byte
    *o5__bufp++= frac;
return 1;
    }
  do {
    *o5__bufp++= frac|0x80;
    v>>= 7;
    frac= v&0x7f;
    } while(frac!=v);
  *o5__bufp++= frac;
return o5__bufp-p0;
  }  // end   o5_uvar32()

static inline int o5_svar32(int32_t v) {
  // write a signed 32 bit integer as signed Varint;
  // writing starts at position o5__bufp;
  // return: bytes written;
  // o5__bufp: increased by the number of written bytes;
  byte* p0;
  uint32_t u;
  uint32_t frac;

  if(o5__bufp>=o5__bufe) {
    static int msgn= 1;
    if(--msgn>=0) {
    fprintf(stderr,"osmconvert Error: .o5m memory overflow.\n");
return 0;
      }
    }
  p0= o5__bufp;
  if(v<0)
    u= (((uint64_t)(-v))<<1)-1;
  else
    u= v<<1;
  frac= u&0x7f;
  if(frac==u) {  // just one byte
    *o5__bufp++= frac;
return 1;
    }
  do {
    *o5__bufp++= frac|0x80;
    u>>= 7;
    frac= u&0x7f;
    } while(frac!=u);
  *o5__bufp++= frac;
return o5__bufp-p0;
  }  // end   o5_svar32()

static inline int o5_svar64(int64_t v) {
  // write a signed 64 bit integer as signed Varint;
  // writing starts at position o5__bufp;
  // return: bytes written;
  // o5__bufp: increased by the number of written bytes;
  byte* p0;
  uint64_t u;
  uint32_t frac;

  if(o5__bufp>=o5__bufe) {
    static int msgn= 1;
    if(--msgn>=0) {
    fprintf(stderr,"osmconvert Error: .o5m memory overflow.\n");
return 0;
      }
    }
  p0= o5__bufp;
  if(v<0)
    u= (((uint64_t)(-v))<<1)-1;
  else
    u= v<<1;
  frac= u&0x7f;
  if(frac==u) {  // just one byte
    *o5__bufp++= frac;
return 1;
    }
  do {
    *o5__bufp++= frac|0x80;
    u>>= 7;
    frac= u&0x7f;
    } while(frac!=u);
  *o5__bufp++= frac;
return o5__bufp-p0;
  }  // end   o5_svar64()

static inline void o5_markref(int pos) {
  // mark reference area;
  // pos: ==0: start; ==1: end;
  //      0 is accepted only once per dataset; only the first
  //      request is valid;
  //      1 may be repeated, the last one counts;
  if(pos==0) {
    if(o5__bufr0==o5__buf) o5__bufr0= o5__bufp;
    }
  else
    o5__bufr1= o5__bufp;
  }  // end   o5_markref()

static inline void o5_type(int type) {
  // mark object type we are going to process now;
  // should be called every time a new object is started to be
  // written into o5_buf[];
  // type: object type; 0: node; 1: way; 2: relation;
  //       if object type hase changed, a 0xff byte ("reset")
  //       will be written;
  static int oldtype= -1;

  // process changes of object type
  if(type!=oldtype) {  // object type has changed
    oldtype= type;
    o5_reset();
    }
  oldtype= type;
  }  // end   o5_type()

static void o5_write() {
  // write o5__buf[] contents to standard output;
  // include object length information after byte 0 and include
  // ref area length information right before o5__bufr0 (if !=NULL);
  // if buffer is empty, this procedure does nothing;
  byte buftemp[30];
  int reflen;  // reference area length
  int len;  // object length

  // get some length information
  len= o5__bufp-o5__buf;
  if(len<=0) goto o5_write_end;
  reflen= 0;  // (default)
  if(o5__bufr1<o5__bufr0) o5__bufr1= o5__bufr0;
  if(o5__bufr0>o5__buf) {
      // reference area contains at least 1 byte
    reflen= o5__bufr1-o5__bufr0;
    len+= o5_uvar32buf(buftemp,reflen);
    }  // end   reference area contains at least 1 byte

  // write header
  if(--len>=0) {
    write_char(o5__buf[0]);
    write_mem(buftemp,o5_uvar32buf(buftemp,len));
    }

  // write body
  if(o5__bufr0==o5__buf)  // no reference area
    write_mem(o5__buf+1,o5__bufp-(o5__buf+1));
  else {  // valid reference area
    write_mem(o5__buf+1,o5__bufr0-(o5__buf+1));
    write_mem(buftemp,o5_uvar32buf(buftemp,reflen));
    write_mem(o5__bufr0,o5__bufp-o5__bufr0);
    }  // end   valid reference area

  // reset buffer pointer
  o5_write_end:
  o5__bufp= o5__buf;  // set original buffer pointer to buffer start
  o5__bufr0= o5__bufr1= o5__buf;  // clear reference area marks
  }  // end   o5_write()

//------------------------------------------------------------
// end   Module o5_   o5m conversion module
//------------------------------------------------------------



//------------------------------------------------------------
// Module stw_   string write module
//------------------------------------------------------------

// this module provides procedures for conversions from
// c formatted strings into referenced string data stream objects
// - and writing it to buffered standard output;
// as usual, all identifiers of a module have the same prefix,
// in this case 'stw'; an underline will follow in case of a
// global accessible object, two underlines in case of objects
// which are not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----

#define stw__tabM 15000
#define stw__tabstrM 250  // must be < row size of stw__rab[]
#define stw__hashtabM 150001  // (preferably a prime number)
static char stw__tab[stw__tabM][256];
  // string table; see o5m documentation;
  // row length must be at least stw__tabstrM+2;
  // each row contains a double string; each of the two strings
  // is terminated by a zero byte, the lengths must not exceed
  // stw__tabstrM bytes in total;
static int stw__tabi= 0;
  // index of last entered element in string table
static int stw__hashtab[stw__hashtabM];
  // has table; elements point to matching strings in stw__tab[];
  // -1: no matching element;
static int stw__tabprev[stw__tabM],stw__tabnext[stw__tabM];
  // for to chaining of string table rows which match
  // the same hash value; matching rows are chained in a loop;
  // if there is only one row matching, it will point to itself;
static int stw__tabhash[stw__tabM];
  // has value of this element as a link back to the hash table;
  // a -1 element indicates that the string table entry is not used;

static inline int stw__hash(const char* s1,const char* s2) {
  // get hash value of a string pair;
  // s2: ==NULL: single string; this is treated as s2=="";
  // return: hash value in the range 0..(stw__hashtabM-1);
  // -1: the strings are longer than stw__tabstrM characters in total;
  uint32_t h;
  uint32_t c;
  int len;

  len= stw__tabstrM;
  h= 0;
  for(;;) {
    if((c= *s1++)==0 || --len<0) break; h+= c; 
    if((c= *s1++)==0 || --len<0) break; h+= c<<8;
    if((c= *s1++)==0 || --len<0) break; h+= c<<16;
    if((c= *s1++)==0 || --len<0) break; h+= c<<24;
    }
  if(s2!=NULL) for(;;) {
    if((c= *s2++)==0 || --len<0) break; h+= c; 
    if((c= *s2++)==0 || --len<0) break; h+= c<<8;
    if((c= *s2++)==0 || --len<0) break; h+= c<<16;
    if((c= *s2++)==0 || --len<0) break; h+= c<<24;
    }
  if(len<0)
return -1;
  h%= stw__hashtabM;
  return h;
  }  // end   stw_hash()

static inline int stw__getref(int stri,const char* s1,const char* s2) {
  // get the string reference of a string pair;
  // the strings must not have more than 250 characters in total
  // (252 including terminators), there is no check in this procedure;
  // stri: presumed index in string table (we got it from hash table);
  //       must be >=0 and <stw__tabM, there is no boundary check;
  // s2: ==NULL: it's not a string pair but a single string;
  // stw__hashnext[stri]: chain to further occurrences;
  // return: reference of the string;
  //         ==-1: this string is not stored yet
  int strie;  // index of last occurrence
  const char* sp,*tp;
  int ref;

  if(s2==NULL) s2="";
  strie= stri;
  do {
    // compare the string (pair) with the tab entry
    tp= stw__tab[stri];
    sp= s1;
    while(*tp==*sp && *tp!=0) { tp++; sp++; }
    if(*tp==0 && *sp==0) {
        // first string identical to first string in table
      tp++;  // jump over first string terminator
      sp= s2;
      while(*tp==*sp && *tp!=0) { tp++; sp++; }
      if(*tp==0 && *sp==0) {
          // second string identical to second string in table
        ref= stw__tabi-stri;
        if(ref<=0) ref+= stw__tabM;
return ref;
        }
      }  // end   first string identical to first string in table
    stri= stw__tabnext[stri];
    } while(stri!=strie);
  return -1;
  }  // end   stw__getref()

//------------------------------------------------------------

static inline void stw_reset() {
  // clear string table and string hash table;
  // must be called before any other procedure of this module
  // and may be called every time the string processing shall
  // be restarted;
  int i;

  stw__tabi= 0;
  i= stw__tabM;
  while(--i>=0) stw__tabhash[i]= -1;
  i= stw__hashtabM;
  while(--i>=0) stw__hashtab[i]= -1;
  }  // end   stw_reset()

static void stw_write(const char* s1,const char* s2) {
  // write a string (pair), e.g. key/val, to o5m buffer;
  // if available, write a string reference instead of writing the
  // string pair directly;
  // no reference is used if the strings are longer than
  // 250 characters in total (252 including terminators);
  // s2: ==NULL: it's not a string pair but a single string;
  int h;  // hash value
  int ref;

  /* try to find a matching string (pair) in string table */ {
    int i;  // index in stw__tab[]

    ref= -1;  // ref invalid (default)
    h= stw__hash(s1,s2);
    if(h>=0) {  // string (pair) short enough for the string table
      i= stw__hashtab[h];
      if(i>=0)  // string (pair) presumably stored already
        ref= stw__getref(i,s1,s2);
      }  // end   string (pair) short enough for the string table
    if(ref>=0) {  // we found the string (pair) in the table
      o5_uvar32(ref);  // write just the reference
return;
      }  // end   we found the string (pair) in the table
    else {  // we did not find the string (pair) in the table
      // write string data
      o5_byte(0); o5_str(s1);
      if(s2!=NULL) o5_str(s2);  // string pair, not a single string
      if(h<0)  // string (pair) too long,
          // cannot be stored in string table
return;
      }  // end   we did not find the string (pair) in the table
    }  // end   try to find a matching string (pair) in string table
  // here: there is no matching string (pair) in the table

  /* free new element - if still being used */ {
    int h0;  // hash value of old element

    h0= stw__tabhash[stw__tabi];
    if(h0>=0) {  // new element in string table is still being used
      // delete old element
      if(stw__tabnext[stw__tabi]==stw__tabi)
          // self-chain, i.e., only this element
        stw__hashtab[h0]= -1;  // invalidate link in hash table
      else {  // one or more other elements in chain
        stw__hashtab[h0]= stw__tabnext[stw__tabi];  // just to ensure
          // that hash entry does not point to deleted element
        // now unchain deleted element
        stw__tabprev[stw__tabnext[stw__tabi]]= stw__tabprev[stw__tabi];
        stw__tabnext[stw__tabprev[stw__tabi]]= stw__tabnext[stw__tabi];
        }  // end   one or more other elements in chain
      }  // end   next element in string table is still being used
    }  // end   free new element - if still being used

  /* enter new string table element data */ {
    char* sp;
    int i;

    sp= stpcpy(stw__tab[stw__tabi],s1)+1;
      // write first string into string table
    if(s2==NULL)  // single string
      *sp= 0;  // second string must be written as empty string
        // into string table
    else
      stpcpy(sp,s2);  // write second string into string table
    i= stw__hashtab[h];
    if(i<0)  // no reference in hash table until now
      stw__tabprev[stw__tabi]= stw__tabnext[stw__tabi]= stw__tabi;
        // self-link the new element;
    else {  // there is already a reference in hash table
      // in-chain the new element
      stw__tabnext[stw__tabi]= i;
      stw__tabprev[stw__tabi]= stw__tabprev[i];
      stw__tabnext[stw__tabprev[stw__tabi]]= stw__tabi;
      stw__tabprev[i]= stw__tabi;
      }
    stw__hashtab[h]= stw__tabi;  // link the new element to hash table
    stw__tabhash[stw__tabi]= h;  // backlink to hash table element
    // new element now in use; set index to oldest element
    if(++stw__tabi>=stw__tabM) {  // index overflow
      stw__tabi= 0;  // restart index
      if(loglevel>=2) {
        static int rs= 0;
        fprintf(stderr,
          "osmconvert: string table index restart %i\n",++rs);
        }
      }  // end   index overflow
    }  // end   enter new string table element data
  }  // end   stw_write()

//------------------------------------------------------------
// end   Module stw_   string write module
//------------------------------------------------------------



//------------------------------------------------------------
// Module str_   string read module
//------------------------------------------------------------

// this module provides procedures for conversions from
// strings which have been stored in data stream objects to
// c-formatted strings;
// as usual, all identifiers of a module have the same prefix,
// in this case 'str'; an underline will follow in case of a
// global accessible object, two underlines in case of objects
// which are not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----

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
str_info_t* str__infop= NULL;

static void str__end() {
  // clean-up this module;
  str_info_t* p;

  while(str__infop!=NULL) {
    p= str__infop->prev;
    free(str__infop);
    str__infop= p;
    }
  }  // end str__end()

//------------------------------------------------------------

static str_info_t* str_open() {
  // open an new string client unit;
  // this will allow us to process multiple o5m input files;
  // return: handle of the new unit;
  //         ==NULL: error;
  // you do not need to care about closing the unit(s);
  static bool firstrun= true;
  str_info_t* prev;

  prev= str__infop;
  str__infop= (str_info_t*)malloc(sizeof(str_info_t));
  if(str__infop==NULL) {
    fprintf(stderr,"osmconvert Error: "
      "could not get memory for string buffer.\n");
return NULL;
    }
  str__infop->tabi= 0;
  str__infop->tabn= 0;
  str__infop->prev= prev;
  if(firstrun) {
    firstrun= false;
    atexit(str__end);
    }
  return str__infop;
  }  // end   str_open()

static inline void str_switch(str_info_t* sh) {
  // switch to another string unit
  // sh: string unit handle;
  str__infop= sh;
  }  // end str_switch()

static inline void str_reset() {
  // clear string table;
  // must be called before any other procedure of this module
  // and may be called every time the string processing shall
  // be restarted;
  str__infop->tabi= str__infop->tabn= 0;
  }  // end   str_reset()

static void str_read(byte** pp, string & s1p, string & s2p) {
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
  char* p;
  int len1,len2;
  int ref;

  p= (char*)*pp;
  if(*p==0) {  // string (pair) given directly
    s1p= ++p;
    len1= strlen(p);
    p+= len1+1;
    if(s2p == "") {  // single string
      //p= strchr(p,0)+1;  // jump over second string (if any)
      if(len1<=str__tabstrM) {
          // single string short enough for string table
        stpcpy(str__infop->tab[str__infop->tabi],s1p.c_str())[1]= 0;
          // add a second terminator, just in case someone will try
          // to read this single string as a string pair later;
        if(++str__infop->tabi>=str__tabM) str__infop->tabi= 0;
        if(str__infop->tabn<str__tabM) str__infop->tabn++;
        }  // end   single string short enough for string table
      }  // end   single string
    else {  // string pair
      s2p= p;
      len2= strlen(p);
      p+= len2+1;
      if(len1+len2<=str__tabstrM) {
          // string pair short enough for string table
        memcpy(str__infop->tab[str__infop->tabi],s1p.c_str(),len1+len2+2);
        if(++str__infop->tabi>=str__tabM) str__infop->tabi= 0;
        if(str__infop->tabn<str__tabM) str__infop->tabn++;
        }  // end   string pair short enough for string table
      }  // end   string pair
    *pp= (byte*)p;
    }  // end   string (pair) given directly
  else {  // string (pair) given by reference
    ref= pbf_uint32(pp);
    if(ref>str__infop->tabn) {  // string reference invalid
      WARNv("invalid .o5m string reference: %i->%i\n",
        str__infop->tabn,ref)
      s1p= "(invalid)";
      if(s2p!="")  // caller wants a string pair
        s2p= "(invalid)";
      }  // end   string reference invalid
    else {  // string reference valid
      ref= str__infop->tabi-ref;
      if(ref<0) ref+= str__tabM;
      s1p= str__infop->tab[ref];
      if(s2p!="")  // caller wants a string pair
        s2p= strchr(str__infop->tab[ref],0)+1;
      }  // end   string reference valid
    }  // end   string (pair) given by reference

}


static void str_read(byte** pp,char** s1p,char** s2p) {
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
  char* p;
  int len1,len2;
  int ref;

  p= (char*)*pp;
  if(*p==0) {  // string (pair) given directly
    *s1p= ++p;
    len1= strlen(p);
    p+= len1+1;
    if(s2p==NULL) {  // single string
      //p= strchr(p,0)+1;  // jump over second string (if any)
      if(len1<=str__tabstrM) {
          // single string short enough for string table
        stpcpy(str__infop->tab[str__infop->tabi],*s1p)[1]= 0;
          // add a second terminator, just in case someone will try
          // to read this single string as a string pair later;
        if(++str__infop->tabi>=str__tabM) str__infop->tabi= 0;
        if(str__infop->tabn<str__tabM) str__infop->tabn++;
        }  // end   single string short enough for string table
      }  // end   single string
    else {  // string pair
      *s2p= p;
      len2= strlen(p);
      p+= len2+1;
      if(len1+len2<=str__tabstrM) {
          // string pair short enough for string table
        memcpy(str__infop->tab[str__infop->tabi],*s1p,len1+len2+2);
        if(++str__infop->tabi>=str__tabM) str__infop->tabi= 0;
        if(str__infop->tabn<str__tabM) str__infop->tabn++;
        }  // end   string pair short enough for string table
      }  // end   string pair
    *pp= (byte*)p;
    }  // end   string (pair) given directly
  else {  // string (pair) given by reference
    ref= pbf_uint32(pp);
    if(ref>str__infop->tabn) {  // string reference invalid
      WARNv("invalid .o5m string reference: %i->%i\n",
        str__infop->tabn,ref)
      *s1p= (char *)"(invalid)";
      if(s2p!=NULL)  // caller wants a string pair
        *s2p= (char *)"(invalid)";
      }  // end   string reference invalid
    else {  // string reference valid
      ref= str__infop->tabi-ref;
      if(ref<0) ref+= str__tabM;
      *s1p= str__infop->tab[ref];
      if(s2p!=NULL)  // caller wants a string pair
        *s2p= strchr(str__infop->tab[ref],0)+1;
      }  // end   string reference valid
    }  // end   string (pair) given by reference
  }  // end   str_read()

//------------------------------------------------------------
// end   Module str_   string read module
//------------------------------------------------------------



//------------------------------------------------------------
// Module wo_   write osm module
//------------------------------------------------------------

// this module provides procedures which write osm objects;
// it uses procedures from module o5_;
// as usual, all identifiers of a module have the same prefix,
// in this case 'wo'; an underline will follow in case of a
// global accessible object, two underlines in case of objects
// which are not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----

static int wo__format= 0;  // output format;
  // 0: o5m; 11: native XML; 12: pbf2osm; 13: Osmosis;
static char* wo__xmlclosetag= NULL;  // close tag for XML output;
static bool wo__xmlshorttag= false;
  // write the short tag ("/>") instead of the long tag;
#define wo__CLOSE {  /* close the newest written object; */ \
  if(wo__xmlclosetag!=NULL) { if(wo__xmlshorttag) write_str("\"/>"NL); \
    else write_str(wo__xmlclosetag); \
    wo__xmlclosetag= NULL; wo__xmlshorttag= false; } }
#define wo__CONTINUE {  /* continue an XML object */ \
  if(wo__xmlshorttag) { write_str("\">"NL); wo__xmlshorttag= false; \
      /* from now on: long close tag necessary; */ } }

static inline void wo__history(int32_t hisver,int64_t histime,
    int64_t hiscset,uint32_t hisuid,const char* hisuser) {
  // write osm object history;
  // hisver: version; 0: no history is to be written
  //                     (necessary if o5m format);
  // histime: time (seconds since 1970)
  // hiscset: changeset
  // hisuid: uid
  // hisuser: user name
  // note that when writing o5m format, this procedure needs to be
  // called even if there is no history to be written;
  if(wo__format==0) {  // o5m
    if(hisver==0)  // no history information
      o5_byte(0x00);  // write indicator that there is
        // no history information which would follow
    else {  // history information available
      o5_uvar32(hisver);
      o5_svar64(histime-o5_time); o5_time= histime;
      o5_svar64(hiscset-o5_cset); o5_cset= hiscset;
      if(hisuid==0 || hisuser==NULL || hisuser[0]==0)
          // user identification not available
        stw_write("","");
      else {  // user identification available
        byte uidbuf[30];

        uidbuf[o5_uvar32buf(uidbuf,hisuid)]= 0;
        stw_write((const char*)uidbuf,hisuser);
        }  // end   user identification available
      }  // end   history information available
return;
    }  // end   o5m
  // here: XML format
  if(hisver==0)  // no history information
return;
  switch(wo__format) {  // depending on output format
  case 11:  // native XML
    write_str("\" version=\""); write_uint32(hisver);
    write_str("\" timestamp=\""); write_timestamp(histime);
    write_str("\" changeset=\""); write_uint64(hiscset);
    if(hisuid!=0 && hisuser[0]!=0) {  // user information available
      write_str("\" uid=\""); write_uint32(hisuid);
      write_str("\" user=\""); write_xmlstr(hisuser);
      }
    break;
  case 12:  // pbf2osm XML
    write_str("\" version=\""); write_uint32(hisver);
    write_str("\" changeset=\""); write_uint64(hiscset);
    if(hisuid!=0 && hisuser[0]!=0) {  // user information available
      write_str("\" user=\""); write_xmlstr(hisuser);
      write_str("\" uid=\""); write_uint32(hisuid);
      }
    write_str("\" timestamp=\""); write_timestamp(histime);
    break;
  case 13:  // Osmosis XML
    write_str("\" version=\""); write_uint32(hisver);
    write_str("\" timestamp=\""); write_timestamp(histime);
    if(hisuid!=0 && hisuser[0]!=0) {  // user information available
      write_str("\" uid=\""); write_uint32(hisuid);
      write_str("\" user=\""); write_xmlmnstr(hisuser);
      }
    write_str("\" changeset=\""); write_uint64(hiscset);
    break;
    }  // end   depending on output format
  }  // end   wo__history()

//------------------------------------------------------------

static void wo_start(int format,bool bboxvalid,
    int32_t x1,int32_t y1,int32_t x2,int32_t y2) {
  // start writing osm objects;
  // format: 0: o5m; 11: native XML; 12: pbf2osm; 13: Osmosis;
  // bboxvalid: the following bbox coordinates are valid;
  // x1,y1,x2,y2: bbox coordinates (base 10^-7);
  if(format<0 || (format >0 && format<11) || format>13) format= 0;
  wo__format= format;

  if(wo__format==0) {  // o5m
    static const byte o5mfileheader[]= {0xff,0xe0,0x03,'o','5','m'};
    write_mem(o5mfileheader,sizeof(o5mfileheader));
return;
    }  // end   o5m
  // here: XML
  write_str("<?xml version=\'1.0\' encoding=\'UTF-8\'?>"NL);
  switch(wo__format) {  // depending on output format
  case 11:  // native XML
    if(global_outosc)
      write_str("<osmChange version=\"0.6\" generator=\"osmconvert "
        VERSION"\">"NL);
    else
      write_str("<osm version=\"0.6\" generator=\"osmconvert "
        VERSION"\">"NL);
    break;
  case 12:  // pbf2osm XML
    if(global_outosc)
      write_str("<osm version=\"0.6\" generator=\"pbf2osm\">"NL);
    else
      write_str("<osm version=\"0.6\" generator=\"pbf2osm\">"NL);
    break;
  case 13:  // Osmosis XML
    if(global_outosc)
      write_str("<osm version=\"0.6\" generator=\"Osmosis 0.39\">"NL);
    else
      write_str("<osm version=\"0.6\" generator=\"Osmosis 0.39\">"NL);
    break;
    }  // end   depending on output format
  if(wo__format!=12) {  // bbox may be written
    if(border_active)  // borders are to be applied
      border_querybox(&x1,&y1,&x2,&y2);
    if(border_active || bboxvalid) {  // borders are to be applied OR
        // bbox has been supplied
      write_str("\t<bounds minlat=\""); write_sfix7(y1);
      write_str("\" minlon=\""); write_sfix7(x1);
      write_str("\" maxlat=\""); write_sfix7(y2);
      write_str("\" maxlon=\""); write_sfix7(x2);
      write_str("\"/>"NL);
      }
    }  // end   bbox may be written
  }  // end   wo_start()

static void wo_end() {
  // end writing osm objects;
  switch(wo__format) {  // depending on output format
  case 0:  // o5m
    o5_write();  // write last object - if any
    write_char(0xfe);  // write o5m eof indicator
    break;
  case 11:  // native XML
  case 12:  // pbf2osm XML
  case 13:  // Osmosis XML
    wo__CLOSE
    write_str("</osm>"NL);
    if(wo__format>=12)
      write_str("<!--End of emulated output.-->"NL);
    break;
    }  // end   depending on output format
  }  // end   wo_end()

static inline void wo_flush() {
  // write temporarily stored object information;
  switch(wo__format) {  // depending on output format
  case 0:  // o5m
    o5_write();  // write last object - if any
    break;
  case 11:  // native XML
  case 12:  // pbf2osm XML
  case 13:  // Osmosis XML
    wo__CLOSE
    }  // end   depending on output format
  write_flush();
  }  // end   wo_flush()

static int wo_format(int format) {
  // get or change output format;
  // format: -9: return the currently used format, do not change it;
  if(format==-9)  // do not change the format
return wo__format;
  wo_flush();
  if(format<0 || (format >0 && format<11) || format>13) format= 0;
  wo__format= format;
  return wo__format;
  }  // end   wo_format()

static inline void wo_reset() {
  // in case of o5m format, write a Reset;
  // note that this is done automatically at every change of
  // object type; this procedure offers to write additional Resets
  // at every time you want;
  if(wo__format==0)
    o5_reset();
  }  // end   wo_reset()

static inline void wo_node(int64_t id,int32_t lon,int32_t lat,
    int32_t hisver,int64_t histime,int64_t hiscset,
    uint32_t hisuid,const char* hisuser) {
  // write osm node body;
  // id: id of this object;
  // lon: latitude in 100 nanodegree;
  // lat: latitude in 100 nanodegree;
  // hisver: version; 0: no history is to be written
  //                     (necessary if o5m format);
  // histime: time (seconds since 1970)
  // hiscset: changeset
  // hisuid: uid
  // hisuser: user name
  if(wo__format==0) {  // o5m
    o5_write();  // write last object - if any
    o5_type(0);
    o5_byte(0x10);  // data set id for node
    o5_svar64(id-o5_id); o5_id= id;
    o5_svar32(lon-o5_lon); o5_lon= lon;
    o5_svar32(lat-o5_lat); o5_lat= lat;
    wo__history(hisver,histime,hiscset,hisuid,hisuser);
return;
    }  // end   o5m
  wo__CLOSE
  switch(wo__format) {  // depending on output format
  case 11:  // native XML
    write_str("\t<node id=\""); write_sint64(id);
    write_str("\" lat=\""); write_sfix7(lat);
    write_str("\" lon=\""); write_sfix7(lon);
    wo__history(hisver,histime,hiscset,hisuid,hisuser);
    wo__xmlclosetag= (char *)"\t</node>"NL;  // preset close tag
    break;
  case 12:  // pbf2osm XML
    write_str("\t<node id=\""); write_sint64(id);
    write_str("\" lat=\""); write_sfix7o(lat);
    write_str("\" lon=\""); write_sfix7o(lon);
    wo__history(hisver,histime,hiscset,hisuid,hisuser);
    wo__xmlclosetag= (char *)"\t</node>"NL;  // preset close tag
    break;
  case 13:  // Osmosis XML
    write_str("  <node id=\""); write_sint64(id);
    wo__history(hisver,histime,hiscset,hisuid,hisuser);
    write_str("\" lat=\""); write_sfix7(lat);
    write_str("\" lon=\""); write_sfix7(lon);
    wo__xmlclosetag= (char *)"  </node>"NL;  // preset close tag
    break;
    }  // end   depending on output format
  wo__xmlshorttag= true;  // (default)
  }  // end   wo_node()

static inline void wo_way(int64_t id,
    int32_t hisver,int64_t histime,int64_t hiscset,
    uint32_t hisuid,const char* hisuser) {
  // write osm way body;
  // id: id of this object;
  // hisver: version; 0: no history is to be written
  //                     (necessary if o5m format);
  // histime: time (seconds since 1970)
  // hiscset: changeset
  // hisuid: uid
  // hisuser: user name
  if(wo__format==0) {  // o5m
    o5_write();  // write last object - if any
    o5_type(1);
    o5_byte(0x11);  // data set id for node
    o5_svar64(id-o5_id); o5_id= id;
    wo__history(hisver,histime,hiscset,hisuid,hisuser);
    o5_markref(0);
return;
    }  // end   o5m
  wo__CLOSE
  switch(wo__format) {  // depending on output format
  case 11:  // native XML
    write_str("\t<way id=\""); write_sint64(id);
    wo__history(hisver,histime,hiscset,hisuid,hisuser);
    wo__xmlclosetag= (char *)"\t</way>"NL;  // preset close tag
    break;
  case 12:  // pbf2osm XML
    write_str("\t<way id=\""); write_sint64(id);
    wo__history(hisver,histime,hiscset,hisuid,hisuser);
    wo__xmlclosetag= (char *)"\t</way>"NL;  // preset close tag
    break;
  case 13:  // Osmosis XML
    write_str("  <way id=\""); write_sint64(id);
    wo__history(hisver,histime,hiscset,hisuid,hisuser);
    wo__xmlclosetag= (char *)"  </way>"NL;  // preset close tag
    break;
    }  // end   depending on output format
  wo__xmlshorttag= true;  // (default)
  }  // end   wo_way()

static inline void wo_relation(int64_t id,
    int32_t hisver,int64_t histime,int64_t hiscset,
    uint32_t hisuid,const char* hisuser) {
  // write osm relation body;
  // id: id of this object;
  // hisver: version; 0: no history is to be written
  //                     (necessary if o5m format);
  // histime: time (seconds since 1970)
  // hiscset: changeset
  // hisuid: uid
  // hisuser: user name
  if(wo__format==0) {  // o5m
    o5_write();  // write last object - if any
    o5_type(2);
    o5_byte(0x12);  // data set id for node
    o5_svar64(id-o5_id); o5_id= id;
    wo__history(hisver,histime,hiscset,hisuid,hisuser);
    o5_markref(0);
return;
    }  // end   o5m
  wo__CLOSE
  switch(wo__format) {  // depending on output format
  case 11:  // native XML
    write_str("\t<relation id=\""); write_sint64(id);
    wo__history(hisver,histime,hiscset,hisuid,hisuser);
    wo__xmlclosetag= (char *)"\t</relation>"NL;  // preset close tag
    break;
  case 12:  // pbf2osm XML
    write_str("\t<relation id=\""); write_sint64(id);
    wo__history(hisver,histime,hiscset,hisuid,hisuser);
    wo__xmlclosetag= (char *)"\t</relation>"NL;  // preset close tag
    break;
  case 13:  // Osmosis XML
    write_str("  <relation id=\""); write_sint64(id);
    wo__history(hisver,histime,hiscset,hisuid,hisuser);
    wo__xmlclosetag=(char *) "  </relation>"NL;  // preset close tag
    break;
    }  // end   depending on output format
  wo__xmlshorttag= true;  // (default)
  }  // end   wo_relation()

static void wo_delete(int otype,int64_t id) {
  // write osm delete request;
  // this is possible for o5m format only;
  // for any other output format, this procedure does nothing;
  // otype: 0: node; 1: way; 2: relation;
  if(otype<0 || otype>2)
return;
  if(wo__format==0) {  // o5m (.o5c)
    o5_write();  // write last object - if any
    o5_type(otype);
    o5_byte(0x10+otype);  // data set id
    o5_svar64(id-o5_id); o5_id= id;
    }  // end   o5m (.o5c)
  else {  // .osm (.osc)
    wo__CLOSE
    switch(otype) {
    case 0: // node
      write_str("<delete>"NL"\t<node id=\""); write_sint64(id);
      wo__xmlclosetag= (char *)"\"/>"NL"</delete>"NL;  // preset close tag
      break;
    case 1: // way
      write_str("<delete>"NL"\t<way id=\""); write_sint64(id);
      wo__xmlclosetag= (char *)"\"/>"NL"</delete>"NL;  // preset close tag
      break;
    case 2:  // relation
      write_str("<delete>"NL"\t<relation id=\""); write_sint64(id);
      wo__xmlclosetag= (char *)"\"/>"NL"</delete>"NL;
        // preset close tag
      }
    wo__xmlshorttag= false;  // (default)
    wo__CLOSE  // write close tag
    }  // end   .osm (.osc)
  }  // end   wo_delete()

static inline void wo_noderef(int64_t noderef) {
  // write osm object node reference;
  if(wo__format==0) {  // o5m
    o5_svar64(noderef-o5_ref[0]); o5_ref[0]= noderef;
    o5_markref(1);
return;
    }  // end   o5m
  // here: XML format
  wo__CONTINUE
  switch(wo__format) {  // depending on output format
  case 11:  // native XML
  case 12:  // pbf2osm XML
    write_str("\t\t<nd ref=\""); write_sint64(noderef);
    write_str("\"/>"NL);
    break;
  case 13:  // Osmosis XML
    write_str("    <nd ref=\""); write_sint64(noderef);
    write_str("\"/>"NL);
    break;
    }  // end   depending on output format
  }  // end   wo_noderef()

static inline void wo_ref(int64_t refid,int reftype,
    const char* refrole) {
  // write osm object reference;
  if(wo__format==0) {  // o5m
    char o5typerole[4000];

    o5_svar64(refid-o5_ref[reftype]); o5_ref[reftype]= refid;
    o5typerole[0]= reftype+'0';
    strmcpy(o5typerole+1,refrole,sizeof(o5typerole)-1);
    stw_write(o5typerole,NULL);
    o5_markref(1);
return;
    }  // end   o5m
  // here: XML format
  wo__CONTINUE
  switch(wo__format) {  // depending on output format
  case 11:  // native XML
  case 12:  // pbf2osm XML
    if(reftype==0)
      write_str("\t\t<member type=\"node\" ref=\"");
    else if(reftype==1)
      write_str("\t\t<member type=\"way\" ref=\"");
    else
      write_str("\t\t<member type=\"relation\" ref=\"");
    write_sint64(refid);
    write_str("\" role=\""); write_xmlstr(refrole);
    write_str("\"/>"NL);
    break;
  case 13:  // Osmosis XML
    if(reftype==0)
      write_str("    <member type=\"node\" ref=\"");
    else if(reftype==1)
      write_str("    <member type=\"way\" ref=\"");
    else
      write_str("    <member type=\"relation\" ref=\"");
    write_sint64(refid);
    write_str("\" role=\""); write_xmlmnstr(refrole);
    write_str("\"/>"NL);
    break;
    }  // end   depending on output format
  }  // end   wo_ref()

static inline void wo_keyval(const char* key,const char* val) {
  // write osm object's keyval;
  if(wo__format==0) {  // o5m
    stw_write(key,val);
return;
    }  // end   o5m
  // here: XML format
  wo__CONTINUE
  switch(wo__format) {  // depending on output format
  case 11:  // native XML
    write_str("\t\t<tag k=\""); write_xmlstr(key);
    write_str("\" v=\""); write_xmlstr(val);
    write_str("\"/>"NL);
    break;
  case 12:  // pbf2osm XML
    write_str("\t\t<tag k=\""); write_xmlstr(key);
    write_str("\" v=\""); write_xmlstr(val);
    write_str("\" />"NL);
    break;
  case 13:  // Osmosis XML
    write_str("    <tag k=\""); write_xmlmnstr(key);
    write_str("\" v=\""); write_xmlmnstr(val);
    write_str("\"/>"NL);
    break;
    }  // end   depending on output format
  }  // end   wo_keyval()

//------------------------------------------------------------
// end   Module wo_   write osm module
//------------------------------------------------------------



//------------------------------------------------------------
// Module oo_   osm to osm module
//------------------------------------------------------------

// this module provides procedures which read osm objects,
// process them and write them as osm objects, using module wo_;
// that goes for .osm format as well as for .o5m format;
// as usual, all identifiers of a module have the same prefix,
// in this case 'oo'; an underline will follow in case of a
// global accessible object, two underlines in case of objects
// which are not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----

static void oo__rrprocessing(int* maxrewindp) {
  // process temporary relation reference file;
  // the file must have been written; this procedure processes
  // the interrelation references of this file and updates
  // the hash table of module hash_ accordingly;
  // maxrewind: maximum number of rewinds;
  // return:
  // maxrewind: <0: maximum number of rewinds was not sufficient;
  int changed;
    // number of relations whose flag has been changed, i.e.,
    // the recursive processing will continue;
    // if none of the relations' flags has been changed,
    // this procedure will end;
  int h;
  int32_t relid;  // relation id;
  int32_t refid;  // interrelation reference id;
  bool flag;

  h= 0;
  relid= 0; flag= false;
  while(*maxrewindp>=0) {  // for every recursion
    changed= 0;
    if(rr_rewind())  // could not rewind
  break;
    for(;;) {  // for every reference
      for(;;) {  // get next id
        if(rr_read(&refid))
          goto rewind;  // if at file end, rewind
        if(refid!=0)
      break;
        // here: a relation id will follow
        rr_read(&relid);  // get the relation id
        flag= hash_geti(2,relid);  // get the related flag
        }  // end   get next id
      if(flag)  // flag already set
    continue;  // go on until next relation
      if(!hash_geti(2,refid))  // flag of reference is not set
    continue;  // go on and examine next reference of this relation
      hash_seti(2,relid);  // set flag of this relation
      flag= true;
      changed++;  // memorize that we changed a flag
      }  // end   for every reference
    rewind:
    if(loglevel>0) fprintf(stderr,
      "Interrelational hierarchy %i: %i dependencies.\n",++h,changed);
    if(changed==0)  // no changes have been made in last recursion
  break;  // end the processing
    (*maxrewindp)--;
    }  // end   for every recursion
  }  // end   oo__rrprocessing()

static byte oo__whitespace[]= {
  0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,  // HT LF VT FF CR
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // SPC
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#define oo__ws(c) (oo__whitespace[(byte)(c)])
static byte oo__whitespacenul[]= {
  1,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,  // NUL HT LF VT FF CR
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,  // SPC /
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // >
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#define oo__wsnul(c) (oo__whitespacenul[(byte)(c)])
static byte oo__letter[]= {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,
  0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#define oo__le(c) (oo__letter[(byte)(c)])

static const uint8_t* oo__hexnumber= (uint8_t*)
  // convert a hex character to a number
  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x00\x00\x00\x00\x00\x00"
  "\x00\x0a\x0b\x0c\x0d\x0e\x0f\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x0a\x0b\x0c\x0d\x0e\x0f\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

static inline uint32_t oo__strtouint32(const char* s) {
  // read a number and convert it to an unsigned 64-bit integer;
  // return: number;
  int32_t i;
  byte b;

  i= 0;
  for(;;) {
    b= (byte)(*s++ -'0');
    if(b>=10)
  break;
    i= i*10+b;
    }
  return i;
  }  // end   oo__strtouint32()

#if 0  // presently unused
static inline int32_t oo__strtosint32(const char* s) {
  // read a number and convert it to a signed 64-bit integer;
  // return: number;
  int sign;
  int32_t i;
  byte b;

  if(*s=='-') { s++; sign= -1; } else sign= 1;
  i= 0;
  for(;;) {
    b= (byte)(*s++ -'0');
    if(b>=10)
  break;
    i= i*10+b;
    }
  return i*sign;
  }  // end   oo__strtosint32()
#endif

static inline int64_t oo__strtosint64(const char* s) {
  // read a number and convert it to a signed 64-bit integer;
  // return: number;
  int sign;
  int64_t i;
  byte b;

  if(*s=='-') { s++; sign= -1; } else sign= 1;
  i= 0;
  for(;;) {
    b= (byte)(*s++ -'0');
    if(b>=10)
  break;
    i= i*10+b;
    }
  return i*sign;
  }  // end   oo__strtosint64()

static const int32_t oo__nildeg= 2000000000L;

static inline int32_t oo__strtodeg(char* s) {
  // read a number which represents a degree value and
  // convert it to a fixpoint number;
  // s[]: string with the number between -180 and 180,
  //      e.g. "-179.99", "11", ".222";
  // return: number in 10 millionth degrees;
  //         =='oo__nildeg': syntax error;
  static const long di[]= {10000000L,10000000L,1000000L,100000L,
    10000L,1000L,100L,10L,1L};
  static const long* dig= di+1;
  int sign;
  int d;  // position of decimal digit;
  long k;
  char c;

  if(*s=='-') { s++; sign= -1; } else sign= 1;
  if(!isdig(*s) && *s!='.')
return border__nil;
  k= 0;
  d= -1;
  do {  // for every digit
    c= *s++;
    if(c=='.') { d= 0; continue; }  // start fractional part
    else if(!isdig(c) || c==0)
  break;
    k= k*10+c-'0';
    if(d>=0) d++;
    } while(d<7);  // end   for every digit
  k*= dig[d]*sign;
  return k;
  }  // end   oo__strtodeg()

static char* oo__tz=NULL;  // original value of time zone

static inline int64_t oo__strtimetosint64(const char* s) {
  // read a timestamp in OSM format, e.g.: "2010-09-30T19:23:30Z",
  // and convert it to a signed 64-bit integer;
  // return: time as a number (seconds since 1970);
  struct tm tm;

  tm.tm_isdst= 0;
  tm.tm_year=
    (s[0]-'0')*1000+(s[1]-'0')*100+(s[2]-'0')*10+(s[3]-'0')-1900;
  tm.tm_mon= (s[5]-'0')*10+s[6]-'0'-1;
  tm.tm_mday= (s[8]-'0')*10+s[9]-'0';
  tm.tm_hour= (s[11]-'0')*10+s[12]-'0';
  tm.tm_min= (s[14]-'0')*10+s[15]-'0';
  tm.tm_sec= (s[17]-'0')*10+s[18]-'0';
  #if __WIN32__  //,,,
  // use replcement for timegm() because Windows does not know it
  if(oo__tz==NULL) {
    oo__tz= getenv("TZ");
    putenv("TZ=");
    tzset();
    }
  return mktime(&tm);
  #else
  return timegm(&tm);
  #endif
  }  // end   oo__strtimetosint64()

static inline void oo__xmltostr(char* s) {
  // read an xml string and convert is into a regular UTF-8 string,
  // for example: "Mayer&apos;s" -> "Mayer's";
  char* t;  // pointer in overlapping target string
  char c;
  uint32_t u;

  //char* s0; s0= s;
  for(;;) {  // for all characters, until first '&' or string end;
    c= *s;
    if(c==0)  // no character to convert
return;
    if(c=='&')
  break;
    s++;
    }
  t= s;
  for(;;) {  // for all characters after the first '&'
    c= *s++;
    if(c==0)  // at the end of string
  break;
    if(c!='&') {
      *t++= c;
  continue;
      }
    c= *s;
    if(c=='#') {  // numeric value
      c= *++s;
      if(c=='x') {  // hex value
        s++;
        u= 0;
        for(;;) {
          c= *s++;
          if(c==';' || c==0)
        break;
          u= (u<<4)+oo__hexnumber[(byte)c];
          }
        }  // end   hex value
      else {  // decimal value
        u= 0;
        for(;;) {
          c= *s++;
          if(c==';' || c==0)
        break;
          u= u*10+c-'0';
          }
        }  // end   decimal value
      if(u<128)  // 1 byte sufficient
        *t++= (char)u;
      else if(u<2048) {  // 2 bytes sufficient
        *t++= (u>>6)|0xc0; *t++= (u&0x3f)|0x80; }
      else if(u<65536) {  // 3 bytes sufficient
        *t++= (u>>12)|0xe0; *t++= ((u>>6)&0x3f)|0x80;
        *t++= (u&0x3f)|0x80; }
      else {  // 4 bytes necessary
        *t++= ((u>>18)&0x03)|0xf0; *t++= ((u>>12)&0x3f)|0x80;
        *t++= ((u>>6)&0x3f)|0x80; *t++= (u&0x3f)|0x80; }
      }  // end   numeric value
    else if(strzcmp(s,"quot;")==0) {
      s+= 5; *t++= '\"'; }
    else if(strzcmp(s,"apos;")==0) {
      s+= 5; *t++= '\''; }
    else if(strzcmp(s,"amp;")==0) {
      s+= 4; *t++= '&'; }
    else if(strzcmp(s,"lt;")==0) {
      s+= 3; *t++= '<'; }
    else if(strzcmp(s,"gt;")==0) {
      s+= 3; *t++= '>'; }
    else {  // unknown escape code
      *t++= '&';
      }
    }  // end   for all characters after the first '&'
  *t= 0;  // terminate target string
  //fprintf(stderr,"Z %s\n",s0);sleep(1);//,,
  }  // end   oo__xmltostr()

static bool oo__xmlheadtag;  // currently, we are inside an xml start tag,
  // maybe a short tag, e.g. <node ... > or <node ... />
  // (the second example is a so-called short tag)
static char* oo__xmlkey,*oo__xmlval;  // return values of oo__xmltag

static inline bool oo__xmltag() {
  // read the next xml key/val and return them both;
  // due to performance reasons, global and module global variables
  // are used;
  // read_bufp: address at which the reading begins;
  // oo__xmlheadtag: see above;
  // return: no more xml keys/vals to read inside the outer xml tag;
  // oo__xmlkey,oo__xmlval: newest xml key/val which have been read;
  //                        ""/"": encountered the end of an
  //                               enclosed xml tag;
  char c;

  for(;;) {  // until break
    while(!oo__wsnul(*read_bufp)) read_bufp++;
      // find next whitespace or null character or '/'
    while(oo__ws(*read_bufp)) read_bufp++;
      // find first character after the whitespace(s)
    c= *read_bufp;
    if(c==0) {
      oo__xmlkey= oo__xmlval= (char *)"";
return true;
      }
    else if(c=='/') {
      oo__xmlkey= oo__xmlval=(char *) "";
      c= *++read_bufp;
      read_bufp++;
      if(c=='>') {  // short tag ands here
        if(oo__xmlheadtag) {
            // this ending short tag is the object's tag
          oo__xmlheadtag= false;
return true;
          }
return false;
        }  // end   short tag ands here
  continue;
      }
    else if(c=='<') {
      oo__xmlheadtag= false;
      if(*++read_bufp=='/' && (
          (c= *++read_bufp)=='n' || c=='w' || c=='r') ) {
        // this has been long tag which is ending now
        while(!oo__ws(*read_bufp)) read_bufp++;
          // find next whitespace
        oo__xmlkey= oo__xmlval= (char *)"";
return true;
        }
  continue;
      }
    oo__xmlkey= (char*)read_bufp;
    while(oo__le(*read_bufp)) read_bufp++;
    if(*read_bufp!='=') {
      oo__xmlkey= (char *)"";
  continue;
      }
    *read_bufp++= 0;
    if(*read_bufp!='\"')
  continue;
    oo__xmlval= (char*)(++read_bufp);
    for(;;) {
      c= *read_bufp;
      if(c=='\"')
    break;
      if(c==0) {
      oo__xmlkey= oo__xmlval= (char *)"";
return true;
        }
      read_bufp++;
      }
    *read_bufp++= 0;
  break;
    }  // end   until break
  oo__xmltostr(oo__xmlkey);
  oo__xmltostr(oo__xmlval);
  return false;
  }  // end   oo__xmltag()

typedef struct {
  read_info_t* ri;  // file handles for input files
  int format;  // input file format;
    // ==-9: unknown; ==0: o5m; ==10: xml; ==-1: pbf;
  str_info_t* str;  // string unit handle (if o5m format)
  uint64_t tyid;  // type/id of last read osm object of this file
  const char* filename;
  bool endoffile;
  bool deleteobject;  // replacement for .osc <delete> tag
  int64_t o5seqid;  // last o5m id, for delta coding in oo__gettyid()
  int64_t o5id;  // for o5m delta coding
  int32_t o5lon,o5lat;  // for o5m delta coding
  int64_t o5histime;  // for o5m delta coding
  int64_t o5hiscset;  // for o5m delta coding
  int64_t o5rid[3];  // for o5m delta coding
  } oo__if_t;
static oo__if_t oo__if[global_fileM];
static oo__if_t* oo__ifp= oo__if;  // currently used element in oo__if[]
#define oo__ifI (oo__ifp-oo__if)  // index
static oo__if_t* oo__ife= oo__if;  // logical end of elements in oo__if[]
static oo__if_t* oo__ifee= oo__if+global_fileM;
  // physical end of oo_if[]
static int oo_ifn= 0;  // number of currently open files

static inline int oo__gettyid() {
  // get tyid of the next object in the currently processed input file;
  // tyid is a combination of object type and id: we take the id and
  // add UINT64_C(0x0800000000000000) for nodes,
  // UINT64_C(0x1800000000000000) for ways, and
  // UINT64_C(0x2800000000000000) for relations;
  // oo__ifp: handle of the currently processed input file;
  // return: ==0: ok; !=0: could not get tyid because starting object
  //         is not an osm object;
  // oo__ifp->tyid: tyid of the starting osm object;
  //                if there is not an osm object starting at
  //                read_bufp, *iyidp remains unchanged;
  static const uint64_t idoffset[]= {UINT64_C(0x0800000000000000),
    UINT64_C(0x1800000000000000),UINT64_C(0x2800000000000000),
    0,0,0,0,0,0,0,0,0,0,0,0,0,UINT64_C(0x0800000000000000),
    UINT64_C(0x1800000000000000),UINT64_C(0x2800000000000000)};
  int format;

  format= oo__ifp->format;
  if(format==0) {  // o5m
    byte* p;

    p= read_bufp;
    if(*p==0xff) {  // this is an o5m Reset object
      oo__ifp->o5seqid= 0;
return 1;
      }
    if((*p & 0xfc)!=0x10)  // no osm object is starting here
return 1;
    oo__ifp->tyid= idoffset[*p++];
    pbf_intjump(&p);  // jump over length information
    oo__ifp->o5seqid+= pbf_sint64(&p);
    oo__ifp->tyid+= oo__ifp->o5seqid;
return 0;
    }
  else if(format>0) {  // 10: osm xml
    char* s;
    uint64_t r;

    s= (char*)read_bufp;
    for(;;) {
      while(*s!='<' && *s!=0) s++;
      if(*s==0)
    break;
      s++;
      if(*s=='n' && s[1]=='o') r= UINT64_C(0x0800000000000000);
      else if(*s=='w') r= UINT64_C(0x1800000000000000);
      else if(*s=='r') r= UINT64_C(0x2800000000000000);
      else
    break;
      do {
        s++;
        if(s==0)
    break;
        } while(*s!=' ');
      while(*s==' ') s++;
      if(s[0]!='i' || s[1]!='d' || s[2]!='=' || s[3]!='\"')
    continue;
      oo__ifp->tyid= r+oo__strtosint64(s+4);
return 0;
      }
return 1;
    }
  else if(format==-1) {  // pbf
    if((pb_type & 3)!=pb_type)  // not an osm object
return 1;
    oo__ifp->tyid= idoffset[pb_type]+pb_id;
return 0;
    }
return 2;  // (unknown format)
  }  // end   oo__gettyid()

static inline int oo__getformat() {
  // determine the formats of all opened files of unknown format
  // and store these determined formats;
  // do some intitialization for the format, of necessary;
  // oo__if[].format: !=-9: do nothing for this file;
  // return: 0: ok; !=0: error;
  //         5: too many pbf files;
  //            this is, because the module pbf (see above)
  //            does not have multi-client capabilities;
  // oo__if[].format: input file format; ==0: o5m; ==10: xml; ==-1: pbf;
  static int pbffilen= 0;  // number of pbf input files;
  oo__if_t* ifptemp;
  byte* bufp;
  #define bufsp ((char*)bufp)  // for signed char

  ifptemp= oo__ifp;
  oo__ifp= oo__if;
  while(oo__ifp<oo__ife) {  // for all input files
    if(oo__ifp->ri!=NULL && oo__ifp->format==-9) {
        // format not yet determined
      read_switch(oo__ifp->ri);
      if(read_bufp>=read_bufe) {  // file empty
        PERRv("file empty: %.80s",oo__ifp->filename)
return 2;
        }
      bufp= read_bufp;
      if(bufp[0]==0 && bufp[1]==0 && bufp[2]==0 &&
          bufp[3]>8 && bufp[3]<20) {  // presumably .pbf format
        if(++pbffilen>1) {   // pbf
          PERR("more than one .pbf input file is not allowed.");
return 5;
          }
        oo__ifp->format= -1;
        pb_ini();
        }
      else if(strzcmp(bufsp,"<?xml")==0 ||
          strzcmp(bufsp,"<osm")==0) {  // presumably .osm format
        oo__ifp->format= 10;
        }
      else if(bufp[0]==0xff && bufp[1]==0xe0 &&
          strzcmp(bufsp+2,"\x03""o5m")==0) {  // presumably .o5m format
        oo__ifp->format= 0;
        oo__ifp->str= str_open();
          // call some initialization of string read module
        }
      else if((bufp[0]==0xff && bufp[1]>=0x10 && bufp[1]<=0x12) ||
          (bufp[0]==0xff && bufp[1]==0xff &&
          bufp[2]>=0x10 && bufp[2]<=0x12) ||
          (bufp[0]==0xff && read_bufe==read_bufp+1)) {
          // presumably shortened .o5m format
        if(loglevel>=2)
          fprintf(stderr,"osmconvert: Not a standard .o5m file header "
            "%.80s\n",oo__ifp->filename);
        oo__ifp->format= 0;
        oo__ifp->str= str_open();
          // call some initialization of string read module
        }
      else {  // unknown file format
        PERRv("unknown file format: %.80s",oo__ifp->filename)
return 3;
        }
      oo__ifp->tyid= 0;
      oo__gettyid();
        // initialize tyid of the currently used input file
      }  // format not yet determined
    oo__ifp++;
    }  // for all input files
  oo__ifp= ifptemp;
  return 0;
  #undef bufsp
  }  // end oo__getformat()

static bool oo__switchignore= false;  // this request is set by
  // oo__switch() and orders the caller, i.e. oo_main(), to ignore
  // the next object;

static inline void oo__switch() {
  // determine the input file with the lowest tyid
  // and switch to it
  oo__if_t* ifp,*ifpmin;
  uint64_t tyidmin,tyidold,tyid;

  // update tyid of the currently used input file and check sequence
  if(oo__ifp!=NULL) {  // handle of current input file is valid
    tyidold= oo__ifp->tyid;
    if(oo__gettyid()==0) {  // new tyid is valid
      if(oo__ifp->tyid<tyidold) {  // wrong sequence
        int64_t id; int ty;

        ty= tyidold>>60;
        id= ((int64_t)(tyidold & UINT64_C(0xfffffffffffffff)))-
          INT64_C(0x800000000000000);
        WARNv("wrong order at %s %Ld in file %s",
          ONAME(ty),id,oo__ifp->filename)
        }  // wrong sequence
      }  // new tyid is valid
    }  // end   handle of current input file is valid

  // find file with smallest tyid
  tyidmin= UINT64_C(0xffffffffffffffff);
  ifpmin= oo__ifp;
    // default; therefore we do not switch in cases we do not
    // find a minimum
  ifp= oo__ife;
  while(ifp>oo__if) {
    ifp--;
    if(ifp->ri!=NULL) {  // file handle valid
      tyid= ifp->tyid;
      if(tyid<tyidmin) {
        tyidmin= tyid;
        ifpmin= ifp;
        }
      }  // file handle valid
    }

  // switch to that file ,,,
  if(ifpmin!=oo__ifp) {
      // that file is not the file we're already reading from
    oo__ifp= ifpmin;
    read_switch(oo__ifp->ri);
    str_switch(oo__ifp->str);
    }

  // set ignore request if tyid smaller or same as that tyid
  // we already had
  /* block */ {
    static uint64_t tyid_old= 0;

    oo__switchignore= oo__ifp->tyid<=tyid_old && oo__ifp->tyid!=0;
    tyid_old= oo__ifp->tyid;
    }
  }  // end oo__switch()

static void oo__close() {
  // close an input file;
  // oo__ifp: handle of currently active input file;
  // if this file has already been closed, nothing happens;
  // after calling this procedure, the handle of active input file
  // will be invalid; you may call oo__switch() to select the
  // next file in the sequence;
  if(oo__ifp!=NULL && oo__ifp->ri!=NULL) {
    if(!oo__ifp->endoffile  && oo_ifn>0)  // missing logical end of file
      fprintf(stderr,"osmconvert Warning: "
        "unexpected end of input file: %.80s\n",oo__ifp->filename);
    read_close(oo__ifp->ri);
    oo__ifp->ri= NULL;
    oo__ifp->tyid= UINT64_C(0xffffffffffffffff);
      // (to prevent this file being selected as next file
      // in the sequence)
    oo_ifn--;
    }
  oo__ifp= NULL;
  }  // end oo__close()

static void oo__end() {
  // clean-up this module;
  oo_ifn= 0;  // mark end of program;
    // this is used to suppress warning messages in oo__close()
  while(oo__ife>oo__if) {
    oo__ifp= --oo__ife;
    oo__close();
    }
  oo_ifn= 0;
  if(oo__tz!=NULL) {  // time zone must be restored
    char s[256];

    snprintf(s,sizeof(s),"TZ=%s",oo__tz);
    putenv(s);
    tzset();
    oo__tz= NULL;
    }  // time zone must be restored
  }  // end oo__end()

//------------------------------------------------------------

static bool oo_open(const char* filename) {
  // open an input file;
  // filename[]: path and name of input file;
  //             ==NULL: standard input;
  // return: 0: ok; 1: no appropriate input file;
  //         2: maximum number of input files exceeded;
  // the handle for the current input file oo__ifp is set
  // to the opened file;
  // after having opened all input files, call oo__getformat();
  // you do not need to care about closing the file;
  static bool firstrun= true;

  if(oo__ife>=oo__ifee) {
    fprintf(stderr,"osmconvert Error: too many input files.\n");
return 2;
    }
  if(read_open(filename)!=0)
return 1;
  oo__ife->ri= read_infop;
  oo__ife->str= NULL;
  oo__ife->format= -9;  // 'not yet determined'
  oo__ife->tyid= 0;
  oo__ife->filename= filename;
  oo__ife->endoffile= false;
  oo__ife->deleteobject= false;
  oo__ife->o5seqid= 0;
  oo__ifp= oo__ife++;
  oo_ifn++;
  if(firstrun) {
    firstrun= false;
    atexit(oo__end);
    }
  return 0;
  }  // end   oo_open()

static int oo_sequencetype= -1;
  // type of last object which has been processed;
  // -1: no object yet; 0: node; 1: way; 2: relation;
static int64_t oo_sequenceid= INT64_C(-0x7fffffffffffffff);
  // id of last object which has been processed;

static int oo_main() {
  // start reading osm objects;
  // return: ==0: ok; !=0: error;
  // this procedure must only be called once;
  // before calling this procedure you must open an input file
  // using oo_open();
  int wformat;
  bool hashactive;
    // must be set to true if border_active OR global_dropbrokenrefs;
  int dependenciesstage;
    // stage of the processing of interrelational dependencies;
    // only used if borders are to be applied and relations shall be
    // processed recursively;
    // 0: no recursive processing at all;
    // 1: write nodes and ways, change stage to 2 as soon as
    //    first relation has been encountered;
    // 1->2: at this moment, change the regular output file to a
    //       tempfile, and switch output format to .o5m;
    // 2: write interrelation references into a second to tempfile,
    //    use module rr_ for this purpose;
    // 2->3: at this moment, change output back to standard output,
    //       and change input to the start of the tempfile;
    //       in addition to this, process temporarily stored
    //       interrelation data;
    // 3: write only relations, use tempfile as input;
  static char o5mtempfile[400];  // must be static because
    // this file will be deleted by an at-exit procedure;
  #define oo__maxrewindINI 12
  int maxrewind;  // maximum number of relation-relation dependencies
  bool writeheader;  // header must be written
  bool bboxvalid;  // the following bbox coordinates are valid;
  int32_t x1,y1,x2,y2;  // bbox coordinates (base 10^-7);
  int otype;  // type of currently processed object;
    // 0: node; 1: way; 2: relation;
  uint32_t complete;  // flags for valid data
  bool hisavailable;  // history information available
  int64_t id;  // flag mask 1 (see oo__if_t)
  int32_t lon,lat;  // flag masks 2, 4 (see oo__if_t)
  uint32_t hisver;  // flag mask 8
  int64_t histime;  // flag mask 16 (see oo__if_t)
  int64_t hiscset;  // flag mask 32 (see oo__if_t)
  uint32_t hisuid;  // flag mask 64
  string hisuser;  // flag mask 128
  // int64_t rid[3];  // for delta-coding (see oo__if_t)
  #define oo__refM 100000
  int64_t refid[oo__refM];
  int64_t* refidee;  // end address of array
  int64_t* refide,*refidp;  // pointer in array
  byte reftype[oo__refM];
  byte* reftypee,*reftypep;  // pointer in array
  char* refrole[oo__refM];
  char** refrolee,**refrolep;  // pointer in array
  #define oo__keyvalM 4000
  char* key[oo__keyvalM],*val[oo__keyvalM];
  char** keyee;  // end address of first array
  char** keye,**keyp;  // pointer in array
  char** vale,**valp;  // pointer in array
  byte* bufp;  // pointer in read buffer
  #define bufsp ((char*)bufp)  // for signed char
  byte* bufe;  // pointer in read buffer, end of object
  char c;  // latest character which has been read
  byte b;  // latest byte which has been read
  int l;
  byte* bp;
  char* sp;

  // procedure initialization
  atexit(oo__end);
  bboxvalid= false;
  hashactive= border_active || global_dropbrokenrefs;
  dependenciesstage= 0;  // 0: no recursive processing at all;
  maxrewind= oo__maxrewindINI;
  writeheader= true;
  refidee= refid+oo__refM;
  keyee= key+oo__keyvalM;

  // get input file format and care about tempfile name
  if(oo__getformat())
return 5;
  if(hashactive && !global_droprelations) {
      // borders to apply AND relations are required
    dependenciesstage= 1;  // initiate recursive processing;
      // 1: write nodes and ways, change stage to 2 as soon as
      //    first relation has been encountered;
    strcpy(stpmcpy(o5mtempfile,global_tempfilename,
      sizeof(o5mtempfile)-2),".1");
    }
  else
    dependenciesstage= 0;  // no recursive processing

  // process the file
  for(;;) {  // read all input files

    // get next object
    read_input();
    pb_type= 0;  // (default)
    if(oo__ifp->format==-1) pb_input();

    // merging - if more than one file
    if(oo_ifn>1 && dependenciesstage<=2)
      oo__switch();
    else if(global_mergeduplicates)
      oo__gettyid();

    // care about recursive processing
    if((read_bufp>=read_bufe && oo__ifp->format>=0) ||
        pb_type<0) {  // at end of input file
        // 0: no recursive processing at all;
        // 1: write nodes and ways, change stage to 2 as soon as
        //    first relation has been encountered;
        // 1->2: at this moment, change the regular output file to a
        //       tempfile, and switch output format to .o5m;
        // 2: write interrelation references into a second to tempfile,
        //    use module rr_ for this purpose;
        // 2->3: at this moment, change output back to standard output,
        //       change output format to original output format,
        //       and change input to the start of the tempfile;
        //       in addition to this, process temporarily stored
        //       interrelation data;
        // 3: write only relations, use tempfile as input;
      if(pb_type<0) {
        if(pb_type<-1)  // error
return 1000-pb_type;
        oo__ifp->endoffile= true;
        }
      if(dependenciesstage!=2) {
          // 0: no recursive processing at all;
          // 1: write nodes and ways, change stage to 2 as soon as
          //    first relation has been encountered;
          // 3: write only relations, use tempfile as input;
        if(dependenciesstage==3) {
          oo__ifp->endoffile= true;
            // this is because the file we have read
            // has been created as temporary file by the program
            // and does not contain an eof object;
          if(loglevel>0) fprintf(stderr,
            "Relation hierarchies: %i of maximal %i.\n",
            oo__maxrewindINI-maxrewind,oo__maxrewindINI);
          if(maxrewind<0)
            fprintf(stderr,
              "osmconvert Warning: relation dependencies too complex\n"
              "         (more than %i hierarchy levels).\n"
              "         A few relations might have been excluded\n"
              "         although lying within the borders.\n",
              oo__maxrewindINI);
          }
        oo__close();
        if(oo_ifn<=0)  // we just closed the last input file
  break;
        oo__switch();  // select the next file in sequence
  continue;
        }  // end   dependenciesstage!=2
      // here: dependenciesstage==2
      // 2: write interrelation references into a second to tempfile,
      //    use module rr_ for this purpose;
      // 2->3: at this moment, change output back to standard output,
      //       change output format to original output format,
      //       and change input to the start of the tempfile;
      //       in addition to this, process temporarily stored
      //       interrelation data;
      wo_flush();
      wo_reset();
      wo_flush();
      if(write_newfile(NULL))
return 21;
      wo_format(wformat);
      oo__rrprocessing(&maxrewind);
      // 3: write only relations, use tempfile as input;
      dependenciesstage= 3;  // enter next stage
      oo__close();
      if(oo_open(o5mtempfile))
return 22;
      if(oo__getformat())
return 23;
      read_input();
  continue;
      }  // end   at end of input file
    if(dependenciesstage==1)
        // 1: write nodes and ways, change stage to 2 as soon as
        //    first relation has been encountered;
    if(oo__ifp->endoffile) {  // after logical end of file
      fprintf(stderr,"osmconvert Warning: unexpected contents "
        "after logical end of file.\n");
  break;
      }
    bufp= read_bufp;
    b= *bufp; c= (char)b;

    // care about header and unknown objects
    if(oo__ifp->format<0) {  // -1, pbf
      if(pb_type<0 || pb_type>2) {  // not a regular dataset id
        if(pb_type==8) {  // pbf header
          x1= pb_bbx1; y1= pb_bby1;
          x2= pb_bbx2; y2= pb_bby2;
          bboxvalid= pb_bbvalid;
          }  // end   pbf header
  continue;
        }  // end not a regular dataset id
      otype= pb_type;
      }  // end   pbf
    else if(oo__ifp->format==0) {  // o5m
      if(b<0x10 || b>0x12) {  // not a regular dataset id
        if(b>=0xf0) {  // single byte dataset
          if(b==0xff) {  // file start, resp. o5m reset
            oo__ifp->o5id= 0;
            oo__ifp->o5lat= oo__ifp->o5lon= 0;
            oo__ifp->o5hiscset= 0;
            oo__ifp->o5histime= 0;
            oo__ifp->o5rid[0]= oo__ifp->o5rid[1]= oo__ifp->o5rid[2]= 0;
            str_reset();
            }
          else if(b==0xfe)
            oo__ifp->endoffile= true;
          else if(write_testmode)
            WARNv("unknown .o5m short dataset id: 0x%02x\n",b)
          read_bufp++;
  continue;
          }  // end   single byte dataset
        else {  // unknown multibyte dataset
          if(write_testmode && b!=0xe0)
            WARNv("unknown .o5m dataset id: 0x%02x\n",b)
          read_bufp++;
          l= pbf_uint32(&read_bufp);  // jump over this dataset
          read_bufp+= l;  // jump over this dataset
  continue;
          }  // end   unknown multibyte dataset
        }  // end   not a regular dataset id
      otype= b&3;
      }  // end   o5m
    else {  // xml
      while(c!=0 && c!='<') c= (char)*++bufp;
      if(c==0) {
  break;
        }
      c= bufsp[1];
      if(c=='n' && bufsp[2]=='o')  // node
        otype= 0;
      else if(c=='w')  // way
        otype= 1;
      else if(c=='r')  // relation
        otype= 2;
      else if(c=='b') {  // bounds
        uint32_t bboxcomplete;  // flags for x1,y1,x2,y2

        bboxcomplete= 0;
        bufp+= 8;  // jump over "<bounds "
        for(;;) {  // for every word in 'bounds'
          c= *bufsp;
          if(c=='/' || c=='>' || c==0)
        break;
          if(oo__ws(c)) {
            bufp++;
        continue;
            }
          if((l= strzlcmp(bufsp,"minlon=\""))>0) {
            bufp+= l;
            x1= oo__strtodeg(bufsp);
            if(x1!=oo__nildeg) bboxcomplete|= 1;
            }
          else if((l= strzlcmp(bufsp,"minlat=\""))>0) {
            bufp+= l;
            y1= oo__strtodeg(bufsp);
            if(y1!=oo__nildeg) bboxcomplete|= 2;
            }
          else if((l= strzlcmp(bufsp,"maxlon=\""))>0) {
            bufp+= l;
            x2= oo__strtodeg(bufsp);
            if(x2!=oo__nildeg) bboxcomplete|= 4;
            }
          else if((l= strzlcmp(bufsp,"maxlat=\""))>0) {
            bufp+= l;
            y2= oo__strtodeg(bufsp);
            if(y2!=oo__nildeg) bboxcomplete|= 8;
            }
          for(;;) {  // find next blank
            c= *bufsp;
            if(oo__wsnul(c))
          break;
            bufp++;
            }
          }  // end   for every word in 'bounds'
        bboxvalid= bboxcomplete==15;
        read_bufp= bufp;
  continue;
        }  // end   bounds
      else if(c=='c' || (c=='m' && bufsp[2]=='o') || c=='d') {
          // create, modify or delete
        oo__ifp->deleteobject= c=='d';
        read_bufp= bufp+1;
  continue;
        }   // end   create, modify or delete
      else if(c=='/') {  // xml end object
        if(bufsp[2]=='d')  // end of delete
          oo__ifp->deleteobject= false;
        else if(strzcmp(bufsp+2,"osm>")==0) {  // end of file
          oo__ifp->endoffile= true;
          read_bufp= bufp+6;
          while(oo__ws(*read_bufp)) read_bufp++;
  continue;
          }   // end   end of file
        goto unknownxmlobject;
        }   // end   xml end object
      else {  // unknown xml object
        unknownxmlobject:
        bufp++;
        for(;;) {  // find end of xml object
          c= *bufsp;
          if(c=='>' || c==0)
        break;
          bufp++;
          }
        read_bufp= bufp;
  continue;
        }  // end   unknown XML object
      read_bufp= bufp;
      }  // end   xml

    // write header
    if(writeheader) {
      writeheader= false;
      if(global_outo5m) wformat= 0;
      else if(global_emulatepbf2osm) wformat= 12;
      else if(global_emulateosmosis) wformat= 13;
      else wformat= 11;
      wo_start(wformat,bboxvalid && oo_ifn==1,x1,y1,x2,y2);
      }

    readobject:

    // object initialization
    complete= 0;
    hisuid= 0;
    hisuser= "";
    refide= refid;
    reftypee= reftype;
    refrolee= refrole;
    keye= key;
    vale= val;

    // read one osm object
    if(oo__ifp->format<0) {  // pbf
      // read id
      id= pb_id; complete|= 1;
      // read coordinates (for nodes only)
      if(otype==0) {  // node
        lon= pb_lon; lat= pb_lat;
        complete|= 2+4;
        }  // node
      // read history
      hisver= pb_hisver;
      if(hisver!=0) {  // history information available
        histime= pb_histime;
        hiscset= pb_hiscset;
        hisuid= pb_hisuid;
        hisuser= pb_hisuser;
        if(!global_drophistory) complete|= 8+16+32+64+128;
        }  // end   history information available
      // read noderefs (for ways only)
      if(otype==1)  // way
        refide= refid+pb_noderef(refid,oo__refM);
      // read refs (for relations only)
      else if(otype==2) {  // relation
        l= pb_ref(refid,reftype,refrole,oo__refM);
        refide= refid+l;
        reftypee= reftype+l;
        refrolee= refrole+l;
        }  // end   relation
      // read node key/val pairs
      l= pb_keyval(key,val,oo__keyvalM);
      keye= key+l; vale= val+l;
      }  // end   pbf
    else if(oo__ifp->format==0) {  // o5m
      // read object id
      bufp++;
      l= pbf_uint32(&bufp);
      read_bufp= bufe= bufp+l;
      id= oo__ifp->o5id+= pbf_sint64(&bufp);
      if(bufp>=bufe) // just the id, i.e. this is a delete request
        oo__ifp->deleteobject= true;
      else {  // not a delete request
        oo__ifp->deleteobject= false;
        // read coordinates (for nodes only)
        if(otype==0) {  // node
          // read node body
          lon= oo__ifp->o5lon+= pbf_sint32(&bufp);
          lat= oo__ifp->o5lat+= pbf_sint32(&bufp);
          }  // end   node
        complete|= 1+2+4;
        // read history
        hisver= pbf_uint32(&bufp);
        if(hisver!=0) {  // history information available
          histime= oo__ifp->o5histime+= pbf_sint64(&bufp);
          hiscset= oo__ifp->o5hiscset+= pbf_sint32(&bufp);
	  string tmp(sp);
          str_read(&bufp,tmp,hisuser);
          hisuid= pbf_uint64((byte**)&sp);
          if(!global_drophistory) complete|= 8+16+32+64+128;
          }  // end   history information available
        // read noderefs (for ways only)
        if(otype==1) {  // way
          l= pbf_uint32(&bufp);
          bp= bufp+l;
          if(bp>bufe) bp= bufe;  // (format error)
          while(bufp<bp && refide<refidee)
            *refide++= oo__ifp->o5rid[0]+= pbf_sint64(&bufp);
          }  // end   way
        // read refs (for relations only)
        else if(otype==2) {  // relation
          int64_t ri;  // temporary, refid
          int rt;  // temporary, reftype
          char* rr;  // temporary, refrole

          l= pbf_uint32(&bufp);
          bp= bufp+l;
          if(bp>bufe) bp= bufe;  // (format error)
          while(bufp<bp && refide<refidee) {
            ri= pbf_sint64(&bufp);
            str_read(&bufp,&rr,NULL);
            *reftypee++= rt= (*rr++ -'0')%3;  // (suppress errors)
            *refide++= oo__ifp->o5rid[rt]+= ri;
            *refrolee++= rr;
            }
          }  // end   relation
        // read node key/val pairs
        keye= key; vale= val;
        while(bufp<bufe && keye<keyee)
          str_read(&bufp,keye++,vale++);
        }  // end   not a delete request
      }  // end   o5m
    else {  // xml
      int64_t ri;  // temporary, refid, rcomplete flag 1
      int rt;  // temporary, reftype, rcomplete flag 2
      char* rr;  // temporary, refrole, rcomplete flag 3
      int rcomplete;
      char* k;  // temporary, key
      char* v;  // temporary, val
      int r;

      read_bufp++;  // jump over '<'
      oo__xmlheadtag= true;  // (default)
      rcomplete= 0;
      k= v= NULL;
      for(;;) {  // until break;
        r= oo__xmltag();
        if(oo__xmlheadtag) {  // still in object header
          if(oo__xmlkey[0]=='i') { // id
            id= oo__strtosint64(oo__xmlval); complete|= 1; }
          else if(oo__xmlkey[0]=='l') {  // letter l
            if(oo__xmlkey[1]=='o') { // lon
              lon= oo__strtodeg(oo__xmlval); complete|= 2; }
            else if(oo__xmlkey[1]=='a') { // lon
              lat= oo__strtodeg(oo__xmlval); complete|= 4; }
            }  // end   letter l
          else if(!global_drophistory) {  // history not to drop
            if(oo__xmlkey[0]=='v' && oo__xmlkey[1]=='e') { // hisver
              hisver= oo__strtouint32(oo__xmlval); complete|= 8; }
            else if(oo__xmlkey[0]=='t') { // histime
              histime= oo__strtimetosint64(oo__xmlval); complete|= 16; }
            else if(oo__xmlkey[0]=='c') { // hiscset
              hiscset= oo__strtosint64(oo__xmlval); complete|= 32; }
            else if(oo__xmlkey[0]=='u' && oo__xmlkey[1]=='i') { // hisuid
              hisuid= oo__strtouint32(oo__xmlval); complete|= 64; }
            else if(oo__xmlkey[0]=='u' && oo__xmlkey[1]=='s') { //hisuser
              hisuser= oo__xmlval; complete|= 128; }
            }  // end   history not to drop
          }  // end   still in object header
        else {  // in object body
          if(oo__xmlkey[0]==0) {  // xml tag completed
            if(rcomplete>=3) {  // at least refid and reftype
              *refide++= ri;
              *reftypee++= rt;
              if(rcomplete<4)  // refrole is missing
                rr= (char*)"";  // assume an empty string as refrole
              *refrolee++= rr;
              }  // end   at least refid and reftype
            rcomplete= 0;
            if(v!=NULL && k!=NULL) {  // key/val available
              *keye++= k; *vale++= v;
              k= v= NULL;
              }  // end   key/val available
            }  // end   xml tag completed
          else {  // inside xml tag
            if(otype!=0 && refide<refidee) {
                // not a node AND still space in refid array
              if(oo__xmlkey[0]=='r' && oo__xmlkey[1]=='e') { // refid
                ri= oo__strtosint64(oo__xmlval); rcomplete|= 1;
                if(otype==1) {rt= 0; rcomplete|= 2; } }
              else if(oo__xmlkey[0]=='t' && oo__xmlkey[1]=='y') {
                  // reftype
                rt= oo__xmlval[0]=='n'? 0: oo__xmlval[0]=='w'? 1: 2;
                rcomplete|= 2; }
              else if(oo__xmlkey[0]=='r' && oo__xmlkey[1]=='o') {
                  // refrole
                rr= oo__xmlval; rcomplete|= 4; }
              }  // end   still space in refid array
            if(keye<keyee) {  // still space in key/val array
              if(oo__xmlkey[0]=='k') // key
                k= oo__xmlval;
              else if(oo__xmlkey[0]=='v') // val
                v= oo__xmlval;
              }  // end   still space in key/val array
            }  // end   inside xml tag
          }  // end   in object body
        if(r)
      break;
        }  // end   until break;
      }  // end   xml

    // care about multiple occurrences of one object within one file
    if(global_mergeduplicates) {
        // user allows duplicate objects in input file; that means
        // we must take the last object only of each duplicate
        // because this is assumed to be the newest; ,,,
      uint64_t tyidold;

      tyidold= oo__ifp->tyid;
      if(oo__gettyid()==0) {
        if(oo__ifp->tyid==tyidold)  // next object has same type and id
          goto readobject;  // dispose this object and take the next
        }
      }

    // stop processing if object is to ignore
    if(oo__switchignore) {
      oo__switchignore= false;
  continue;
      }

    // stop processing if in wrong stage for nodes or ways
    if(dependenciesstage>=2 && otype<=1)
        // 2: write interrelation references into a second to tempfile,
        //    use module rr_ for this purpose;
        // 3: write only relations, use tempfile as input;
  continue;  // ignore this object

    // care about possible array overflows
    if(refide>refidee)
      WARNv("way %Ld has too many noderefs.\n",id)
    if(refide>refidee)
      WARNv("relation %Ld has too many refs.\n",id)
    if(keye>=keyee)
      WARNv("%s %Ld has too many key/val pairs.\n",
        ONAME(otype),id)

    // check sequence, if necessary
    if(oo_ifn==1 && dependenciesstage!=3) {
      if(otype<=oo_sequencetype &&
          (otype<oo_sequencetype || id<=oo_sequenceid))
        WARNv("wrong sequence at %s %Ld.\n",ONAME(otype),id)
      }
    oo_sequencetype= otype; oo_sequenceid= id;

    // process object deletion
    if(oo__ifp->deleteobject) {  // object is to delete
      if((otype==0 && !global_dropnodes) ||
          (otype==1 && !global_dropways) ||
          (otype==2 && !global_droprelations))
          // section is not to drop anyway
        if(global_outo5c || global_outosc)  // write o5c or osc file
          wo_delete(otype,id);  // write delete request
  continue;  // end processing for this object
      }  // end   object is to delete

    // write the object
    hisavailable= (complete&(8+16+32))==(8+16+32);
    if(otype==0) {  // write node
      if(!border_active || border_queryinside(lon,lat)) {
          // no border to be applied OR node lies inside
        if(hashactive)  // border are to be applied
          hash_seti(0,id);  // mark this node id as 'inside'
        if(!global_dropnodes) {  // not to drop
          if(hisavailable)  // no history information
            wo_node(id,lon,lat,
		    hisver,histime,hiscset,hisuid,hisuser.c_str());
          else  // history information available
            wo_node(id,lon,lat,0,0,0,0,"");
          keyp= key; valp= val;
          while(keyp<keye)  // for all key/val pairs of this object
            wo_keyval(*keyp++,*valp++);
          }  // end   not to drop
        }  // end   no border to be applied OR node lies inside
      }  // write node
    else if(otype==1) {  // write way
      bool inside;  // way lies inside borders, if appl.

      if(!hashactive)  // no borders shall be applied
        inside= true;
      else {  // borders are to be applied
        inside= false;  // (default)
        refidp= refid;
        while(refidp<refide) {  // for every referenced node
          if(hash_geti(0,*refidp)) {
            inside= true;
        break;
            }
          refidp++;
          }  // end   for every referenced node
        }  // end   borders are to be applied
      if(inside) {  // no borders OR at least one node inside
        if(hashactive)
          hash_seti(1,id);  // memorize that this way lies inside
        if(!global_dropways) {  // not ways to drop
          if(hisavailable)  // no history information
            wo_way(id,hisver,histime,hiscset,hisuid,hisuser.c_str());
          else  // history information available
            wo_way(id,0,0,0,0,"");
          refidp= refid;
          while(refidp<refide) {  // for every referenced node
            if(!global_dropbrokenrefs || hash_geti(0,*refidp))
                // referenced node lies inside the borders
              wo_noderef(*refidp);
            refidp++;
            }  // end   for every referenced node
          keyp= key; valp= val;
          while(keyp<keye)  // for all key/val pairs of this object
            wo_keyval(*keyp++,*valp++);
          }  // end   not ways to drop
        }  // end   no border OR at least one node inside
      }  // write way
    else if(otype==2) {  // write relation
      if(!global_droprelations) {  // not relations to drop
        bool inside;  // relation may lie inside borders, if appl.
        bool in;  // relation lies inside borders
        int64_t ri;  // temporary, refid
        int rt;  // temporary, reftype
        char* rr;  // temporary, refrole

        if(dependenciesstage==1) {
            // 1: write nodes and ways, change stage to 2 as soon as
            //    first relation has been encountered;
          // 1->2: at this moment, change the regular output file to a
          //       tempfile, and switch output format to .o5m;
          dependenciesstage= 2;
          wo_flush();
          if(write_newfile(o5mtempfile))
return 23;
          wo_format(0);
          if(rr_ini(global_tempfilename))
return 24;
          }
        in= hash_geti(2,id);
        if(dependenciesstage==2) {
            // 2: write interrelation references into a second to
            //    tempfile, use module rr_ for this purpose;
          bool idwritten;

          idwritten= false;
          refidp= refid; reftypep= reftype;
          while(refidp<refide) {  // for every referenced object
            ri= *refidp;
            rt= *reftypep;
            if(rt==2) {  // referenced object is a relation
              if(!idwritten) {  // did not yet write our relation's id
                rr_rel(id);  // write it now
                idwritten= true;
                }
              rr_ref(ri);
              }
            refidp++; reftypep++;
            }  // end   for every referenced object
          inside= true;
          }
        else if(dependenciesstage==3) {
          inside= in;
          }
        else
          inside= true;
        if(inside) {  // no borders OR at least one node inside
          if(hisavailable)  // no history information
            wo_relation(id,hisver,histime,hiscset,hisuid,hisuser.c_str());
          else  // history information available
            wo_relation(id,0,0,0,0,"");
          refidp= refid; reftypep= reftype; refrolep= refrole;
          while(refidp<refide) {  // for every referenced object
            ri= *refidp;
            rt= *reftypep;
            rr= *refrolep;

            if(dependenciesstage<3) {
              if(rt==2 || hash_geti(rt,ri)) {
                  // referenced object is a relation OR
                  // lies inside the borders
                wo_ref(ri,rt,rr);
                if(rt!=2 && !in) {
                  hash_seti(2,id); in= true; }
                }
              else {  // referenced object lies outside the borders
                if(!global_dropbrokenrefs) {
                  wo_ref(ri,rt,rr);
                  }
                }
              }
            else {  // dependenciesstage==3
              if(!global_dropbrokenrefs || hash_geti(rt,ri)) {
                  // broken refs are to be listed anyway OR
                  // referenced object lies inside the borders
                wo_ref(ri,rt,rr);
                }
              }
            refidp++; reftypep++; refrolep++;
            }  // end   for every referenced object
          keyp= key; valp= val;
          while(keyp<keye)  // for all key/val pairs of this object
            wo_keyval(*keyp++,*valp++);
          }  // end   no borders OR at least one node inside
        }  // end   not relations to drop
      }  // write relation
    }  // end   read all input files
  wo_end();
  return 0;
  }  // end   oo_main()

//------------------------------------------------------------
// end   Module oo_   osm to osm module
//------------------------------------------------------------



int main(int argc,const char** argv) {
  // main program;
  // for the meaning of the calling line parameters please look at the
  // contents of helptext[];
  int h_n,h_w,h_r;  // user-suggested hash size in MiB, for
    // hash tables of nodes, ways, and relations;
  int r;

  // initializations
  h_n= h_w= h_r= 0;
  #ifdef _WIN32
    setmode(fileno(stdout),O_BINARY);
    setmode(fileno(stdin),O_BINARY);
  #endif

  // read command line parameters
  if(argc<=1) {  // no command line parameters given
    fprintf(stderr,"osmconvert " VERSION "\n"
      "Converts .osm, .o5m, .pbf, .osc files to .osm, .o5m, o5c files,\n"
      "applies changes of .osc, .o5c files and sets limiting borders.\n"
      "To get detailed help, please enter: ./osmconvert -h\n");
return 0;  // end the program, because without having parameters
      // we do not know what to do;
    }
  while(--argc>0) {  // for every parameter in command line
    argv++;  // switch to next parameter; as the first one is just
      // the program name, we must do this previous reading the
      // first 'real' parameter;
    if(strcmp(argv[0],"-h")==0 || strcmp(argv[0],"-help")==0) {
        // user wants help text
      fprintf(stderr,"%s",helptext);  // print help text
        // (took "%s", to prevent oversensitive compiler reactions)
return 0;
      }
    if(strzcmp(argv[0],"--drop-his")==0) {
        // user does not want history information in standard output
      global_drophistory= true;
  continue;  // take next parameter
      }
    if(strzcmp(argv[0],"--drop-bro")==0) {
        // user does not want broken references in standard output
      global_dropbrokenrefs= true;
  continue;  // take next parameter
      }
    if(strzcmp(argv[0],"--drop-nod")==0) {
        // user does not want nodes section in standard output
      global_dropnodes= true;
  continue;  // take next parameter
      }
    if(strzcmp(argv[0],"--drop-way")==0) {
        // user does not want ways section in standard output
      global_dropways= true;
  continue;  // take next parameter
      }
    if(strzcmp(argv[0],"--drop-rel")==0) {
        // user does not want relations section in standard output
      global_droprelations= true;
  continue;  // take next parameter
      }
    if(strzcmp(argv[0],"--merge-dup")==0) {
        // user wants duplicate objects in input file to be merged
      global_mergeduplicates= true;
  continue;  // take next parameter
      }
    if(strcmp(argv[0],"--out-o5m")==0 ||
        strcmp(argv[0],"-5")==0) {
        // user wants output in o5m format
      global_outo5m= true;
  continue;  // take next parameter
      }
    if(strcmp(argv[0],"--out-o5c")==0 ||
        strcmp(argv[0],"-5c")==0) {
        // user wants output in o5m format
      global_outo5m= global_outo5c= true;
  continue;  // take next parameter
      }
    if(strcmp(argv[0],"--out-osm")==0) {
        // user wants output in osm format
      // this is default anyway, hence ignore this parameter
  continue;  // take next parameter
      }
    if(strcmp(argv[0],"--out-osc")==0) {
        // user wants output in osc format
      global_outosc= true;
  continue;  // take next parameter
      }
    if(strzcmp(argv[0],"--emulate-o")==0) {
        // emulate osmosis compatible output
      global_emulateosmosis= true;
  continue;  // take next parameter
      }
    if(strzcmp(argv[0],"--emulate-p")==0) {
        // emulate pbf2osm compatible output
      global_emulatepbf2osm= true;
  continue;  // take next parameter
      }
    if(strzcmp(argv[0],"-t=")==0 && argv[0][3]!=0) {
        // user-defined prefix for names of temorary files
      global_tempfilename= argv[0]+3;
  continue;  // take next parameter
      }
    if(strzcmp(argv[0],"-v")==0) {
        // test mode
      if(argv[0][2]=='=')
        loglevel= argv[0][3]-'0';
      else
        loglevel= argv[0][2]-'0';
      if(loglevel<1) loglevel= 1;
      if(loglevel>MAXLOGLEVEL) loglevel= MAXLOGLEVEL;
      if(loglevel==1)
        fprintf(stderr,"osmconvert: Verbose mode.\n");
      else
        fprintf(stderr,"osmconvert: Verbose mode %i.\n",loglevel);
  continue;  // take next parameter
      }
    if(strcmp(argv[0],"-t")==0) {
        // test mode
      write_testmode= true;
      fprintf(stderr,"osmconvert: Entering test mode.\n");
  continue;  // take next parameter
      }
    if(strzcmp(argv[0],"-h=")==0 && isdig(argv[0][3])) {
        // "-h=...": user wants a specific hash size;
      const char* p;

      p= argv[0]+3;  // jump over "-h="
      h_n= h_w= h_r= 0;
      // read the up to three values for hash tables' size;
      // format examples: "-h=200-20-10", "-h=1200"
      while(isdig(*p)) { h_n= h_n*10+*p-'0'; p++; }
      if(*p!=0) { p++; while(isdig(*p)) { h_w= h_w*10+*p-'0'; p++; } }
      if(*p!=0) { p++; while(isdig(*p)) { h_r= h_r*10+*p-'0'; p++; } }
  continue;  // take next parameter
      }
    if(strzcmp(argv[0],"-b=")==0) {
        // border consideration by a bounding box
      if(!border_box(argv[0]+3)) {
        fprintf(stderr,"osmconvert Error: use border format: "
          " -b\"x1,y1,x2,y2\"\n");
return 3;
        }  // end   border consideration by a bounding box
      continue;  // take next parameter
      }
    if(strzcmp(argv[0],"-B=")==0) {
        // border consideration by polygon file
      if(!border_file(argv[0]+3)) {
        fprintf(stderr,
          "osmconvert Error: no polygon file or too large: %s\n",
          argv[0]);
return 4;
        }  // end   border consideration by polygon file
  continue;  // take next parameter
      }
    if(strcmp(argv[0],"-")==0) {  // use standard input
      if(oo_open(NULL))  // file cannot be read
return 1;
  continue;  // take next parameter
      }
    // here: parameter must be a file name ,,,
    if(oo_open(argv[0]))  // file cannot be read
return 1;
    }  // end   for every parameter in command line

  // process parameters
  if(oo_ifn==0) {  // no input files given
    PERR("use \"-\" to read from standard input or try:  osmconvert -h")
return 0;  // end the program, because without having input files
      // we do not know what to do;
    }
  if(border_active || global_dropbrokenrefs) {  // user wants borders
    int r;

    if(h_n==0) h_n= 400;  // use standard value if not set otherwise
    if(h_w==0 && h_r==0) {
        // user chose simple form for hash memory value
      // take the one given value as reference and determine the 
      // three values using these factors: 80%, 15%, 5%
      h_w= h_n/5; h_r= h_n/20;
      h_n-= h_w; h_w-= h_r; }
    r= hash_ini(h_n,h_w,h_r);  // initialize hash table
    if(r==1)
      fprintf(stderr,"osmconvert: Hash size had to be reduced.\n");
    else if(r==2)
      fprintf(stderr,"osmconvert: Not enough memory for hash.\n");
    }  // end   user wants borders
  if(global_outo5m || border_active || global_dropbrokenrefs) {
      // .o5m format is needed as output
    if(o5_ini()!=0) {
      fprintf(stderr,"osmconvert: Not enough memory for .o5m buffer.\n");
return 5;
      }
    }  // end

  // do the work
  r= oo_main();
  if(loglevel>=2) {  // verbose
    fprintf(stderr,"osmconvert: Number of bytes read: %Ld\n",
      read_count());
    if(read_bufp!=NULL && read_bufp<read_bufe)
      fprintf(stderr,"osmconvert: Next bytes to parse:\n"
        "  %.02X %.02X %.02X %.02X %.02X %.02X %.02X %.02X\n",
        read_bufp[0],read_bufp[1],read_bufp[2],read_bufp[3],
        read_bufp[4],read_bufp[5],read_bufp[6],read_bufp[7]);
    }  // verbose
  write_flush();
  if(hash_queryerror()!=0)
    r= 91;
  if(loglevel>0) {  // verbose mode
    if(oo_sequenceid!=INT64_C(-0x7fffffffffffffff))
      fprintf(stderr,"osmconvert: Last processed: %s %Ld.\n",
        ONAME(oo_sequencetype),oo_sequenceid);
    if(r!=0)
      fprintf(stderr,"osmconvert Exit: %i\n",r);
    }  // verbose mode
  return r;
  }  // end   main()

