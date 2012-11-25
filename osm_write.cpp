
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
static const char* write__filename_standard= NULL;
  // name of standard output file; ==NULL: standard output;
static const char* write__filename_temp= NULL;
  // name of the tempfile; ==NULL: no tempfile;
static char write__buf[UINT64_C(16000000)];
static char* write__bufe= write__buf+sizeof(write__buf);
  // (const) water mark for buffer filled 100%
static char* write__bufp= write__buf;
static int write__fd= 1;  // (initially standard output)
static int write__fd_standard= 1;  // (initially standard output)
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
  }  // end   write__close()

static void write__end() {
  // terminate the services of this module;
  if(write__fd>1)
    write__close();
  if(write__fd_standard>1) {
    write__fd= write__fd_standard;
    write__close();
    write__fd_standard= 0;
    }
  if(loglevel<2)
    if(write__filename_temp!=NULL) unlink(write__filename_temp);
  }  // end   write__end()

//------------------------------------------------------------

static bool write_testmode= false;  // no standard output
static bool write_error= false;  // an error has occurred

static inline void write_flush() {
  if(write__bufp>write__buf && !write_testmode)
      // at least one byte in buffer AND not test mode
    write_error|=
      write(write__fd,write__buf,write__bufp-write__buf)<0;
  write__bufp= write__buf;
  }  // end   write_flush();

static int write_open(const char* filename) {
  // open standard output file;
  // filename: name of the output file;
  //           this string must be accessible until program end;
  //           ==NULL: standard output;
  // this procedure must be called before any output is done;
  // return: 0: OK; !=0: error;
  static bool firstrun= true;

  if(loglevel>=2)
    fprintf(stderr,"Write-opening: %s\n",
      filename==NULL? "stdout": filename);
  if(filename!=NULL) {  // not standard output
    write__fd= open(filename,
      O_WRONLY|O_CREAT|O_TRUNC|O_BINARY,00600);
    if(write__fd<1) {
      fprintf(stderr,
        "osmconvert Error: could not open output file: %.80s\n",
          filename);
      write__fd= 1;
return 1;
      }
    write__fd_standard= write__fd;
    write__filename_standard= filename;
    }
  if(firstrun) {
    firstrun= false;
    atexit(write__end);
    }
  return 0;
  }  // end   write_open()

static int write_newfile(const char* filename) {
  // change to another (temporary) output file;
  // filename: new name of the output file;
  //           this string must be accessible until program end
  //           because the name will be needed to delete the file;
  //           ==NULL: change back to standard output file;
  // the previous output file is closed by this procedure, unless
  // it is standard output;
  // return: 0: OK; !=0: error;
  if(loglevel>=2)
    fprintf(stderr,"Write-opening: %s\n",
      filename==NULL? "stdout": filename);
  if(filename==NULL) {  // we are to change back to standard output file
    if(loglevel>=2)
      fprintf(stderr,"Write-reopening: %s\n",
        write__filename_standard==NULL? "stdout":
        write__filename_standard);
    write__close();  // close temporary file
    write__filename= write__filename_standard;
    write__fd= write__fd_standard;
    }
  else {  // new temporary file shall be opened
    if(loglevel>=2)
      fprintf(stderr,"Write-opening: %s\n",filename);
    write__filename= filename;
    unlink(filename);
    write__fd= open(filename,O_WRONLY|O_CREAT|O_TRUNC|O_BINARY,00600);
    if(write__fd<1) {
      fprintf(stderr,
        "osmconvert Error: could not open output file: %.80s\n",
        filename);
      write__fd= 1;
return 2;
      }
    write__filename_temp= filename;
    }
  return 0;
  }  // end   write_newfile()

static inline void write_char(int c) {
  // write one byte to stdout, use a buffer;
  if(write__bufp>=write__bufe) {  // the write buffer is full
    if(!write_testmode)
      write_error|=
        write(write__fd,write__buf,write__bufp-write__buf)<0;
    write__bufp= write__buf;
    }
  *write__bufp++= (char)c;
  }  // end   write_char();

static inline void write_mem(const byte* b,int l) {
  // write a memory area to stdout, use a buffer;
  while(--l>=0) {
    if(write__bufp>=write__bufe) {  // the write buffer is full
      if(!write_testmode)
        write_error|=
          write(write__fd,write__buf,write__bufp-write__buf)<0;
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
        write_error|=
          write(write__fd,write__buf,write__bufp-write__buf)<0;
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
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    #if 1
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    #else
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0};
    #endif
  byte b0,b1,b2,b3;
  int i;
  uint32_t u;
  #define write__char_D(c) { \
    if(write__bufp>=write__bufe) { \
      if(!write_testmode) \
        write_error|= \
          write(write__fd,write__buf,write__bufp-write__buf)<0; \
      write__bufp= write__buf; \
      } \
    *write__bufp++= (char)(c); }

  for(;;) {
    b0= *s++;
    if(b0==0)
  break;
    i= allowedchar[b0];
    if(i==0)  // this character may be written as is
      write__char_D(b0)
    else {  // use numeric encoding
      if(--i<=0)  // one byte
        u= b0;
      else {
        b1= *s++;
        if(--i<=0 && b1>=128)  // two bytes
          u= ((b0&0x1f)<<6)+(b1&0x3f);
        else {
          b2= *s++;
          if(--i<=0 && b1>=128 && b2>=128)  // three bytes
            u= ((b0&0x0f)<<12)+((b1&0x3f)<<6)+(b2&0x3f);
          else {
            b3= *s++;
            if(--i<=0 && b1>=128 && b2>=128 && b3>=128)  // four bytes
              u= ((b0&0x07)<<18)+((b1&0x3f)<<12)+
                ((b1&0x3f)<<6)+(b2&0x3f);
            else
              u= (byte)'?';
            }
          }
        }
      write__char_D('&') write__char_D('#')
      if(u<100) {
        if(u>=10)
          write__char_D(u/10+'0')
        write__char_D(u%10+'0')
        }
      else if(u<1000) {
        write__char_D(u/100+'0')
        write__char_D((u/10)%10+'0')
        write__char_D(u%10+'0')
        }
      else {
        char st[30];

        uint32toa(u,st);
        write_str(st);
        }
      write__char_D(';')
      }  // use numeric encoding
    }
  #undef write__char_D
  }  // end   write_xmlstr();

