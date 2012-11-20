//------------------------------------------------------------
// Module oo_   osm to osm module
//------------------------------------------------------------
#include "stdinc.hpp"
#include "obj_relref.hpp"
#include "osm_hash.hpp"
#include "osm_border.h"
#include "oo.h"
#include "read.hpp"
#include "str.hpp"
#include <vector>
#include "process.hpp"
#include "util.h"

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
  int64_t relid;  // relation id;
  int64_t refid;  // interrelation reference id;
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

 byte oo__whitespace[]= {
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
 byte oo__whitespacenul[]= {
  1,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,  // NUL HT LF VT FF CR
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,  // SPC /
  0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,  // <
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
 byte oo__letter[]= {
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

 const uint8_t* oo__hexnumber= (uint8_t*)
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

 inline uint32_t oo__strtouint32(const char* s) {
  // read a number and convert it to an unsigned 32-bit integer;
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
 inline int32_t oo__strtosint32(const char* s) {
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

 inline int64_t oo__strtosint64(const char* s) {
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

 const int32_t oo__nildeg= 2000000000L;

 inline int32_t oo__strtodeg(char* s) {
  // read a number which represents a degree value and
  // convert it to a fixpoint number;
  // s[]: string with the number between -180 and 180,
  //      e.g. "-179.99", "11", ".222";
  // return: number in 10 millionth degrees;
  //         =='oo__nildeg': syntax error;
   const long di[]= {10000000L,10000000L,1000000L,100000L,
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

int64_t oo__strtimetosint64(const char* s) {
  // read a timestamp in OSM format, e.g.: "2010-09-30T19:23:30Z",
  // and convert it to a signed 64-bit integer;
  // also allowed: relative time to NOW, e.g.: "NOW-86400",
  // which means '24 hours ago';
  // return: time as a number (seconds since 1970);
  //         ==0: syntax error;
  if(s[0]=='N') {  // presumably a relative time to 'now'
    if(s[1]!='O' || s[2]!='W' || (s[3]!='+' && s[3]!='-') ||
        !isdig(s[4]))  // wrong syntax
return 0;
    s+= 3;  // jump over "NOW"
    if(*s=='+') s++;  // jump over '+', if any
return time(NULL)+oo__strtosint64(s);
    }  // presumably a relative time to 'now'
  if((s[0]!='1' && s[0]!='2') ||
      !isdig(s[1]) || !isdig(s[2]) || !isdig(s[3]) ||
      s[4]!='-' || !isdig(s[5]) || !isdig(s[6]) ||
      s[7]!='-' || !isdig(s[8]) || !isdig(s[9]) ||
      s[10]!='T' || !isdig(s[11]) || !isdig(s[12]) ||
      s[13]!=':' || !isdig(s[14]) || !isdig(s[15]) ||
      s[16]!=':' || !isdig(s[17]) || !isdig(s[18]) ||
      s[19]!='Z')  // wrong syntax
return 0;
  /* regular timestamp */ {
    struct tm tm;

    tm.tm_isdst= 0;
    tm.tm_year=
      (s[0]-'0')*1000+(s[1]-'0')*100+(s[2]-'0')*10+(s[3]-'0')-1900;
    tm.tm_mon= (s[5]-'0')*10+s[6]-'0'-1;
    tm.tm_mday= (s[8]-'0')*10+s[9]-'0';
    tm.tm_hour= (s[11]-'0')*10+s[12]-'0';
    tm.tm_min= (s[14]-'0')*10+s[15]-'0';
    tm.tm_sec= (s[17]-'0')*10+s[18]-'0';
    #if __WIN32__
    // use replcement for timegm() because Windows does not know it
      #if 0
      if(original_timezone==&original_timezone_none) {
        original_timezone= getenv("TZ");
        putenv("TZ=");
        tzset();
        }
      #endif
    //DPv(timezone %lli,timezone)
return mktime(&tm)-timezone;
    #else
return timegm(&tm);
    #endif
    }  // regular timestamp
  }  // end   oo__strtimetosint64()

 void oo__xmltostr(char* s) {
  // read an xml string and convert is into a regular UTF-8 string,
  // for example: "Mayer&apos;s" -> "Mayer's";
  char* t;  // pointer in overlapping target string
  char c;
  uint32_t u;

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
        *t++= ((u>>18)&0x07)|0xf0; *t++= ((u>>12)&0x3f)|0x80;
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

 bool oo__xmltag(Read & read) {
  // read the next xml key/val and return them both;
  // due to performance reasons, global and module global variables
  // are used;
  // read_bufp: address at which the reading begins;
  // oo__xmlheadtag: see above;
  // return: no more xml keys/vals to read inside the outer xml tag;
  // oo__xmlkey,oo__xmlval: newest xml key/val which have been read;
  //                        "","": encountered the end of an
  //                               enclosed xml tag;
  char xmldelim;
  char c;

  for(;;) {  // until break
    while(!oo__wsnul(*read.bufp())) read.bufpinc();
      // find next whitespace or null character or '/'
    while(oo__ws(*read.bufp())) read.bufpinc();
      // find first character after the whitespace(s)
    c= *read.bufp();
    if(c==0) {
      oo__xmlkey= oo__xmlval= (char*)"";
return true;
      }
    else if(c=='/') {
      oo__xmlkey= oo__xmlval= (char*)"";
      c= *read.bufp();
      read.bufpinc();
      read.bufpinc();
      if(c=='>') {  // short tag ends here
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
      read.bufpinc();
      if(*read.bufp()=='/' && (
			       (c= *read.bufp())=='n' || c=='w' || c=='r') ) {
        // this has been long tag which is ending now
        while(!oo__wsnul(*read.bufp())) read.bufpinc();
          // find next whitespace
        oo__xmlkey= oo__xmlval= (char*)"";
return true;
        }
  continue;
      }
    oo__xmlkey= (char*)read.bufp();
    while(oo__le(*read.bufp())) read.bufpinc();
    if(*read.bufp()!='=') {
      oo__xmlkey= (char*)"";
  continue;
      }
    *read.bufpinc()= 0;
    read.bufpsetnull();
    if(*read.bufp()!='\"' && *read.bufp()!='\'')
  continue;
    xmldelim= (char)*read.bufp();
    oo__xmlval= (char*)(read.bufpinc());
    for(;;) {
      c= *read.bufp();
      if(c==xmldelim)
    break;
      if(c==0) {
	oo__xmlkey= oo__xmlval= (char*)"";
return true;
        }
      read.bufpinc();
      }
    read.bufpsetnull();
  break;
    }  // end   until break
  oo__xmltostr(oo__xmlkey);
  oo__xmltostr(oo__xmlval);
  return false;
  }  // end   oo__xmltag()

static int oo__error= 0;  // error number which will be returned when
  // oo_main() terminates normal;


oo__if_vt oo__if;
static oo__if_vt::iterator oo__ifp= oo__if.begin();  // currently used element in oo__if[]
#define oo__ifI (oo__ifp-oo__if)  // index
static oo__if_vt::iterator oo__ife= oo__if.begin();  // logical end of elements in oo__if[]
static oo__if_vt::iterator oo__ifee= oo__if.end();
  // physical end of oo_if[]
//static int oo_ifn= 0;  // number of currently open files

static bool oo__bbvalid= false;
  // the following bbox coordinates are valid;
static int32_t oo__bbx1= 0,oo__bby1= 0,oo__bbx2= 0,oo__bby2= 0;
  // bbox coordinates (base 10^-7) of the first input file;
static int64_t oo__timestamp= 0;
  // file timestamp of the last input file which has a timestamp;
  // ==0: no file timestamp given;
static bool oo__alreadyhavepbfobject= false;

static void oo__findbb(Process * global, Read & read) {
  // find timestamp and border box in input file;
  // return:
  // oo__bbvalid: following border box information is valid;
  // oo__bbx1 .. oo__bby2: border box coordinates;

  // read_bufp will not be changed;
  byte* bufp,*bufe;

  read_input();
  bufp= (byte*)read.bufp(); 
  bufe= (byte*)read.bufe();
  if(oo__ifp->format==0) {  // o5m
    byte b;  // latest byte which has been read
    int l;

    while(bufp<bufe) {  // for all bytes
      b= *bufp;
      if(b==0 || (b>=0x10 && b<=0x12))  // regular dataset id
return;
      if(b>=0xf0) {  // single byte dataset
        bufp++;
    continue;
        }  // end   single byte dataset
      // here: non-object multibyte dataset
      if(b==0xdc) {
          // timestamp
        bufp++;
        l= pbf_uint32(&bufp);
        bufe= bufp+l;
        if(bufp<bufe) oo__timestamp= pbf_sint64(&bufp);
        bufp= bufe;
    continue;
        }  // timestamp
      if(b==0xdb && oo__ifp==oo__if.begin()) {
          // border box AND first input file
        bufp++;
        l= pbf_uint32(&bufp);
        bufe= bufp+l;
        if(bufp<bufe) oo__bbx1= pbf_sint32(&bufp);
        if(bufp<bufe) oo__bby1= pbf_sint32(&bufp);
        if(bufp<bufe) oo__bbx2= pbf_sint32(&bufp);
        if(bufp<bufe) {
          oo__bby2= pbf_sint32(&bufp);
          oo__bbvalid= true;
          }
        bufp= bufe;
    continue;
        }  // border box
      bufp++;
      l= pbf_uint32(&bufp);  // jump over this dataset
      bufp+= l;  // jump over this dataset
      }  // end   for all bytes
    }  // end   o5m
  else if(oo__ifp->format>0) {  // osm xml
    char* sp;
    char c1,c2,c3;  // next available characters

    while(bufp<bufe) {  // for all bytes
      sp= strchr((char*)bufp,'<');
      if(sp==NULL)
    break;
      c1= sp[1]; c2= sp[2]; c3= sp[3];
      if(c1=='n' && c2=='o' && c3=='d')
return;
      else if(c1=='w' && c2=='a' && c3=='y')
return;
      else if(c1=='r' && c2=='e' && c3=='l')
return;
      else if(c1=='o' && c2=='s' && c3=='m') {
          // "<osm"
        // timestamp must be supplied in this format:
        // <osm version="0.6" generator="OpenStreetMap planet.c"
        // timestamp="2010-08-18T00:11:04Z">
        int l;
        char c;

        sp++;  // jump over '<'
        if(strzcmp(sp,"osmAugmentedDiff")==0)
          global->mergeversions= true;
        for(;;) {  // jump over "osm ", "osmChange ", "osmAugmentedDiff"
          c= *sp;
          if(oo__wsnul(c))
        break;
          sp++;
          }
        for(;;) {  // for every word in 'osm'
          c= *sp;
          if(c=='/' || c=='<' || c=='>' || c==0)
        break;
          if(oo__ws(c)) {
            sp++;
        continue;
            }
          if((l= strzlcmp(sp,"timestamp="))>0 &&
              (sp[10]=='\"' || sp[10]=='\'') && isdig(sp[11])) {
            sp+= l+1;
            oo__timestamp= oo__strtimetosint64(sp);
            }
          for(;;) {  // find next whitespace or '<'
            c= *sp;
            if(oo__wsnul(c))
          break;
            sp++;
            }
          }  // end   for every word in 'osm'
        bufp++;
    continue;
        }  // "<osm"
      else if(c1=='b' && c2=='o' && c3=='u'  && oo__ifp==oo__if.begin()) {
          // bounds AND first input file
        // bounds may be supplied in one of these formats:
        // <bounds minlat="53.01104" minlon="8.481593"
        //   maxlat="53.61092" maxlon="8.990601"/>
        // <bound box="49.10868,6.35017,49.64072,7.40979"
        //   origin="http://www.openstreetmap.org/api/0.6"/>
        uint32_t bboxcomplete;  // flags for oo__bbx1 .. oo__bby2
        int l;
        char c;

        bboxcomplete= 0;
        sp++;  // jump over '<'
        for(;;) {  // jump over "bounds ", resp. "bound "
          c= *sp;
          if(oo__wsnul(c))
        break;
          sp++;
          }
        for(;;) {  // for every word in 'bounds'
          c= *sp;
          if(c=='/' || c=='<' || c=='>' || c==0)
        break;
          if(oo__ws(c) || c==',') {
            sp++;
        continue;
            }
          if((l= strzlcmp(sp,"box=\""))>0 ||
              (l= strzlcmp(sp,"box=\'"))>0) {
            sp+= l;
            c= *sp;
            }
          if((l= strzlcmp(sp,"minlat=\""))>0 ||
              (l= strzlcmp(sp,"minlat=\'"))>0 ||
              ((isdig(c) || c=='-' || c=='.') && (bboxcomplete&2)==0)) {
            sp+= l;
            oo__bby1= oo__strtodeg(sp);
            if(oo__bby1!=oo__nildeg) bboxcomplete|= 2;
            }
          else if((l= strzlcmp(sp,"minlon=\""))>0 ||
              (l= strzlcmp(sp,"minlon=\'"))>0 ||
              ((isdig(c) || c=='-' || c=='.') && (bboxcomplete&1)==0)) {
            sp+= l;
            oo__bbx1= oo__strtodeg(sp);
            if(oo__bbx1!=oo__nildeg) bboxcomplete|= 1;
            }
          else if((l= strzlcmp(sp,"maxlat=\""))>0 ||
              (l= strzlcmp(sp,"maxlat=\'"))>0 ||
              ((isdig(c) || c=='-' || c=='.') && (bboxcomplete&8)==0)) {
            sp+= l;
            oo__bby2= oo__strtodeg(sp);
            if(oo__bby2!=oo__nildeg) bboxcomplete|= 8;
            }
          else if((l= strzlcmp(sp,"maxlon=\""))>0 ||
              (l= strzlcmp(sp,"maxlon=\'"))>0 ||
              ((isdig(c) || c=='-' || c=='.') && (bboxcomplete&4)==0)) {
            sp+= l;
            oo__bbx2= oo__strtodeg(sp);
            if(oo__bbx2!=oo__nildeg) bboxcomplete|= 4;
            }
          for(;;) {  // find next blank or comma
            c= *sp;
            if(oo__wsnul(c) || c==',')
          break;
            sp++;
            }
          }  // end   for every word in 'bounds'
        oo__bbvalid= bboxcomplete==15;
        bufp++;
    continue;
        }  // bounds
      else {
        bufp++;
    continue;
        }
      }  // end   for all bytes of the file
    }  // end   osm xml
  else if(oo__ifp->format==-1) {  // pbf
    //pb_input();
    if(read.pbf.type == PBF::pbf_type && oo__ifp==oo__if.begin()) {
        // pbf header AND first input file
      oo__bbx1= read.pbf.bbx1; 
      oo__bby1= read.pbf.bby1;
      oo__bbx2= read.pbf.bbx2; 
      oo__bby2= read.pbf.bby2;

      oo__bbvalid= read.pbf.bbvalid;
      if(read.pbf.filetimestamp!=0)
        oo__timestamp= read.pbf.filetimestamp;
      }  // end   pbf header
    else
      oo__alreadyhavepbfobject= true;
    }  // end   pbf
  }  // end   oo__findbb()

int oo__gettyid(Process & global, Read & read) {
  // get tyid of the next object in the currently processed input file;
  // tyid is a combination of object type and id: we take the id and
  // add UINT64_C(0x0800000000000000) for nodes,
  // UINT64_C(0x1800000000000000) for ways, and
  // UINT64_C(0x2800000000000000) for relations;
  // if global_diff is set, besides tyid the hisver is retrieved too;
  // oo__ifp: handle of the currently processed input file;
  // return: ==0: ok; !=0: could not get tyid because starting object
  //         is not an osm object;
  // oo__ifp->tyid: tyid of the starting osm object;
  //                if there is not an osm object starting at
  //                read_bufp, *iyidp remains unchanged;
  // oo__ifp->hisver: only if global_diff; version of next object;
  static const uint64_t idoffset[]= {UINT64_C(0x0800000000000000),
    UINT64_C(0x1800000000000000),UINT64_C(0x2800000000000000),
    0,0,0,0,0,0,0,0,0,0,0,0,0,UINT64_C(0x0800000000000000),
    UINT64_C(0x1800000000000000),UINT64_C(0x2800000000000000)};
  int format;

  format= oo__ifp->format;
  if(format==0) {  // o5m
    int64_t o5id;
    byte* p,b;
    int l;

    o5id= oo__ifp->o5id;
    p= read.bufpb();
    while(p<read.bufeb()) {
      b= *p++;
      if(b>=0x10 && b<=0x12) {  // osm object is starting here
        oo__ifp->tyid= idoffset[b];
        read.pbf.intjump(&p);  // jump over length information
        oo__ifp->tyid+= o5id+pbf_sint64(&p);
        if(global.diff)
          oo__ifp->hisver= pbf_uint32(&p);
return 0;
        }
      if(b>=0xf0) {  // single byte
        if(b==0xff)  // this is an o5m Reset object
          o5id= 0;
    continue;
        }
      // here: unknown o5m object
      l= pbf_uint32(&p);  // get length of this object
      p+= l;  // jump over this object;
      }
return 1;
    }
  else if(format>0) {  // 10: osm xml
    char* s;
    uint64_t r;

    s= (char*)read_bufp;
    for(;;) {  // for every byte in XML object
      while(*s!='<' && *s!=0) s++;
      if(*s==0)
    break;
      s++;
      if(*s=='n' && s[1]=='o') r= UINT64_C(0x0800000000000000);
      else if(*s=='w'&& s[1]=='a') r= UINT64_C(0x1800000000000000);
      else if(*s=='r'&& s[1]=='e') r= UINT64_C(0x2800000000000000);
      else
    continue;
      do {
        s++;
        if(*s==0)
    break;
        } while(*s!=' ');
      while(*s==' ') s++;
      if(s[0]=='i' && s[1]=='d' && s[2]=='=' &&
          (s[3]=='\"' || s[3]=='\'')) {  // found id
        oo__ifp->tyid= r+oo__strtosint64(s+4);
        if(!global.diff)
return 0;
        oo__ifp->hisver= 0;
        for(;;) {
          if(*s=='>' || *s==0)
return 0;
          if(s[0]==' ' && s[1]=='v' && s[2]=='e' && s[3]=='r' &&
              s[4]=='s' && s[5]=='i' && s[6]=='o' && s[7]=='n' &&
              s[8]=='=' && (s[9]=='\"' || s[9]=='\'') && isdig(s[10])) {
              // found version
            oo__ifp->hisver= oo__strtouint32(s+10);
return 0;
            }
          s++;
          }
return 0;
        }  // end  found id
      }  // end   for every byte in XML object
return 1;
    }
  else if(format==-1) {  // pbf
#if 1  //,,,
    while(read.pbf.type> PBF::osm_type) {  // not an OSM object
      read.pbf.input(false);
      oo__alreadyhavepbfobject= true;
      }
#endif
    if((read.pbf.type & PBF::osm_bit)!= read.pbf.type)  // still not an osm object
return 1;
    oo__ifp->tyid= idoffset[read.pbf.type]+read.pbf.id;
    oo__ifp->hisver= read.pbf.hisver;
return 0;
    }
return 2;  // (unknown format)
  }  // end   oo__gettyid()

 int oo__getformat(Process & global, Read & read) {
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
  oo__if_vt::iterator ifptemp= oo__ifp;
  byte* bufp;
  #define bufsp ((char*)bufp)  // for signed char

  oo__if_vt::iterator oo__ifp= oo__if.begin();
  while(oo__ifp<oo__ife) {  // for all input files
    if(oo__ifp->ri!=NULL && oo__ifp->format==-9) {
        // format not yet determined
      read.read_switch(oo__ifp->ri);
      if(read.bufp()>=read.bufe()) {  // file empty
        PERRv("file empty: %.80s",oo__ifp->filename)
return 2;
        }
      bufp= read.bufpb();
      if(bufp[0]==0 && bufp[1]==0 && bufp[2]==0 &&
          bufp[3]>8 && bufp[3]<20) {  // presumably .pbf format
        if(++pbffilen>1) {   // pbf
          PERR("more than one .pbf input file is not allowed.");
return 5;
          }
        oo__ifp->format= -1;
        read.pbf.ini();
        read.pbf.input(false);
        oo__alreadyhavepbfobject= true;
        }
      else if(strzcmp(bufsp,"<?xml")==0 ||
          strzcmp(bufsp,"<osm")==0) {  // presumably .osm format
        oo__ifp->format= 10;
        }
      else if(bufp[0]==0xff && bufp[1]==0xe0 && (
          strzcmp(bufsp+2,"\x04""o5m2")==0 ||
          strzcmp(bufsp+2,"\x04""o5c2")==0 )) {
            // presumably .o5m format
        oo__ifp->format= 0;
        oo__ifp->str= str_open();
          // call some initialization of string read module
        }
      else if((bufp[0]==0xff && bufp[1]>=0x10 && bufp[1]<=0x12) ||
          (bufp[0]==0xff && bufp[1]==0xff &&
          bufp[2]>=0x10 && bufp[2]<=0x12) ||
	      (bufp[0]==0xff && read.bufe()==read.bufp()+1)) {
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
      oo__findbb(&global,read);
      oo__ifp->tyid= 0;
      oo__ifp->hisver= 0;
      oo__gettyid(global,read);
        // initialize tyid of the currently used input file
      }  // format not yet determined
    oo__ifp++;
    }  // for all input files
  oo__ifp= ifptemp;
  if(loglevel>0 && oo__timestamp!=0) {
    char s[30];  // timestamp as string

    write_createtimestamp(oo__timestamp,s);
    fprintf(stderr,"osmconvert: File timestamp: %s\n",s);
    }
  if(global.timestamp!=0)  // user wants a new file timestamp
    oo__timestamp= global.timestamp;
  return 0;
  #undef bufsp
  }  // end oo__getformat()

static uint64_t oo__tyidold= 0;  // tyid of the last written object;

 void oo__switch(Process & global, Read & read) {
  // determine the input file with the lowest tyid
  // and switch to it
   oo__if_vt::iterator ifp,ifpmin;
  uint64_t tyidmin,tyidold,tyid;

  // update tyid of the currently used input file and check sequence
  if(oo__ifp!=oo__if.begin()) {  // handle of current input file is valid
    tyidold= oo__ifp->tyid;
    if(oo__gettyid(global, read)==0) {  // new tyid is valid
//DPv(got   %llx %s,oo__ifp->tyid,oo__ifp->filename)
      if(oo__ifp->tyid<tyidold) {  // wrong sequence
        int64_t id; int ty;

        oo__error= 91;
        ty= tyidold>>60;
        id= ((int64_t)(tyidold & UINT64_C(0xfffffffffffffff)))-
          INT64_C(0x800000000000000);
        WARNv("wrong order at %s %Ld in file %s",
	      ONAME(ty),id,oo__ifp->filename);
        ty= oo__ifp->tyid>>60;
        id= ((int64_t)(oo__ifp->tyid & UINT64_C(0xfffffffffffffff)))-
          INT64_C(0x800000000000000);
        WARNv("next object is %s %Ld",ONAME(ty),id);
        }  // wrong sequence
      }  // new tyid is valid
    }  // end   handle of current input file is valid

  // find file with smallest tyid
  tyidmin= UINT64_C(0xffffffffffffffff);
  ifpmin= oo__ifp;
    // default; therefore we do not switch in cases we do not
    // find a minimum
  ifp= oo__ife;
  while(ifp != oo__if.begin()) {
    ifp--;
    if(ifp->ri!=NULL) {  // file handle is valid
//DPv(have  %llx %s,ifp->tyid,ifp->filename)
      tyid= ifp->tyid;
      if(tyid<tyidmin) {
        tyidmin= tyid;
        ifpmin= ifp;
        }
      }  // file handle valid
    }

  // switch to that file
  if(ifpmin!=oo__ifp) {
      // that file is not the file we're already reading from
    oo__ifp= ifpmin;
    read_switch(oo__ifp->ri);
    str_switch(oo__ifp->str);
    }
//DPv(chose %llx %s,oo__ifp->tyid,oo__ifp->filename)
  }  // end oo__switch()

//static int oo_sequencetype= -1;
  // type of last object which has been processed;
  // -1: no object yet; 0: node; 1: way; 2: relation;
//static int64_t oo_sequenceid= INT64_C(-0x7fffffffffffffff);
  // id of last object which has been processed;

void oo__reset(oo__if_vt::iterator ifp) {
  // perform a reset of output procedures and variables;
  // this is mandatory if reading .o5m or .pbf and jumping
  // within the input file;
  ifp->o5id= 0;
  ifp->o5lat= ifp->o5lon= 0;
  ifp->o5hiscset= 0;
  ifp->o5histime= 0;
  ifp->o5rid[0]= ifp->o5rid[1]= ifp->o5rid[2]= 0;
  str_reset();
  if(ifp->format==-1)
    pb_input(true);
  }  // oo__reset()

static int oo__rewindall(Process & global, Read & read) {
  // rewind all input files;
  // return: 0: ok; !=0: error;
  oo__if_vt::iterator ifp,ifp_sav;

  ifp_sav= oo__ifp;  // save original info pointer
  ifp= oo__if.begin();
  while(ifp<oo__ife) {
    if(ifp->riph!=NULL) {
      if(ifp->ri==NULL && ifp->riph!=NULL) {
          // file has been logically closed
        // logically reopen it
        ifp->ri= ifp->riph;
        //oo_ifn++;
	ifp++;
        }
      read_switch(ifp->ri);
      if(read_rewind())
return 1;
      ifp->tyid= 1;
      ifp->endoffile= false;
      ifp->deleteobject= 0;
      oo__reset(ifp);
      }
    ifp++;
    }
  oo__ifp= ifp_sav;  // restore original info pointer
  if(oo__ifp != oo__if.begin() && oo__ifp->ri!=NULL) {
    read_switch(oo__ifp->ri);
    str_switch(oo__ifp->str);
    }
  else
    oo__switch(global,read);
  oo__tyidold= 0;
  oo_sequencetype( -1);
  oo_sequenceid(INT64_C(-0x7fffffffffffffff));
  return 0;
  }  // end oo__rewindall()

static int oo__jumpall(Process & global, Read & read) {
  // jump in all input files to the previously stored position;
  // return: 0: ok; !=0: error;
  std::vector<oo__if_t>::iterator ifp;
  int r;

 std::vector<oo__if_t>::iterator  ifp_sav= oo__ifp;  // save original info pointer
 ifp= oo__if.begin();
  while(ifp<oo__ife) {  // for all files
    if(ifp->riph!=NULL) {  // file is still physically open
      if(ifp->ri==NULL && ifp->riph!=NULL) {
          // file has been logically closed
        // logically reopen it
        ifp->ri= ifp->riph;
        ifp++;
        }
      read_switch(ifp->ri);
      r= read_jump();
      if(r<0)  // jump error
return 1;
      if(r==0) {  // this was a real jump
        ifp->tyid= 1;
        ifp->endoffile= false;
        ifp->deleteobject= ifp->deleteobjectjump;
        oo__reset(ifp);
        }
      }  // file is still physically open
    ifp++;
    }  // for all files
  oo__ifp= ifp_sav;  // restore original info pointer
  if(
     (oo__ifp != oo__if.begin())  && 
     oo__ifp->ri!=NULL) {
    read_switch(oo__ifp->ri);
    str_switch(oo__ifp->str);
    }
  else {
    oo__switch(global,read);
    if(oo__ifp==oo__if.begin()) {  // no file chosen
      //      oo_ifn= 0;
      //ifp= oo__if;
      while(oo__ifp != oo__ife) {  // for all files
        oo__ifp->ri= NULL;  // mark file as able to be logically reopened
        oo__ifp++;
        }
      }
    }
  oo__tyidold= 0;
  oo_sequencetype(-1);
  oo_sequenceid(INT64_C(-0x7fffffffffffffff));
  return 0;
  }  // end oo__jumpall()

static void oo__close() {
  // logically close an input file;
  // oo__ifp: handle of currently active input file;
  // if this file has already been closed, nothing happens;
  // after calling this procedure, the handle of active input file
  // will be invalid; you may call oo__switch() to select the
  // next file in the sequence;
  if(oo__ifp!=NULL && oo__ifp->ri!=NULL) {
    if(!oo__ifp->endoffile  && oo_ifn>0)  // missing logical end of file
      fprintf(stderr,"osmconvert Warning: "
        "unexpected end of input file: %.80s\n",oo__ifp->filename);
    read_switch(oo__ifp->ri);
    //read_close();
    oo__ifp->ri= NULL;
    oo__ifp->tyid= UINT64_C(0xffffffffffffffff);
      // (to prevent this file being selected as next file
      // in the sequence)
    oo_ifn--;
    }
  oo__ifp= NULL;
  }  // end oo__close()

static void oo__closeall() {
  // close all input files;
  // after calling this procedure, the handle of active input file
  // will be invalid;
  oo_ifn= 0;  // mark end of program;
    // this is used to suppress warning messages in oo__close()
  while(oo__ife>oo__if) {
    oo__ifp= --oo__ife;
    oo__ifp->endoffile= true;  // suppress warnings (see oo__close())
    if(oo__ifp->riph!=NULL) {
      read_switch(oo__ifp->riph);
      read_close();
      }
    oo__ifp->ri= oo__ifp->riph= NULL;
    oo__ifp->tyid= UINT64_C(0xffffffffffffffff);
    }
  }  // end oo__closeall()

static void* oo__malloc_p[50];
  // pointers for controlled memory allocation
static int oo__malloc_n= 0;
  // number of elements used in oo__malloc_p[]

static void* oo__malloc(size_t size) {
  // same as malloc(), but the allocated memory will be
  // automatically freed at program end;
  void* mp;

  mp= malloc(size);
  if(mp==NULL) {
    PERRv("cannot allocate %Ld bytes of memory.",(int64_t)size);
    exit(1);
    }
  oo__malloc_p[oo__malloc_n++]= mp;
  return mp;
  }  // oo__malloc()

static void oo__end() {
  // clean-up this module;
  oo__closeall();
  while(oo__malloc_n>0)
    free(oo__malloc_p[--oo__malloc_n]);
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
    fprintf(stderr,"osmconvert Error: too many input files: %d>%d\n",
      (int)(oo__ife-oo__if),global_fileM);
return 2;
    }
  if(read_open(filename)!=0)
return 1;
  oo__ife->ri= oo__ife->riph= read_infop;
  oo__ife->str= NULL;
  oo__ife->format= -9;  // 'not yet determined'
  oo__ife->tyid= 0;
  if(filename==NULL)
    oo__ife->filename= "standard input";
  else
    oo__ife->filename= filename;
  oo__ife->endoffile= false;
  oo__ife->deleteobject= 0;
  oo__ifp= oo__ife++;
  oo_ifn++;
  if(firstrun) {
    firstrun= false;
    atexit(oo__end);
    }
  return 0;
  }  // end   oo_open()

int dependencystage;
  // stage of the processing of interobject dependencies:
  // interrelation dependencies, --complete-ways or --complex-ways;
  // processing in stages allows us to reprocess parts of the data;
  // abbrevation "ht" means hash table (module hash_);
  //
  // 0: no recursive processing at all;
  //
  // option --complex-ways:
  // 11:     no output;
  //         for each node which is inside the borders,
  //           set flag in ht;
  //         store start of ways in read_setjump();
  //         for each way which has a member with flag in ht,
  //           set the way's flag in ht;
  //         for each relation with a member with flag in ht,
  //           store the relation's flag and write the ids
  //           of member ways which have no flag in ht
  //           (use cww_);
  // 11->12: at all files' end:
  //         let all files jump to start of ways,
  //         use read_jump();
  //         set flags for ways, use cww_processing_set();
  // 12:     no output;
  //         for each way with a member with a flag in ht,
  //           set the way's flag in ht and write the ids
  //           of all the way's members (use cwn_);
  // 12->22: as soon as first relation shall be written:
  //         rewind all files;
  //         set flags for nodes, use cwn_processing();
  //
  // option --complete-ways:
  // 21:     no output;
  //         for each node inside the borders,
  //           set flag in ht;
  //         for each way with a member with a flag in ht,
  //           set the way's flag in ht and write the ids
  //           of all the way's members (use cwn_);
  // 21->22: as soon as first relation shall be written:
  //         rewind all files;
  //         set flags for nodes, use cwn_processing();
  // 22:     write each node which has a flag in ht to output;
  //         write each way which has a flag in ht to output;
  // 22->32: as soon as first relation shall be written:
  //         clear flags for ways, use cww_processing_clear();
  //         switch output to temporary file;
  //
  // regular procedure:
  // 31:     for each node inside the borders,
  //           set flag in ht;
  //         for each way with a member with a flag in ht,
  //           set the way's flag in ht;
  // 31->32: as soon as first relation shall be written:
  //         switch output to temporary .o5m file;
  // 32:     for each relation with a member with a flag
  //           in ht, set the relation's flag in ht;
  //         for each relation,
  //           write its id and its members' ids
  //           into a temporary file (use rr_);
  //         if option --all-to-nodes is set, then
  //           for each relation, write its members'
  //             geopositions into a temporary file (use posr_);
  // 32->33: at all files' end:
  //         process all interrelation references (use rr_);
  //         if option --all-to-nodes is set, then
  //           process position array (use posr_);
  //         switch input to the temporary .o5m file;
  //         switch output to regular output file;
  // 33:     write each relation which has a flag in ht
  //           to output; use temporary .o5m file as input;
  // 33->99: at all files' end: end this procedure;
  //
  // out-of-date:
  // 1: (this stage is applied only with --complete-ways option)
  //    read nodes and ways, do not write anything; change to next
  //    stage as soon as the first relation has been encountered;
  //    now: 21;
  // 1->2: at this moment, rewind all input files;
  //    now: 21->22;
  // 2: write nodes and ways, change to next stage as soon as
  //    the first relation has been encountered;
  //    now: 22 or 31;
  // 2->3: at this moment, change the regular output file to a
  //       tempfile, and switch output format to .o5m;
  //    now: 22->32 or 31->32;
  // 3: write interrelation references into a second to tempfile,
  //    use modules rr_ or posr_ for this purpose;
  //    now: 32;
  // 3->4: at this moment, change output back to standard output,
  //       and change input to the start of the tempfile;
  //       in addition to this, process temporarily stored
  //       interrelation data;
  //    now: 32->33;
  // 4: write only relations, use tempfile as input;
  //    now: 33;
static void oo__dependencystage(int ds) {
  // change the dependencystage;
  if(loglevel>=2)
    PINFOv2("changing dependencystage from %i to %i.",dependencystage,ds)
  dependencystage= ds;
  }  // oo__dependencystage()

static int oo_main() {
  // start reading osm objects;
  // return: ==0: ok; !=0: error;
  // this procedure must only be called once;
  // before calling this procedure you must open an input file
  // using oo_open();
  int wformat;  // output format;
    // 0: o5m; 11,12,13,14: some different XML formats;
    // 21: csv; -1: PBF;
  bool hashactive;
    // must be set to true if border_active OR global_dropbrokenrefs;
  static char o5mtempfile[400];  // must be static because
    // this file will be deleted by an at-exit procedure;
  #define oo__maxrewindINI 12
  int maxrewind;  // maximum number of relation-relation dependencies
  int maxrewind_posr;  // same as before, but for --all-to-nodes
  bool writeheader;  // header must be written
  int otype;  // type of currently processed object;
    // 0: node; 1: way; 2: relation;
  int64_t id;
  int32_t lon,lat;
  uint32_t hisver;
  int64_t histime;
  int64_t hiscset;
  uint32_t hisuid;
  char* hisuser;
  int64_t* refid;  // ids of referenced object
  int64_t* refidee;  // end address of array
  int64_t* refide,*refidp;  // pointer in array
  int32_t** refxy;  // coordinates of referenced object
  int32_t** refxyp;  // pointer in array
  byte* reftype;  // types of referenced objects
  byte* reftypee,*reftypep;  // pointer in array
  char** refrole;  // roles of referenced objects
  char** refrolee,**refrolep;  // pointer in array
  #define oo__keyvalM 8000  // changed from 4000 to 8000
    // because there are old ways with this many key/val pairs
    // in full istory planet due to malicious Tiger import
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
  struct {
    int64_t nodes,ways,relations;  // number of objects
    int64_t node_id_min,node_id_max;
    int64_t way_id_min,way_id_max;
    int64_t relation_id_min,relation_id_max;
    int64_t timestamp_min,timestamp_max;
    int32_t lon_min,lon_max;
    int32_t lat_min,lat_max;
    int32_t keyval_pairs_max;
    int keyval_pairs_otype;
    int64_t keyval_pairs_oid;
    int32_t noderefs_max;
    int64_t noderefs_oid;
    int32_t relrefs_max;
    int64_t relrefs_oid;
    } statistics;
  bool diffcompare;  // the next object shall be compared
    // with the object which has just been read;
  bool diffdifference;
    // there was a difference in object comparison

  // procedure initialization
  atexit(oo__end);
  memset(&statistics,0,sizeof(statistics));
  oo__bbvalid= false;
  hashactive= border_active || global_dropbrokenrefs;
  dependencystage= 0;  // 0: no recursive processing at all;
  maxrewind= maxrewind_posr= oo__maxrewindINI;
  writeheader= true;
  if(global_outo5m) wformat= 0;
  else if(global_outpbf) wformat= -1;
  else if(global_emulatepbf2osm) wformat= 12;
  else if(global_emulateosmosis) wformat= 13;
  else if(global_emulateosmium) wformat= 14;
  else if(global_outcsv) wformat= 21;
  else wformat= 11;
  refid= (int64_t*)oo__malloc(sizeof(int64_t)*global_maxrefs);
  refidee= refid+global_maxrefs;
  refxy= (int32_t**)oo__malloc(sizeof(int32_t*)*global_maxrefs);
  reftype= (byte*)oo__malloc(global_maxrefs);
  refrole= (char**)oo__malloc(sizeof(char*)*global_maxrefs);
  keyee= key+oo__keyvalM;
  diffcompare= false;
  diffdifference= false;

  // get input file format and care about tempfile name
  if(oo__getformat())
return 5;
  if((hashactive && !global_droprelations) ||
      global_alltonodes) {
      // (borders to apply AND relations are required) OR
      // user wants ways and relations to be converted to nodes
    // initiate recursive processing;
    if(global_complexways) {
      oo__dependencystage(11);
        // 11:     no output;
        //         for each node which is inside the borders,
        //           set flag in ht;
        //         store start of ways in read_setjump();
        //         for each way which has a member with flag in ht,
        //           set the way's flag in ht;
        //         for each relation with a member with flag in ht,
        //           store the relation's flag and write the ids
        //           of member ways which have no flag in ht
        //           (use cww_);
      if(cwn_ini(global_tempfilename))
return 28;
      if(cww_ini(global_tempfilename))
return 28;
      }
    else if(global_completeways) {
      oo__dependencystage(21);
        // 21:     no output;
        //         for each node inside the borders,
        //           set flag in ht;
        //         for each way with a member with a flag in ht,
        //           set the way's flag in ht and write the ids
        //           of all the way's members (use cwn_);
      if(cwn_ini(global_tempfilename))
return 28;
      }
    else
      oo__dependencystage(31);
        // 31:     for each node inside the borders,
        //           set flag in ht;
        //         for each way with a member with a flag in ht,
        //           set the way's flag in ht;
    strcpy(stpmcpy(o5mtempfile,global_tempfilename,
      sizeof(o5mtempfile)-2),".1");
    }
  else {
    oo__dependencystage(0);  // no recursive processing
    global_completeways= false;
    global_complexways= false;
    }

  // print file timestamp and nothing else if requested
  if(global_outtimestamp) {
    if(oo__timestamp!=0)  // file timestamp is valid
      write_timestamp(oo__timestamp);
    else
      write_str("(invalid timestamp)");
    write_str(NL);
return 0;  // nothing else to do here
    }

  // process the file
  for(;;) {  // read all input files

    if(oo_ifn>0) {  // at least one input file open

      // get next object - if .pbf
      //read_input(); (must not be here because of diffcompare)
      if(oo__ifp->format==-1) {
        if(!oo__alreadyhavepbfobject)
          pb_input(false);
        while(pb_type>2)  // unknown pbf object
          pb_input(false);  // get next object
        }

      // merging - if more than one file
      if((oo_ifn>1 || oo__tyidold>0) && dependencystage!=33)
          // input file switch necessary;
          // not:
          // 33:     write each relation which has a flag in ht
          //           to output;
        oo__switch();
      else if(global_mergeversions)
        oo__gettyid();
      else
        oo__ifp->tyid= 1;
      if(diffcompare && oo__ifp!=oo__if) {
          // comparison must be made with the first file but presently
          // the second file is active
        // switch to the first file
        oo__ifp= oo__if;
        read_switch(oo__ifp->ri);
        str_switch(oo__ifp->str);
        }

      // get next data
      read_input();

      }  // at least one input file open

    // care about end of input file
    if(oo_ifn==0 || (read_bufp>=read.bufe() && oo__ifp->format>=0) ||
        (oo__ifp->format==-1 && pb_type<0)) {  // at end of input file
      if(oo_ifn>0) {
        if(oo__ifp->format==-1 && pb_type<0) {
          if(pb_type<-1)  // error
return 1000-pb_type;
          oo__ifp->endoffile= true;
          }
        oo__close();
        }
      if(oo_ifn>0)  // still input files
        oo__switch();
      else {  // at end of all input files
        // care about recursive processing
        if(dependencystage==11) {
            // 11:     no output;
            //         for each node which is inside the borders,
            //           set flag in ht;
            //         store start of ways in read_setjump();
            //         for each way which has a member with flag in ht,
            //           set the way's flag in ht;
            //         for each relation with a member with flag in ht,
            //           store the relation's flag and write the ids
            //           of member ways which have no flag in ht
            //           (use cww_);
          // 11->12: at all files' end:
          //         let all files jump to start of ways,
          //         use read_jump();
          //         set flags for ways, use cww_processing_set();
          if(oo__jumpall())
return 28;
          cww_processing_set();
          oo__dependencystage(12);
            // 12:     no output;
            //         for each way with a member with a flag in ht,
            //           set the way's flag in ht and write the ids
            //           of all the way's members (use cwn_);
  continue;  // do not write this object
          }
        if(dependencystage==21 || dependencystage==12) {
            // 12:     no output;
            //         for each way with a member with a flag in ht,
            //           set the way's flag in ht and write the ids
            //           of all the way's members (use cwn_);
            // 21:     no output;
            //         for each node inside the borders,
            //           set flag in ht;
            //         for each way with a member with a flag in ht,
            //           set the way's flag in ht and write the ids
            //           of all the way's members (use cwn_);
          // 12->22: as soon as first relation shall be written:
          //         rewind all files;
          //         set flags for nodes, use cwn_processing();
          // 21->22: as soon as first relation shall be written:
          //         rewind all files;
          //         set flags for nodes, use cwn_processing();
          if(oo__rewindall())
return 28;
          cwn_processing();
          oo__dependencystage(22);
            // 22:     write each node which has a flag in ht to output;
            //         write each way which has a flag in ht to output;
  continue;  // do not write this object
          }
        if(dependencystage!=32) {
            // not:
            // 32:     for each relation with a member with a flag
            //           in ht, set the relation's flag in ht;
            //         for each relation,
            //           write its id and its members' ids
            //           into a temporary file (use rr_);
            //         if option --all-to-nodes is set, then
            //           for each relation, write its members'
            //             geopositions into a temporary file
            //             (use posr_);
          if(dependencystage==33) {
              // 33:     write each relation which has a flag in ht
              //           to output; use temporary .o5m file as input;
            if(oo__ifp!=NULL)
              oo__ifp->endoffile= true;
                // this is because the file we have read
                // has been created as temporary file by the program
                // and does not contain an eof object;
            if(maxrewind_posr<maxrewind) maxrewind= maxrewind_posr;
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
  break;
          }  // end   dependencystage!=32
        // here: dependencystage==32
        // 32:     for each relation with a member with a flag
        //           in ht, set the relation's flag in ht;
        //         for each relation,
        //           write its id and its members' ids
        //           into a temporary file (use rr_);
        //         if option --all-to-nodes is set, then
        //           for each relation, write its members'
        //             geopositions into a temporary file (use posr_);
        // 32->33: at all files' end:
        //         process all interrelation references (use rr_);
        //         if option --all-to-nodes is set, then
        //           process position array (use posr_);
        //         switch input to the temporary .o5m file;
        //         switch output to regular output file;
        if(!global_outnone) {
          wo_flush();
          wo_reset();
          wo_end();
          wo_flush();
          }
        if(write_newfile(NULL))
return 21;
        if(!global_outnone) {
          wo_format(wformat);
          wo_reset();
          }
        if(hashactive)
          oo__rrprocessing(&maxrewind);
        if(global_alltonodes)
          posr_processing(&maxrewind_posr,refxy);
        oo__dependencystage(33);  // enter next stage
        oo__tyidold= 0;  // allow the next object to be written
        if(oo_open(o5mtempfile))
return 22;
        if(oo__getformat())
return 23;
        read_input();
  continue;
        }  // at end of all input files
      }  // at end of input file

    // care about unexpected contents at file end
    if(dependencystage<=31)
        // 31:     for each node inside the borders,
        //           set flag in ht;
        //         for each way with a member with a flag in ht,
        //           set the way's flag in ht;
    if(oo__ifp->endoffile) {  // after logical end of file
      WARNv("osmconvert Warning: unexpected contents "
        "after logical end of file: %.80s",oo__ifp->filename)
  break;
      }

    readobject:
    bufp= read_bufp;
    b= *bufp; c= (char)b;

    // care about header and unknown objects
    if(oo__ifp->format<0) {  // -1, pbf
      if(pb_type<0 || pb_type>2)  // not a regular dataset id
  continue;
      otype= pb_type;
      oo__alreadyhavepbfobject= false;
      }  // end   pbf
    else if(oo__ifp->format==0) {  // o5m
      if(b<0x10 || b>0x12) {  // not a regular dataset id
        if(b>=0xf0) {  // single byte dataset
          if(b==0xff) {  // file start, resp. o5m reset
            if(read_setjump())
              oo__ifp->deleteobjectjump= oo__ifp->deleteobject;
            oo__reset(oo__ifp);
            }
          else if(b==0xfe)
            oo__ifp->endoffile= true;
          else if(write_testmode)
            WARNv("unknown .o5m short dataset id: 0x%02x",b)
          read_bufp++;
  continue;
          }  // end   single byte dataset
        else {  // unknown multibyte dataset
          if(b!=0xdb && b!=0xdc && b!=0xe0)
              // not border box AND not header
            WARNv("unknown .o5m dataset id: 0x%02x",b)
          read_bufp++;
          l= pbf_uint32(&read_bufp);  // length of this dataset
          read_bufp+= l;  // jump over this dataset
  continue;
          }  // end   unknown multibyte dataset
        }  // end   not a regular dataset id
      otype= b&3;
      }  // end   o5m
    else {  // xml
      while(c!=0 && c!='<') c= (char)*++bufp;
      if(c==0) {
        read_bufp= read.bufe();
  continue;
        }
      c= bufsp[1];
      if(c=='n' && bufsp[2]=='o' && bufsp[3]=='d')  // node
        otype= 0;
      else if(c=='w' && bufsp[2]=='a')  // way
        otype= 1;
      else if(c=='r' && bufsp[2]=='e')  // relation
        otype= 2;
      else if(c=='c' || (c=='m' && bufsp[2]=='o') || c=='d' ||
          c=='i' || c=='k' || c=='e' ) {
          // create, modify or delete,
          // insert, keep or erase
        if(c=='d' || c=='e')
          oo__ifp->deleteobject= 2;
        read_bufp= bufp+1;
  continue;
        }   // end   create, modify or delete,
            // resp. insert, keep or erase
      else if(c=='/') {  // xml end object
        if(bufsp[2]=='d' || bufsp[2]=='e')  // end of delete or erase
          oo__ifp->deleteobject= 0;
        else if(strzcmp(bufsp+2,"osm>")==0) {  // end of file
          oo__ifp->endoffile= true;
          read_bufp= bufp+6;
          while(oo__ws(*read_bufp)) read_bufp++;
  continue;
          }   // end   end of file
        else if(strzcmp(bufsp+2,"osmChange>")==0) {  // end of file
          oo__ifp->endoffile= true;
          read_bufp= bufp+6+6;
          while(oo__ws(*read_bufp)) read_bufp++;
  continue;
          }   // end   end of file
        else if(strzcmp(bufsp+2,"osmAugmentedDiff>")==0) {
            // end of file
          oo__ifp->endoffile= true;
          read_bufp= bufp+6+13;
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
      // here: regular OSM XML object
      if(read_setjump())
        oo__ifp->deleteobjectjump= oo__ifp->deleteobject;
      read_bufp= bufp;
      }  // end   xml

    // write header
    if(writeheader) {
      writeheader= false;
      if(!global_outnone)
        wo_start(wformat,oo__bbvalid,
          oo__bbx1,oo__bby1,oo__bbx2,oo__bby2,oo__timestamp);
      }

    // object initialization
    if(!diffcompare) {  // regularly read the object
      hisver= 0;
      histime= 0;
      hiscset= 0;
      hisuid= 0;
      hisuser= "";
      refide= refid;
      reftypee= reftype;
      refrolee= refrole;
      keye= key;
      vale= val;
      }  // regularly read the object
    if(oo__ifp->deleteobject==1) oo__ifp->deleteobject= 0;


    // read one osm object
    if(oo__ifp->format<0) {  // pbf
      // read id
      id= pb_id;
      // read coordinates (for nodes only)
      if(otype==0) {  // node
        lon= pb_lon; lat= pb_lat;
        }  // node
      // read author
      hisver= pb_hisver;
      if(hisver!=0) {  // author information available
        histime= pb_histime;
        if(histime!=0) {
          hiscset= pb_hiscset;
          hisuid= pb_hisuid;
          hisuser= pb_hisuser;
          }
        }  // end   author information available
      oo__ifp->deleteobject= pb_hisvis==0? 1: 0;
      // read noderefs (for ways only)
      if(otype==1)  // way
        refide= refid+pb_noderef(refid,global_maxrefs);
      // read refs (for relations only)
      else if(otype==2) {  // relation
        l= pb_ref(refid,reftype,refrole,global_maxrefs);
        refide= refid+l;
        reftypee= reftype+l;
        refrolee= refrole+l;
        }  // end   relation
      // read node key/val pairs
      l= pb_keyval(key,val,oo__keyvalM);
      keye= key+l; vale= val+l;
      }  // end   pbf
    else if(oo__ifp->format==0) {  // o5m
      bufp++;
      l= pbf_uint32(&bufp);
      read_bufp= bufe= bufp+l;
      if(diffcompare) {  // just compare, do not store the object
        uint32_t hisverc;
        int64_t histimec;
        char* hisuserc;
        int64_t* refidc;  // pointer for object contents comparison
        byte* reftypec;  // pointer for object contents comparison
        char** refrolec;  // pointer for object contents comparison
        char** keyc,**valc;  // pointer for object contents comparison

        // initialize comparison variables
        hisverc= 0;
        histimec= 0;
        hisuserc= "";
        refidc= refid;
        reftypec= reftype;
        refrolec= refrole;
        keyc= key;
        valc= val;

        // compare object id
        if(id!=(oo__ifp->o5id+= pbf_sint64(&bufp)))
          diffdifference= true;

        // compare author
        hisverc= pbf_uint32(&bufp);
        if(hisverc!=hisver)
          diffdifference= true;
        if(hisverc!=0) {  // author information available
          histimec= oo__ifp->o5histime+= pbf_sint64(&bufp);
          if(histimec!=0) {
            if(histimec!=histime)
              diffdifference= true;
            if(hiscset!=(oo__ifp->o5hiscset+= pbf_sint32(&bufp)))
              diffdifference= true;
            str_read(&bufp,&sp,&hisuserc);
            if(strcmp(hisuserc,hisuser)!=0)
              diffdifference= true;
            if(hisuid!=pbf_uint64((byte**)&sp))
              diffdifference= true;
            }
          }  // end   author information available

        if(bufp>=bufe) {
            // just the id and author, i.e. this is a delete request
          oo__ifp->deleteobject= 1;
          diffdifference= true;
          }
        else {  // not a delete request
          oo__ifp->deleteobject= 0;

          // compare coordinates (for nodes only)
          if(otype==0) {  // node
            // read node body
            if(lon!=(oo__ifp->o5lon+= pbf_sint32(&bufp)))
              diffdifference= true;
            if(lat!=(oo__ifp->o5lat+= pbf_sint32(&bufp)))
              diffdifference= true;
            }  // end   node

          // compare noderefs (for ways only)
          if(otype==1) {  // way
            l= pbf_uint32(&bufp);
            bp= bufp+l;
            if(bp>bufe) bp= bufe;  // (format error)
            while(bufp<bp && refidc<refidee) {
              if(*refidc!=(oo__ifp->o5rid[0]+= pbf_sint64(&bufp)))
                diffdifference= true;
              refidc++;
              }
            }  // end   way

          // compare refs (for relations only)
          else if(otype==2) {  // relation
            int64_t ri;  // temporary, refid
            int rt;  // temporary, reftype
            char* rr;  // temporary, refrole

            l= pbf_uint32(&bufp);
            bp= bufp+l;
            if(bp>bufe) bp= bufe;  // (format error)
            while(bufp<bp && refidc<refidee) {
              ri= pbf_sint64(&bufp);
              str_read(&bufp,&rr,NULL);
              if(*reftypec!=(rt= (*rr++ -'0')%3))
                diffdifference= true;
              if(*refidc!=(oo__ifp->o5rid[rt]+= ri))
                diffdifference= true;
              if(refrolec>=refrolee || strcmp(*refrolec,rr)!=0)
                diffdifference= true;
              reftypec++;
              refidc++;
              refrolec++;
              }
            }  // end   relation

          // compare node key/val pairs
          while(bufp<bufe && keyc<keyee) {
            char* k,*v;

            k= v= "";
            str_read(&bufp,&k,&v);
            if(keyc>=keye || strcmp(k,*keyc)!=0 || strcmp(v,*valc)!=0)
              diffdifference= true;
            keyc++; valc++;
            }
          }  // end   not a delete request

        // compare indexes
        if(keyc!=keye || (otype>0 && refidc!=refide))
          diffdifference= true;

        }  // just compare, do not store the object
      else {  // regularly read the object
        // read object id
        id= oo__ifp->o5id+= pbf_sint64(&bufp);
        // read author
        hisver= pbf_uint32(&bufp);
        if(hisver!=0) {  // author information available
          histime= oo__ifp->o5histime+= pbf_sint64(&bufp);
          if(histime!=0) {
            hiscset= oo__ifp->o5hiscset+= pbf_sint32(&bufp);
            str_read(&bufp,&sp,&hisuser);
            hisuid= pbf_uint64((byte**)&sp);
            }
          }  // end   author information available
        if(bufp>=bufe) 
            // just the id and author, i.e. this is a delete request
          oo__ifp->deleteobject= 1;
        else {  // not a delete request
          oo__ifp->deleteobject= 0;
          // read coordinates (for nodes only)
          if(otype==0) {  // node
            // read node body
            lon= oo__ifp->o5lon+= pbf_sint32(&bufp);
            lat= oo__ifp->o5lat+= pbf_sint32(&bufp);
            }  // end   node
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
        }  // regularly read the object
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
          if(oo__xmlkey[0]=='i' && oo__xmlkey[1]=='d') // id
            id= oo__strtosint64(oo__xmlval);
          else if(oo__xmlkey[0]=='l') {  // letter l
            if(oo__xmlkey[1]=='o') // lon
              lon= oo__strtodeg(oo__xmlval);
            else if(oo__xmlkey[1]=='a') // lon
              lat= oo__strtodeg(oo__xmlval);
            }  // end   letter l
          else if(oo__xmlkey[0]=='v' && oo__xmlkey[1]=='i') {  // visible
            if(oo__xmlval[0]=='f' || oo__xmlval[0]=='n')
              if(oo__ifp->deleteobject==0)
                oo__ifp->deleteobject= 1;
            }  // end   visible
          else if(oo__xmlkey[0]=='a' && oo__xmlkey[1]=='c') {  // action
            if(oo__xmlval[0]=='d' && oo__xmlval[1]=='e')
              if(oo__ifp->deleteobject==0)
                oo__ifp->deleteobject= 1;
            }  // end   action
          else if(!global_dropversion) {  // version not to drop
            if(oo__xmlkey[0]=='v' && oo__xmlkey[1]=='e') // hisver
              hisver= oo__strtouint32(oo__xmlval);
            if(!global_dropauthor) {  // author not to drop
              if(oo__xmlkey[0]=='t') // histime
                histime= oo__strtimetosint64(oo__xmlval);
              else if(oo__xmlkey[0]=='c') // hiscset
                hiscset= oo__strtosint64(oo__xmlval);
              else if(oo__xmlkey[0]=='u' && oo__xmlkey[1]=='i') // hisuid
                hisuid= oo__strtouint32(oo__xmlval);
              else if(oo__xmlkey[0]=='u' && oo__xmlkey[1]=='s') //hisuser
                hisuser= oo__xmlval;
              }  // end   author not to drop
            }  // end   version not to drop
          }  // end   still in object header
        else {  // in object body
          if(oo__xmlkey[0]==0) {  // xml tag completed
            if(rcomplete>=3) {  // at least refid and reftype
              *refide++= ri;
              *reftypee++= rt;
              if(rcomplete<4)  // refrole is missing
                rr= "";  // assume an empty string as refrole
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
    if(global_mergeversions) {
        // user allows duplicate objects in input file; that means
        // we must take the last object only of each duplicate
        // because this is assumed to be the newest;
      uint64_t tyidold;

      tyidold= oo__ifp->tyid;
      if(oo__gettyid()==0) {
        if(oo__ifp->tyid==tyidold)  // next object has same type and id
          goto readobject;  // dispose this object and take the next
        }
      oo__ifp->tyid= tyidold;
      }

    // care about possible array overflows
    if(refide>=refidee)
      PERRv2("%s %Ld has too many refs.",ONAME(otype),id)
    if(keye>=keyee)
      PERRv2("%s %Ld has too many key/val pairs.",
        ONAME(otype),id)

    // care about diffs and sequence
    if(global_diffcontents) {  // diff contents is to be considered
      // care about identical contents if calculating a diff
      if(oo__ifp!=oo__if && oo__ifp->tyid==oo__if->tyid) {
          // second file and there is a similar object in the first file
          // and version numbers are different
        diffcompare= true;  // compare with next object, do not read
        diffdifference= false;  // (default)
  continue;  // no check the first file
        }
      }  // diff contents is to be considered
    else {  // no diff contents is to be considered
      // stop processing if object is to ignore because of duplicates
      // in same or other file(s)
      if(oo__ifp->tyid<=oo__tyidold)
  continue;
      oo__tyidold= 0;
      if(oo_ifn>1)
        oo__tyidold= oo__ifp->tyid;
      // stop processing if in wrong stage for nodes or ways
      if(dependencystage>=32 && otype<=1)
          // 32:     for each relation with a member with a flag
          //           in ht, set the relation's flag in ht;
          //         for each relation,
          //           write its id and its members' ids
          //           into a temporary file (use rr_);
          //         if option --all-to-nodes is set, then
          //           for each relation, write its members'
          //             geopositions into a temporary file (use posr_);
          // 33:     write each relation which has a flag in ht
          //           to output; use temporary .o5m file as input;
  continue;  // ignore this object
      // check sequence, if necessary
      if(oo_ifn==1 && dependencystage!=33) {
          // not:
          // 33:     write each relation which has a flag in ht
          //           to output; use temporary .o5m file as input;
        if(otype<=oo_sequencetype &&
            (otype<oo_sequencetype || id<oo_sequenceid ||
            (oo_ifn>1 && id<=oo_sequenceid))) {
          oo__error= 92;
          WARNv("wrong sequence at %s %Ld",
            ONAME(oo_sequencetype),oo_sequenceid)
          WARNv("next object is %s %Ld",ONAME(otype),id)
          }
        }  // dependencystage>=32
      }  // no diff contents is to be considered
    oo_sequencetype= otype; oo_sequenceid= id;

    // care about calculating a diff file
    if(global_diff) {  // diff
      if(oo__ifp==oo__if) {  // first file has been chosen
        if(diffcompare) {
          diffcompare= false;
          if(!diffdifference)
  continue;  // there has not been a change in object's contents
          oo__ifp->deleteobject= 0;
          }
        else {
          //if(global_diffcontents && oo_ifn<2) continue;
          oo__ifp->deleteobject= 1;  // add "delete request";
          }
        }
      else {  // second file has been chosen
        if(oo__ifp->tyid==oo__if->tyid &&
            oo__ifp->hisver==oo__if->hisver)
            // there is a similar object in the first file
  continue;  // ignore this object
        }  // end   second file has been chosen
      }  // end   diff

    // care about dependency stages
    if(dependencystage==11) {
        // 11:     no output;
        //         for each node which is inside the borders,
        //           set flag in ht;
        //         store start of ways in read_setjump();
        //         for each way which has a member with flag in ht,
        //           set the way's flag in ht;
        //         for each relation with a member with flag in ht,
        //           store the relation's flag and write the ids
        //           of member ways which have no flag in ht
        //           (use cww_);
      if(otype>=1)  // way or relation
        read_lockjump();
      if(oo__ifp->deleteobject==0) {  // object is not to delete
        if(otype==0) {  // node
          if(!border_active || border_queryinside(lon,lat))
              // no border to be applied OR node lies inside
            hash_seti(0,id);  // mark this node id as 'inside'
          }  // node
        else if(otype==1) {  // way
          refidp= refid;
          while(refidp<refide) {  // for every referenced node
            if(hash_geti(0,*refidp))
          break;
            refidp++;
            }  // end   for every referenced node
          if(refidp<refide)  // at least on node lies inside
            hash_seti(1,id);  // memorize that this way lies inside
          }  // way
        else {  // relation
          int64_t ri;  // temporary, refid
          int rt;  // temporary, reftype
          char* rr;  // temporary, refrole
          bool relinside;  // this relation lies inside
          bool wayinside;  // at least one way lies inside
          bool ismp;  // this relation is a multipolygon

          relinside= wayinside= ismp= false;
          refidp= refid; reftypep= reftype; refrolep= refrole;
          while(refidp<refide) {  // for every referenced object
            ri= *refidp; rt= *reftypep; rr= *refrolep;
            if(!relinside && hash_geti(rt,ri))
              relinside= true;
            if(!wayinside && rt==1 && (strcmp(rr,"outer")==0 ||
                strcmp(rr,"inner")==0) && hash_geti(1,ri))
                // referenced object is a way and part of
                // a multipolygon AND lies inside
              wayinside= true;
            refidp++; reftypep++; refrolep++;
            }  // end   for every referenced object
          if(relinside) {  // relation lies inside
            hash_seti(2,id);
            if(wayinside) {  // at least one way lies inside
              keyp= key; valp= val;
              while(keyp<keye) {  // for all key/val pairs of this object
                if(strcmp(*keyp,"type")==0 &&
                    strcmp(*valp,"multipolygon")==0) {
                  ismp= true;
              break;
                  }
                keyp++; valp++;
                }  // for all key/val pairs of this object
              if(ismp) {  // is multipolygon
                refidp= refid; reftypep= reftype; refrolep= refrole;
                while(refidp<refide) {  // for every referenced object
                  ri= *refidp; rt= *reftypep; rr= *refrolep;
                  if(rt==1 && (strcmp(rr,"outer")==0 ||
                      strcmp(rr,"inner")==0) &&
                      !hash_geti(1,ri)) {  // referenced object
                      // is a way and part of the multipolygon AND
                      // has not yet a flag in ht
                    cww_ref(ri);  // write id of the way
                    }
                  refidp++; reftypep++; refrolep++;
                  }  // end   for every referenced object
                }  // is multipolygon
              }  // at least one way lies inside
            }  // relation lies inside
          }  // relation
        }  // object is not to delete
continue;  // do not write this object
      }  // dependencystage 11
    else if(dependencystage==12) {
      // 12:     no output;
      //         for each way with a member with a flag in ht,
      //           set the way's flag in ht and write the ids
      //           of all the way's members (use cwn_);
      if(oo__ifp->deleteobject==0) {  // object is not to delete
        if(otype==1 && hash_geti(1,id)) {
            // way AND is marked in ht
          // store ids of all referenced nodes of this way
          refidp= refid;
          while(refidp<refide) {  // for every referenced node
            cwn_ref(*refidp);
            refidp++;
            }  // end   for every referenced node
          }  // way
        }  // object is not to delete
continue;  // do not write this object
      }  // dependencystage 12
    else if(dependencystage==21) {
        // 21:     no output;
        //         for each node inside the borders,
        //           set flag in ht;
        //         for each way with a member with a flag in ht,
        //           set the way's flag in ht and write the ids
        //           of all the way's members (use cwn_);
      if(oo__ifp->deleteobject==0) {  // object is not to delete
        if(otype==0) {  // node
          if(!border_active || border_queryinside(lon,lat))
              // no border to be applied OR node lies inside
            hash_seti(0,id);  // mark this node id as 'inside'
          }  // node
        else if(otype==1) {  // way
          refidp= refid;
          while(refidp<refide) {  // for every referenced node
            if(hash_geti(0,*refidp))
          break;
            refidp++;
            }  // end   for every referenced node
          if(refidp<refide) {  // at least on node lies inside
            hash_seti(1,id);  // memorize that this way lies inside
            // store ids of all referenced nodes of this way
            refidp= refid;
            while(refidp<refide) {  // for every referenced node
              cwn_ref(*refidp);
              refidp++;
              }  // end   for every referenced node
            }  // at least on node lies inside
          }  // way
        else {  // relation
          oo__ifp->endoffile= true;  // suppress warnings
          oo__close();  // the next stage will be entered as soon as
            // all files have been closed;
            // 21->22: as soon as first relation shall be written:
            //         rewind all files;
            //         set flags for nodes, use cwn_processing();
          }  // relation
        }  // object is not to delete
continue;  // do not write this object
      }  // dependencystage 21
    else if(otype==2) {  // relation
      if(!global_droprelations &&
          (dependencystage==31 || dependencystage==22)) {
          // not relations to drop AND
          // 22:     write each node which has a flag in ht to output;
          //         write each way which has a flag in ht to output;
          // 31:     for each node inside the borders,
          //           set flag in ht;
          //         for each way with a member with a flag in ht,
          //           set the way's flag in ht;
        // 22->32: as soon as first relation shall be written:
        //         clear flags for ways, use cww_processing_clear();
        //         switch output to temporary file;
        // 31->32: as soon as first relation shall be written:
        //         switch output to temporary .o5m file;
        wo_flush();
        if(write_newfile(o5mtempfile))
return 24;
        wo_format(0);
        if(hashactive)
          if(rr_ini(global_tempfilename))
return 25;
        if(dependencystage==22)
          cww_processing_clear();
        if(global_alltonodes)
          if(posr_ini(global_tempfilename))
return 26;
        oo__dependencystage(32);
          // 32:     for each relation with a member with a flag
          //           in ht, set the relation's flag in ht;
          //         for each relation,
          //           write its id and its members' ids
          //           into a temporary file (use rr_);
          //         if option --all-to-nodes is set, then
          //           for each relation, write its members'
          //             geopositions into a temporary file (use posr_);
        }  // dependencystage was 31
      }  // relation
    else {  // node or way
      }  // node or way
    // end   care about dependency stages

    // process object deletion
    if(oo__ifp->deleteobject!=0) {  // object is to delete
      if((otype==0 && !global_dropnodes) ||
          (otype==1 && !global_dropways) ||
          (otype==2 && !global_droprelations))
          // section is not to drop anyway
        if(global_outo5c || global_outosc || global_outosh)
          // write o5c, osc or osh file
          wo_delete(otype,id,hisver,histime,hiscset,hisuid,hisuser);
            // write delete request
  continue;  // end processing for this object
      }  // end   object is to delete

    // care about object statistics
    if(global_statistics &&
        dependencystage!=32) {
        // not:
        // 32:     for each relation with a member with a flag
        //           in ht, set the relation's flag in ht;
        //         for each relation,
        //           write its id and its members' ids
        //           into a temporary file (use rr_);
        //         if option --all-to-nodes is set, then
        //           for each relation, write its members'
        //             geopositions into a temporary file (use posr_);

      if(otype==0) {  // node
        if(statistics.nodes==0) {  // this is the first node
          statistics.lon_min= statistics.lon_max= lon;
          statistics.lat_min= statistics.lat_max= lat;
          }
        statistics.nodes++;
        if(statistics.node_id_min==0 || id<statistics.node_id_min)
          statistics.node_id_min= id;
        if(statistics.node_id_max==0 || id>statistics.node_id_max)
          statistics.node_id_max= id;
        if(lon<statistics.lon_min)
          statistics.lon_min= lon;
        if(lon>statistics.lon_max)
          statistics.lon_max= lon;
        if(lat<statistics.lat_min)
          statistics.lat_min= lat;
        if(lat>statistics.lat_max)
          statistics.lat_max= lat;
        }
      else if(otype==1) {  // way
        statistics.ways++;
        if(statistics.way_id_min==0 || id<statistics.way_id_min)
          statistics.way_id_min= id;
        if(statistics.way_id_max==0 || id>statistics.way_id_max)
          statistics.way_id_max= id;
        if(refide-refid>statistics.noderefs_max) {
          statistics.noderefs_oid= id;
          statistics.noderefs_max= refide-refid;
          }
        }
      else if(otype==2) {  // relation
        statistics.relations++;
        if(statistics.relation_id_min==0 ||
            id<statistics.relation_id_min)
          statistics.relation_id_min= id;
        if(statistics.relation_id_max==0 ||
            id>statistics.relation_id_max)
          statistics.relation_id_max= id;
        if(refide-refid>statistics.relrefs_max) {
          statistics.relrefs_oid= id;
          statistics.relrefs_max= refide-refid;
          }
        }
      if(histime!=0) {  // timestamp valid
        if(statistics.timestamp_min==0 ||
            histime<statistics.timestamp_min)
          statistics.timestamp_min= histime;
        if(statistics.timestamp_max==0 ||
            histime>statistics.timestamp_max)
          statistics.timestamp_max= histime;
        }
      if(keye-key>statistics.keyval_pairs_max) {
        statistics.keyval_pairs_otype= otype;
        statistics.keyval_pairs_oid= id;
        statistics.keyval_pairs_max= keye-key;
        }
      }  // object statistics

    // abort writing if user does not want any standard output
    if(global_outnone)
  continue;

    // write the object
    if(otype==0) {  // write node
      bool inside;  // node lies inside borders, if appl.

      if(!border_active)  // no borders shall be applied
        inside= true;
      else if(dependencystage==22)
          // 22:     write each node which has a flag in ht to output;
          //         write each way which has a flag in ht to output;
        inside= hash_geti(0,id);
      else {
        inside= border_queryinside(lon,lat);  // node lies inside
        if(inside)
          hash_seti(0,id);  // mark this node id as 'inside'
        }
      if(inside) {  // node lies inside
        if(global_alltonodes) {
          // check id range
          if(id>=global_otypeoffset05 || id<=-global_otypeoffset05)
            WARNv("node id %Ld"
              " out of range. Increase --object-type-offset",id)
          posi_set(id,lon,lat);  // store position
          }
        if(!global_dropnodes) {  // not to drop
          wo_node(id,
            hisver,histime,hiscset,hisuid,hisuser,lon,lat);
          keyp= key; valp= val;
          while(keyp<keye)  // for all key/val pairs of this object
            wo_node_keyval(*keyp++,*valp++);
          wo_node_close();
          }  // end   not to drop
        }  // end   node lies inside
      }  // write node
    else if(otype==1) {  // write way
      bool inside;  // way lies inside borders, if appl.

      if(!hashactive)  // no borders shall be applied
        inside= true;
      else if(dependencystage==22)
          // 22:     write each node which has a flag in ht to output;
          //         write each way which has a flag in ht to output;
        inside= hash_geti(1,id);
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
          if(global_alltonodes) {
              // all ways are to be converted to nodes
            int32_t x_min,x_max,y_min,y_max;
            int32_t x_middle,y_middle,xy_distance,new_distance;
            bool is_area;
            int n;  // number of referenced nodes with coordinates

            // check id range
            if(id>=global_otypeoffset05 || id<=-global_otypeoffset05)
              WARNv("way id %Ld"
                " out of range. Increase --object-type-offset",id)

            // determine the center of the way's bbox
            n= 0;
            refidp= refid; refxyp= refxy;
            while(refidp<refide) {  // for every referenced node
              *refxyp= NULL;  // (default)
              if(!global_dropbrokenrefs || hash_geti(0,*refidp)) {
                  // referenced node lies inside the borders
                posi_get(*refidp);  // get referenced node's coordinates
                *refxyp= posi_xy;
                if(posi_xy!=NULL) {  // coordinate is valid
                  if(n==0) {  // first coordinate
                    // just store it as min and max
                    x_min= x_max= posi_xy[0];
                    y_min= y_max= posi_xy[1];
                    }
                  else {  // additional coordinate
                    // adjust extrema
                    if(posi_xy[0]<x_min && x_min-posi_xy[0]<900000000)
                      x_min= posi_xy[0];
                    else if(posi_xy[0]>
                        x_max && posi_xy[0]-x_max<900000000)
                      x_max= posi_xy[0];
                    if(posi_xy[1]<y_min)
                      y_min= posi_xy[1];
                    else if(posi_xy[1]>y_max)
                      y_max= posi_xy[1];
                    }
                  n++;
                  }  // coordinate is valid
                }  // referenced node lies inside the borders
              refidp++; refxyp++;
              }  // end   for every referenced node

            // determine if the way is an area
            is_area= refide!=refid && refide[-1]==refid[0];
                // first node is the same as the last one

            // determine the valid center of the way
            x_middle= x_max/2+x_min/2;
            y_middle= (y_max+y_min)/2;
            if(is_area) {
              lon= x_middle;
              lat= y_middle;
              }
            else {  // the way is not an area
            // determine the node with has the smallest distance
            // to the center of the bbox ,,,
              n= 0;
              refidp= refid; refxyp= refxy;
              while(refidp<refide) {  // for every referenced node
                posi_xy= *refxyp;
                if(posi_xy!=NULL) {
                    // there is a coordinate for this reference
                  if(n==0) {  // first coordinate
                    // just store it as min and max
                    lon= posi_xy[0];
                    lat= posi_xy[1];
                    xy_distance= abs(lon-x_middle)+abs(lat-y_middle);
                    }
                  else {  // additional coordinate
                    new_distance= abs(posi_xy[0]-x_middle)+
                      abs(posi_xy[1]-y_middle);
                    if(new_distance<xy_distance) {
                      lon= posi_xy[0];
                      lat= posi_xy[1];
                      xy_distance= new_distance;
                      }
                    }  // additional coordinate
                  n++;
                  }  // there is a coordinate for this reference
                refidp++; refxyp++;
                }  // end   for every referenced node
              }  // the way is not an area

            // write a node as a replacement for the way
            if(n>0) {  // there is at least one coordinate available
              int64_t id_new;

              if(global_otypeoffsetstep!=0)
                id_new= global_otypeoffsetstep++;
              else
                id_new= id+global_otypeoffset10;
              wo_node(id_new,
                hisver,histime,hiscset,hisuid,hisuser,lon,lat);
              if (global_alltonodes_bbox) {
                char bboxbuf[84];
                write_createsbbox(x_min, y_min, x_max, y_max, bboxbuf);
                wo_node_keyval("bBox", bboxbuf);
                }
              keyp= key; valp= val;
              while(keyp<keye)  // for all key/val pairs of this object
                wo_node_keyval(*keyp++,*valp++);
              wo_node_close();
              posi_set(id+global_otypeoffset10,lon,lat);
              }  // there is at least one coordinate available
            }  // ways are to be converted to nodes
          else  {  // not --all-to-nodes
            wo_way(id,hisver,histime,hiscset,hisuid,hisuser);
            refidp= refid;
            while(refidp<refide) {  // for every referenced node
              if(!global_dropbrokenrefs || hash_geti(0,*refidp))
                  // referenced node lies inside the borders
                wo_noderef(*refidp);
              refidp++;
              }  // end   for every referenced node
            keyp= key; valp= val;
            while(keyp<keye)  // for all key/val pairs of this object
              wo_wayrel_keyval(*keyp++,*valp++);
            wo_way_close();
            }  // not --all-to-nodes
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

        in= hash_geti(2,id);
        if(dependencystage==32) {
          // 32:     for each relation with a member with a flag
          //           in ht, set the relation's flag in ht;
          //         for each relation,
          //           write its id and its members' ids
          //           into a temporary file (use rr_);
          //         if option --all-to-nodes is set, then
          //           for each relation, write its members'
          //             geopositions into a temporary file (use posr_);
          bool has_highway,has_area;  // relation has certain tags
          bool is_area;  // the relation is assumed to represent an area
          bool idwritten,posridwritten;

          // determine if this relation is assumed to represent
          // an area or not
          has_highway= has_area= false;
          keyp= key; valp= val;
          while(keyp<keye) {  // for all key/val pairs of this object
            if(strcmp(*keyp,"highway")==0 ||
                strcmp(*keyp,"waterway")==0 ||
                strcmp(*keyp,"railway")==0 ||
                strcmp(*keyp,"aerialway")==0 ||
                strcmp(*keyp,"power")==0 ||
                strcmp(*keyp,"route")==0
                )
              has_highway= true;
            else if(strcmp(*keyp,"area")==0 &&
                strcmp(*valp,"yes")==0)
              has_area= true;
            keyp++,valp++;
            }
          is_area= !has_highway || has_area;

          // write the id of this relation and its members
          // to a temporary file
          idwritten= posridwritten= false;
          refidp= refid; reftypep= reftype;
          while(refidp<refide) {  // for every referenced object
            ri= *refidp;
            rt= *reftypep;
            if(hashactive) {
              if(rt==2) {  // referenced object is a relation
                if(!idwritten) {  // did not yet write our relation's id
                  rr_rel(id);  // write it now
                  idwritten= true;
                  }
                rr_ref(ri);
                }
              }
            if(global_alltonodes) {
              if(!posridwritten) {
                  // did not yet write our relation's id
                // write it now
                posr_rel(id,is_area);
                posi_set(id+global_otypeoffset20,posi_nil,0);
                  // reserve space for this relation's coordinates
                posridwritten= true;
                }
              if(rt==1)  // way
                ri+= global_otypeoffset10;
              else if(rt==2)  // relation
                ri+= global_otypeoffset20;
              posr_ref(ri);
              }
            refidp++; reftypep++;
            }  // end   for every referenced object
          inside= true;
          }
        else if(dependencystage==33) {
            // 33:     write each relation which has a flag in ht
            //           to output; use temporary .o5m file as input;
          inside= in;
          }
        else
          inside= true;
        if(inside) {  // no borders OR at least one node inside
          if(global_alltonodes && dependencystage==33) {
              // all relations are to be converted to nodes AND
              // 33:     write each relation which has a flag in ht
              //           to output; use temporary .o5m file as input;
            if(id>=global_otypeoffset05 || id<=-global_otypeoffset05)
              WARNv("relation id %Ld"
                " out of range. Increase --object-type-offset",id)
            posi_get(id+global_otypeoffset20);  // get coorinates
            if(posi_xy!=NULL && posi_xy[0]!=posi_nil) {
                // stored coordinates are valid
              int64_t id_new;

              if(global_otypeoffsetstep!=0)
                id_new= global_otypeoffsetstep++;
              else
                id_new= id+global_otypeoffset20;
              // write a node as a replacement for the relation
              wo_node(id_new,
                hisver,histime,hiscset,hisuid,hisuser,
                posi_xy[0],posi_xy[1]);
              if (global_alltonodes_bbox) {
                char bboxbuf[84];
                write_createsbbox(posi_xy[2], posi_xy[3],
                  posi_xy[4], posi_xy[5], bboxbuf);
                wo_node_keyval("bBox", bboxbuf);
                }
              keyp= key; valp= val;
              while(keyp<keye)  // for all key/val pairs of this object
                wo_node_keyval(*keyp++,*valp++);
              wo_node_close();
              }  // stored coordinates are valid
            }  // relations are to be converted to nodes
          else {  // dependencystage!=33 OR not --all-to-nodes
            wo_relation(id,hisver,histime,hiscset,hisuid,hisuser);
            refidp= refid; reftypep= reftype; refrolep= refrole;
            while(refidp<refide) {  // for every referenced object
              ri= *refidp;
              rt= *reftypep;
              rr= *refrolep;

              if(dependencystage<33) {
                  // not:
                  // 33:     write each relation which has a flag in ht
                  //           to output;
                  //         use temporary .o5m file as input;
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
              else {  // dependencystage==33
                // 33:     write each relation which has a flag in ht
                //           to output;
                //         use temporary .o5m file as input;
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
              wo_wayrel_keyval(*keyp++,*valp++);
            wo_relation_close();
            }  // stage!=3 OR not --all-to-nodes
          }  // end   no borders OR at least one node inside
        }  // end   not relations to drop
      }  // write relation
    }  // end   read all input files
  if(!global_outnone) {
    if(writeheader)
      wo_start(wformat,oo__bbvalid,
        oo__bbx1,oo__bby1,oo__bbx2,oo__bby2,oo__timestamp);
    wo_end();
    }
  if(global_statistics) {  // print statistics
    FILE* fi;
    if(global_outstatistics) fi= stdout;
    else fi= stderr;

    if(statistics.timestamp_min!=0) {
      char timestamp[30];

      write_createtimestamp(statistics.timestamp_min,timestamp);
      fprintf(fi,"timestamp min: %s\n",timestamp);
      }
    if(statistics.timestamp_max!=0) {
      char timestamp[30];

      write_createtimestamp(statistics.timestamp_max,timestamp);
      fprintf(fi,"timestamp max: %s\n",timestamp);
      }
    if(statistics.nodes>0) {  // at least one node
      char coord[20];

      write_createsfix7o(statistics.lon_min,coord);
      fprintf(fi,"lon min: %s\n",coord);
      write_createsfix7o(statistics.lon_max,coord);
      fprintf(fi,"lon max: %s\n",coord);
      write_createsfix7o(statistics.lat_min,coord);
      fprintf(fi,"lat min: %s\n",coord);
      write_createsfix7o(statistics.lat_max,coord);
      fprintf(fi,"lat max: %s\n",coord);
      }
    fprintf(fi,"nodes: %Ld\n",statistics.nodes);
    fprintf(fi,"ways: %Ld\n",statistics.ways);
    fprintf(fi,"relations: %Ld\n",statistics.relations);
    if(statistics.node_id_min!=0)
      fprintf(fi,"node id min: %Ld\n",statistics.node_id_min);
    if(statistics.node_id_max!=0)
      fprintf(fi,"node id max: %Ld\n",statistics.node_id_max);
    if(statistics.way_id_min!=0)
      fprintf(fi,"way id min: %Ld\n",statistics.way_id_min);
    if(statistics.way_id_max!=0)
      fprintf(fi,"way id max: %Ld\n",statistics.way_id_max);
    if(statistics.relation_id_min!=0)
      fprintf(fi,"relation id min: %Ld\n",
        statistics.relation_id_min);
    if(statistics.relation_id_max!=0)
      fprintf(fi,"relation id max: %Ld\n",
        statistics.relation_id_max);
    if(statistics.keyval_pairs_max!=0) {
      fprintf(fi,"keyval pairs max: %i\n",
        statistics.keyval_pairs_max);
      fprintf(fi,"keyval pairs max object: %s %Ld\n",
        ONAME(statistics.keyval_pairs_otype),
        statistics.keyval_pairs_oid);
      }
    if(statistics.noderefs_max!=0) {
      fprintf(fi,"noderefs max: %i\n",
        statistics.noderefs_max);
      fprintf(fi,"noderefs max object: way %Ld\n",
        statistics.noderefs_oid);
      }
    if(statistics.relrefs_max!=0) {
      fprintf(fi,"relrefs max: %i\n",
        statistics.relrefs_max);
      fprintf(fi,"relrefs max object: relation %Ld\n",
        statistics.relrefs_oid);
      }
    }  // print statistics
  return oo__error;
  }  // end   oo_main()

//------------------------------------------------------------
// end   Module oo_   osm to osm module
//------------------------------------------------------------
