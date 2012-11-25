//------------------------------------------------------------
// Module oo_   osm to osm module
//------------------------------------------------------------
#include "stdinc.hpp"
#include "obj_relref.hpp"
#include "osm_hash.hpp"
#include "osm_border.h"
#include "oo.h"

#include "str.hpp"
#include <vector>
#include "process.hpp"
#include "util.h"
#include "read.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <vector>


// this module provides procedures which read osm objects,
// process them and write them as osm objects, using module wo_;
// that goes for .osm format as well as for .o5m format;
// as usual, all identifiers of a module have the same prefix,
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

 void oo__xmltostr(OO & oo,char* s) {
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

 bool oo__xmltag(OO & oo,Read & read) {
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
  oo__xmltostr(oo,oo__xmlkey);
  oo__xmltostr(oo,oo__xmlval);
  return false;
  }  // end   oo__xmltag()


static void oo__findbb(OO & oo,Process * global, Read & read) {
  // find timestamp and border box in input file;
  // return:
  // oo__bbvalid: following border box information is valid;
  // oo__bbx1 .. oo__bby2: border box coordinates;

  // read_bufp will not be changed;
  byte* bufp,*bufe;

  read_input();
  bufp= (byte*)read.bufp(); 
  bufe= (byte*)read.bufe();
  if(oo.ifp->format==0) {  // o5m
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
        if(bufp<bufe) oo.timestamp= pbf_sint64(&bufp);
        bufp= bufe;
    continue;
        }  // timestamp
      if(b==0xdb && oo.ifp==oo.inf.begin()) {
          // border box AND first input file
        bufp++;
        l= pbf_uint32(&bufp);
        bufe= bufp+l;
        if(bufp<bufe) oo.bbx1= pbf_sint32(&bufp);
        if(bufp<bufe) oo.bby1= pbf_sint32(&bufp);
        if(bufp<bufe) oo.bbx2= pbf_sint32(&bufp);
        if(bufp<bufe) {
          oo.bby2= pbf_sint32(&bufp);
          oo.bbvalid= true;
          }
        bufp= bufe;
    continue;
        }  // border box
      bufp++;
      l= pbf_uint32(&bufp);  // jump over this dataset
      bufp+= l;  // jump over this dataset
      }  // end   for all bytes
    }  // end   o5m
  else if(oo.ifp->format>0) {  // osm xml
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
            oo.timestamp= oo__strtimetosint64(sp);
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
      else if(c1=='b' && c2=='o' && c3=='u'  && oo.ifp==oo.inf.begin()) {
          // bounds AND first input file
        // bounds may be supplied in one of these formats:
        // <bounds minlat="53.01104" minlon="8.481593"
        //   maxlat="53.61092" maxlon="8.990601"/>
        // <bound box="49.10868,6.35017,49.64072,7.40979"
        //   origin="http://www.openstreetmap.org/api/0.6"/>
        uint32_t bboxcomplete;  // flags for oo.bbx1 .. oo.bby2
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
            oo.bby1= oo__strtodeg(sp);
            if(oo.bby1!=oo__nildeg) bboxcomplete|= 2;
            }
          else if((l= strzlcmp(sp,"minlon=\""))>0 ||
              (l= strzlcmp(sp,"minlon=\'"))>0 ||
              ((isdig(c) || c=='-' || c=='.') && (bboxcomplete&1)==0)) {
            sp+= l;
            oo.bbx1= oo__strtodeg(sp);
            if(oo.bbx1!=oo__nildeg) bboxcomplete|= 1;
            }
          else if((l= strzlcmp(sp,"maxlat=\""))>0 ||
              (l= strzlcmp(sp,"maxlat=\'"))>0 ||
              ((isdig(c) || c=='-' || c=='.') && (bboxcomplete&8)==0)) {
            sp+= l;
            oo.bby2= oo__strtodeg(sp);
            if(oo.bby2!=oo__nildeg) bboxcomplete|= 8;
            }
          else if((l= strzlcmp(sp,"maxlon=\""))>0 ||
              (l= strzlcmp(sp,"maxlon=\'"))>0 ||
              ((isdig(c) || c=='-' || c=='.') && (bboxcomplete&4)==0)) {
            sp+= l;
            oo.bbx2= oo__strtodeg(sp);
            if(oo.bbx2!=oo__nildeg) bboxcomplete|= 4;
            }
          for(;;) {  // find next blank or comma
            c= *sp;
            if(oo__wsnul(c) || c==',')
          break;
            sp++;
            }
          }  // end   for every word in 'bounds'
        oo.bbvalid= bboxcomplete==15;
        bufp++;
    continue;
        }  // bounds
      else {
        bufp++;
    continue;
        }
      }  // end   for all bytes of the file
    }  // end   osm xml
  else if(oo.ifp->format==-1) {  // pbf
    //pb_input();
    if(read.pbf.type == PBF::pbf_type && oo.ifp==oo.inf.begin()) {
        // pbf header AND first input file
      oo.bbx1= read.pbf.bbx1; 
      oo.bby1= read.pbf.bby1;
      oo.bbx2= read.pbf.bbx2; 
      oo.bby2= read.pbf.bby2;

      oo.bbvalid= read.pbf.bbvalid;
      if(read.pbf.filetimestamp!=0)
        oo.timestamp= read.pbf.filetimestamp;
      }  // end   pbf header
    else
      oo.alreadyhavepbfobject= true;
    }  // end   pbf
  }  // end   oo__findbb()