static inline void write_xmlmnstr(const char* s) {
  // write an XML string to stdout, use a buffer;
  // every character which is not allowed within an XML string
  // will be replaced by the appropriate mnemonic or decimal sequence;
  static const byte allowedchar[]= {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,9,0,0,0,9,9,0,0,0,0,0,0,0,0,  // \"&'
    0,0,0,0,0,0,0,0,0,0,0,0,9,0,9,0,  // <>
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,  // {}DEL
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    #if 1
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    #else
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0};
    #endif
  byte b0,b1,b2,b3;
  int i;
  uint32_t u;
  #define write__char_D(c) { \
    if(write__bufp>=write__bufe) { \
      if(!write_testmode) \
        write_error|= \
          write(write__fd,write__buf,write__bufp-write__buf)<0; \
      write__bufp= write__buf; \
      } \
    *write__bufp++= (char)(c); }
  #define D(i) ((byte)(s[i]))
  #define DD ((byte)c)

  for(;;) {
    b0= *s++;
    if(b0==0)
  break;
    i= allowedchar[b0];
    if(i==0)  // this character may be written as is
      write__char_D(b0)
    else if(i==9) {  // there is a mnemonic for this character
      write__char_D('&')
      switch(b0) {
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
      }  // there is a mnemonic for this character
    else {  // use numeric encoding
      if(--i<=0)  // one byte
        u= b0;
      else {
        b1= *s++;
        if(--i<=0 && b1>=128)  // two bytes
          u= ((b0&0x1f)<<6)+(b1&0x3f);
        else {
          b2= *s++;
          if(--i<=0 && b1>=128 && b2>=128)  // three bytes
            u= ((b0&0x0f)<<12)+((b1&0x3f)<<6)+(b2&0x3f);
          else {
            b3= *s++;
            if(--i<=0 && b1>=128 && b2>=128 && b3>=128)  // four bytes
              u= ((b0&0x07)<<18)+((b1&0x3f)<<12)+
                ((b1&0x3f)<<6)+(b2&0x3f);
            else
              u= (byte)'?';
            }
          }
        }
      write__char_D('&') write__char_D('#')
      if(u<100) {
        if(u>=10)
          write__char_D(u/10+'0')
        write__char_D(u%10+'0')
        }
      else if(u<1000) {
        write__char_D(u/100+'0')
        write__char_D((u/10)%10+'0')
        write__char_D(u%10+'0')
        }
      else {
        char st[30];

        uint32toa(u,st);
        write_str(st);
        }
      write__char_D(';')
      }  // use numeric encoding
    }
  #undef DD
  #undef D
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

static inline void write_createsfix7o(int32_t v,char* s) {
  // convert a signed 7 decimals fixpoint value into a string;
  // keep trailing zeroes;
  // v: fixpoint value
  // sp[12]: destination string;
  char* s1,*s2,c;
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
  }  // end write_createsfix7o()

static inline void write_createsbbox(int32_t x_min, int32_t y_min,
    int32_t x_max, int32_t y_max, char* s) {
  char y_minS[20], x_minS[20], y_maxS[20], x_maxS[20];
  write_createsfix7o(x_min, x_minS);
  write_createsfix7o(y_min, y_minS);
  write_createsfix7o(x_max, x_maxS);
  write_createsfix7o(y_max, y_maxS);
  s = strcpy(s, x_minS);
  s = strcpy(s, ",");
  s = strcpy(s, y_minS);
  s = strcpy(s, ",");
  s = strcpy(s, x_maxS);
  s = strcpy(s, ",");
  s = strcpy(s, y_maxS);
  } // end write_createsbbox()

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

static inline void write_sfix6o(int32_t v) {
  // write a signed 6 decimals fixpoint value to standard output;
  // keep trailing zeroes;
  char s[20],*s1,*s2,c;
  int i;

  s1= s;
  if(v<0)
    { *s1++= '-'; v= -v; }
  s2= s1;
  i= 6;
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
  }  // end write_sfix6o()

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

static inline void write_createtimestamp(uint64_t v,char* sp) {
  // write a timestamp in OSM format, e.g.: "2010-09-30T19:23:30Z",
  // into a string;
  // v: value of the timestamp;
  // sp[21]: destination string;
  time_t vtime;
  struct tm tm;
  int i;

  vtime= v;
  #if __WIN32__
  memcpy(&tm,gmtime(&vtime),sizeof(tm));
  #else
  gmtime_r(&vtime,&tm);
  #endif
  i= tm.tm_year+1900;
  sp+= 3; *sp--= i%10+'0';
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
  }  // end   write_createtimestamp()

static inline void write_timestamp(uint64_t v) {
  // write a timestamp in OSM format, e.g.: "2010-09-30T19:23:30Z"
  char s[30];

  write_createtimestamp(v,s);
  write_str(s);
  }  // end   write_timestamp()

//------------------------------------------------------------
// end   Module write_   write module
//------------------------------------------------------------