int oo__gettyid(OO & oo, Process & global, Read & read) {
  // get tyid of the next object in the currently processed input file;
  // tyid is a combination of object type and id: we take the id and
  // add UINT64_C(0x0800000000000000) for nodes,
  // UINT64_C(0x1800000000000000) for ways, and
  // UINT64_C(0x2800000000000000) for relations;
  // if global_diff is set, besides tyid the hisver is retrieved too;
  // oo.ifp: handle of the currently processed input file;
  // return: ==0: ok; !=0: could not get tyid because starting object
  //         is not an osm object;
  // oo.ifp->tyid: tyid of the starting osm object;
  //                if there is not an osm object starting at
  //                read_bufp, *iyidp remains unchanged;
  // oo.ifp->hisver: only if global_diff; version of next object;
  static const uint64_t idoffset[]= {UINT64_C(0x0800000000000000),
    UINT64_C(0x1800000000000000),UINT64_C(0x2800000000000000),
    0,0,0,0,0,0,0,0,0,0,0,0,0,UINT64_C(0x0800000000000000),
    UINT64_C(0x1800000000000000),UINT64_C(0x2800000000000000)};
  int format;

  format= oo.ifp->format;
  if(format==0) {  // o5m
    int64_t o5id;
    byte* p,b;
    int l;

    o5id= oo.ifp->o5id;
    p= read.bufpb();
    while(p<read.bufeb()) {
      b= *p++;
      if(b>=0x10 && b<=0x12) {  // osm object is starting here
        oo.ifp->tyid= idoffset[b];
        read.pbf.intjump(&p);  // jump over length information
        oo.ifp->tyid+= o5id+pbf_sint64(&p);
        if(global.diff)
          oo.ifp->hisver= pbf_uint32(&p);
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

    s= (char*)read.bufp();
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
        oo.ifp->tyid= r+oo__strtosint64(s+4);
        if(!global.diff)
return 0;
        oo.ifp->hisver= 0;
        for(;;) {
          if(*s=='>' || *s==0)
return 0;
          if(s[0]==' ' && s[1]=='v' && s[2]=='e' && s[3]=='r' &&
              s[4]=='s' && s[5]=='i' && s[6]=='o' && s[7]=='n' &&
              s[8]=='=' && (s[9]=='\"' || s[9]=='\'') && isdig(s[10])) {
              // found version
            oo.ifp->hisver= oo__strtouint32(s+10);
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
      oo.alreadyhavepbfobject= true;
      }
#endif
    if((read.pbf.type & PBF::osm_bit)!= read.pbf.type)  // still not an osm object
return 1;
    oo.ifp->tyid= idoffset[read.pbf.type]+read.pbf.id;
    oo.ifp->hisver= read.pbf.hisver;
return 0;
    }
return 2;  // (unknown format)
  }  // end   oo__gettyid()

 int oo__getformat(OO & oo,Process & global, Read & read) {
  // determine the formats of all opened files of unknown format
  // and store these determined formats;
  // do some intitialization for the format, of necessary;
  // oo.if[].format: !=-9: do nothing for this file;
  // return: 0: ok; !=0: error;
  //         5: too many pbf files;
  //            this is, because the module pbf (see above)
  //            does not have multi-client capabilities;
  // oo.if[].format: input file format; ==0: o5m; ==10: xml; ==-1: pbf;
  static int pbffilen= 0;  // number of pbf input files;
  oo__if_vt::iterator ifptemp= oo.ifp;
  byte* bufp;
  #define bufsp ((char*)bufp)  // for signed char

  oo.ifp= oo.inf.begin();
  while(oo.ifp<oo.ife) {  // for all input files
    if(oo.ifp->ri!=NULL && oo.ifp->format==-9) {
        // format not yet determined
      read.read_switch(oo.ifp->ri);
      if(read.bufp()>=read.bufe()) {  // file empty
        PERRv("file empty: %.80s",oo.ifp->filename)
return 2;
        }
      bufp= read.bufpb();
      if(bufp[0]==0 && bufp[1]==0 && bufp[2]==0 &&
          bufp[3]>8 && bufp[3]<20) {  // presumably .pbf format
        if(++pbffilen>1) {   // pbf
          PERR("more than one .pbf input file is not allowed.");
return 5;
          }
        oo.ifp->format= -1;
        read.pbf.ini();
        read.pbf.input(false);
        oo.alreadyhavepbfobject= true;
        }
      else if(strzcmp(bufsp,"<?xml")==0 ||
          strzcmp(bufsp,"<osm")==0) {  // presumably .osm format
        oo.ifp->format= 10;
        }
      else if(bufp[0]==0xff && bufp[1]==0xe0 && (
          strzcmp(bufsp+2,"\x04""o5m2")==0 ||
          strzcmp(bufsp+2,"\x04""o5c2")==0 )) {
            // presumably .o5m format
        oo.ifp->format= 0;
        oo.ifp->str= str_open();
          // call some initialization of string read module
        }
      else if((bufp[0]==0xff && bufp[1]>=0x10 && bufp[1]<=0x12) ||
          (bufp[0]==0xff && bufp[1]==0xff &&
          bufp[2]>=0x10 && bufp[2]<=0x12) ||
	      (bufp[0]==0xff && read.bufe()==read.bufp()+1)) {
          // presumably shortened .o5m format
        if(loglevel>=2)
          fprintf(stderr,"osmconvert: Not a standard .o5m file header "
            "%.80s\n",oo.ifp->filename);
        oo.ifp->format= 0;
        oo.ifp->str= str_open();
          // call some initialization of string read module
        }
      else {  // unknown file format
        PERRv("unknown file format: %.80s",oo.ifp->filename)
return 3;
        }
      oo__findbb(oo,&global,read);
      oo.ifp->tyid= 0;
      oo.ifp->hisver= 0;
      oo__gettyid(oo,global,read);
        // initialize tyid of the currently used input file
      }  // format not yet determined
    oo.ifp++;
    }  // for all input files
  oo.ifp= ifptemp;
  if(loglevel>0 && oo.timestamp!=0) {
    char s[30];  // timestamp as string

    write_createtimestamp(oo.timestamp,s);
    fprintf(stderr,"osmconvert: File timestamp: %s\n",s);
    }
  if(global.timestamp!=0)  // user wants a new file timestamp
    oo.timestamp= global.timestamp;
  return 0;
  #undef bufsp
  }  // end oo__getformat()

static uint64_t oo__tyidold= 0;  // tyid of the last written object;

void oo__switch(OO & oo, Process & global, Read & read) {
  // determine the input file with the lowest tyid
  // and switch to it
   oo__if_vt::iterator ifp,ifpmin;
  uint64_t tyidmin,tyidold,tyid;

  // update tyid of the currently used input file and check sequence
  if(oo.ifp!=oo.inf.end()) {  // handle of current input file is valid
    tyidold= oo.ifp->tyid;
    if(oo__gettyid(oo,global, read)==0) {  // new tyid is valid
//DPv(got   %llx %s,oo.ifp->tyid,oo.ifp->filename)
      if(oo.ifp->tyid<tyidold) {  // wrong sequence
        int64_t id; int ty;

        oo.error= 91;
        ty= tyidold>>60;
        id= ((int64_t)(tyidold & UINT64_C(0xfffffffffffffff)))-
          INT64_C(0x800000000000000);
        WARNv("wrong order at %s %Ld in file %s",
	      ONAME(ty),id,oo.ifp->filename);
        ty= oo.ifp->tyid>>60;
        id= ((int64_t)(oo.ifp->tyid & UINT64_C(0xfffffffffffffff)))-
          INT64_C(0x800000000000000);
        WARNv("next object is %s %Ld",ONAME(ty),id);
        }  // wrong sequence
      }  // new tyid is valid
    }  // end   handle of current input file is valid

  // find file with smallest tyid
  tyidmin= UINT64_C(0xffffffffffffffff);
  ifpmin= oo.ifp;
    // default; therefore we do not switch in cases we do not
    // find a minimum
  ifp= oo.ife;
  while(ifp != oo.inf.begin()) {
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
  if(ifpmin!=oo.ifp) {
      // that file is not the file we're already reading from
    oo.ifp= ifpmin;
    read_switch(oo.ifp->ri);
    str_switch(oo.ifp->str);
    }
//DPv(chose %llx %s,oo.ifp->tyid,oo.ifp->filename)
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

static int oo__rewindall(OO & oo, Process & global, Read & read) {
  // rewind all input files;
  // return: 0: ok; !=0: error;
  oo__if_vt::iterator ifp,ifp_sav;

  ifp_sav= oo.ifp;  // save original info pointer
  ifp= oo.inf.begin();
  while(ifp<oo.ife) {
    if(ifp->riph!=NULL) {
      if(ifp->ri==NULL && ifp->riph!=NULL) {
          // file has been logically closed
        // logically reopen it
        ifp->ri= ifp->riph;
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
  oo.ifp= ifp_sav;  // restore original info pointer
  if(oo.ifp != oo.inf.begin() && oo.ifp->ri!=NULL) {
    read_switch(oo.ifp->ri);
    str_switch(oo.ifp->str);
    }
  else
    oo__switch(oo,global,read);
  oo__tyidold= 0;
  oo_sequencetype( -1);
  oo_sequenceid(INT64_C(-0x7fffffffffffffff));
  return 0;
  }  // end oo__rewindall()

static int oo__jumpall(OO & oo, Process & global, Read & read) {
  // jump in all input files to the previously stored position;
  // return: 0: ok; !=0: error;
  std::vector<oo__if_t>::iterator ifp;
  int r;

 std::vector<oo__if_t>::iterator  ifp_sav= oo.ifp;  // save original info pointer
 ifp= oo.inf.begin();
  while(ifp<oo.ife) {  // for all files
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
  oo.ifp= ifp_sav;  // restore original info pointer
  if(
     (oo.ifp != oo.inf.begin())  && 
     oo.ifp->ri!=NULL) {
    read_switch(oo.ifp->ri);
    str_switch(oo.ifp->str);
    }
  else {
    oo__switch(oo,global,read);
    if(oo.ifp==oo.inf.begin()) {  // no file chosen
      //ifp= oo.if;
      while(oo.ifp != oo.ife) {  // for all files
        oo.ifp->ri= NULL;  // mark file as able to be logically reopened
        oo.ifp++;
        }
      }
    }
  oo__tyidold= 0;
  oo_sequencetype(-1);
  oo_sequenceid(INT64_C(-0x7fffffffffffffff));
  return 0;
  }  // end oo__jumpall()

static void oo__close(OO & oo) {
  // logically close an input file;
  // oo.ifp: handle of currently active input file;
  // if this file has already been closed, nothing happens;
  // after calling this procedure, the handle of active input file
  // will be invalid; you may call oo__switch() to select the
  // next file in the sequence;
  if(oo.ifp!=oo.inf.end() && oo.ifp->ri!=NULL) {
    if((!oo.ifp->endoffile)  && 
       (oo.ifp> oo.inf.begin()))  // missing logical end of file
      fprintf(stderr,"osmconvert Warning: "
        "unexpected end of input file: %.80s\n",oo.ifp->filename);
    read_switch(oo.ifp->ri);
    //read_close();
    oo.ifp->ri= NULL;
    oo.ifp->tyid= UINT64_C(0xffffffffffffffff);
      // (to prevent this file being selected as next file
      // in the sequence)

    }
  oo.ifp--;
  }  // end oo__close()

static void oo__closeall(OO & oo) {
  // close all input files;
  // after calling this procedure, the handle of active input file
  // will be invalid;

    // this is used to suppress warning messages in oo__close()
  while(oo.ife != oo.inf.begin()) {
    oo.ifp= --oo.ife;
    oo.ifp->endoffile= true;  // suppress warnings (see oo__close())
    if(oo.ifp->riph!=NULL) {
      read_switch(oo.ifp->riph);
      read_close();
      }
    oo.ifp->ri= oo.ifp->riph= NULL;
    oo.ifp->tyid= UINT64_C(0xffffffffffffffff);
    }
  }  // end oo__closeall()

// static void* oo__malloc_p[50];
//   // pointers for controlled memory allocation
// static int oo__malloc_n= 0;
//   // number of elements used in oo__malloc_p[]

// static void* oo__malloc(size_t size) {
//   // same as malloc(), but the allocated memory will be
//   // automatically freed at program end;
//   void* mp;

//   mp= malloc(size);
//   if(mp==NULL) {
//     PERRv("cannot allocate %Ld bytes of memory.",(int64_t)size);
//     exit(1);
//     }
//   oo__malloc_p[oo__malloc_n++]= mp;
//   return mp;
//   }  // oo__malloc()

static void oo__end(OO & oo) {
//   // clean-up this module;
  oo__closeall(oo);
//   while(oo__malloc_n>0)
//     free(oo__malloc_p[--oo__malloc_n]);
}  // end oo__end()



//------------------------------------------------------------

bool oo_open(OO & oo, const char* filename,Read & read) {
  // open an input file;
  // filename[]: path and name of input file;
  //             ==NULL: standard input;
  // return: 0: ok; 1: no appropriate input file;
  //         2: maximum number of input files exceeded;
  // the handle for the current input file oo.ifp is set
  // to the opened file;
  // after having opened all input files, call oo__getformat();
  // you do not need to care about closing the file;
  static bool firstrun= true;

  if(oo.ife>=oo.ifee) {
    fprintf(stderr,"osmconvert Error: too many input files.\n");
    //    fprintf(stderr,"osmconvert Error: too many input files: %d>%d\n",
    //      0,global_fileM);
return 2;
    }
  if(read_open(filename)!=0)
return 1;
  oo.ife->ri= oo.ife->riph= read.infop;
  oo.ife->str= NULL;
  oo.ife->format= -9;  // 'not yet determined'
  oo.ife->tyid= 0;
  if(filename==NULL)
    oo.ife->filename= "standard input";
  else
    oo.ife->filename= filename;
  oo.ife->endoffile= false;
  oo.ife->deleteobject= 0;
  oo.ifp= oo.ife++;

  if(firstrun) {
    firstrun= false;
    atexit(oo.end);
    }
  return 0;
  }  // end   oo_open()


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
static void oo__dependencystage(OO & oo, int ds) {
  // change the dependencystage;
  if(loglevel>=2)
    PINFOv("changing dependencystage from %i to %i.",oo.dependencystage,ds);
  oo.dependencystage= ds;
  }  // oo__dependencystage()



//------------------------------------------------------------
// end   Module oo_   osm to osm module
//------------------------------------------------------------
