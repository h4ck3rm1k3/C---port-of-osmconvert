
//------------------------------------------------------------
// Module rr_   relref temporary module
//------------------------------------------------------------

// this module provides procedures to use a temporary file for
// storing relation's references;
// as usual, all identifiers of a module have the same prefix,
// in this case 'rr'; an underline will follow in case of a
// global accessible object, two underlines in case of objects
// which are not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----

static char rr__filename[400]= "";
static int rr__fd= -1;  // file descriptor for temporary file
#define rr__bufM 400000
static int64_t rr__buf[rr__bufM],*rr__bufp,*rr__bufe,*rr__bufee;
  // buffer - used for write, and later for read;
static bool rr__writemode;  // buffer is used for writing

static inline void rr__flush() {
  if(!rr__writemode || rr__bufp==rr__buf)
return;
  write(rr__fd,rr__buf,(char*)rr__bufp-(char*)rr__buf);
  rr__bufp= rr__buf;
  }  // end   rr__flush()

static inline void rr__write(int64_t i) {
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
  strcpy(stpncpy(rr__filename,filename,sizeof(rr__filename)-2),".0");
  if(rr__fd>=0)  // file already open
return 0;  // ignore this call
  unlink(rr__filename);
  rr__fd= open(rr__filename,O_RDWR|O_CREAT|O_TRUNC|O_BINARY,00600);
  if(rr__fd<0) {
    fprintf(stderr,
      "osmconvert Error: could not open temporary file: %.80s\n",
      rr__filename);
return 1;
    }
  atexit(rr__end);
  rr__bufee= rr__buf+rr__bufM;
  rr__bufp= rr__bufe= rr__buf;
  rr__writemode= true;
  return 0;
  }  // end   rr_ini()

static inline void rr_rel(int64_t relid) {
  // store the id of a relation in tempfile;
  rr__write(0);
  rr__write(relid);
  } // end   rr_rel()

static inline void rr_ref(int64_t refid) {
  // store the id of an interrelation reference in tempfile;
  rr__write(refid);
  } // end   rr_ref()

static int rr_rewind() {
  // rewind the file pointer;
  // return: ==0: ok; !=0: error;
  if(rr__writemode) {
    rr__flush(); rr__writemode= false; }
  if(lseek(rr__fd,0,SEEK_SET)<0) {
    fprintf(stderr,"osmconvert Error: could not rewind temporary file"
      " %.80s\n",rr__filename);
return 1;
    }
  rr__bufp= rr__bufe= rr__buf;
  return 0;
  } // end   rr_rewind()

static inline int rr_read(int64_t* ip) {
  // read one integer; meaning of the values of these integers:
  // every value is an interrelation reference id, with one exception:
  // integers which follow a 0-integer directly are relation ids;
  // note that we take 64-bit-integers although the number of relations
  // will never exceed 2^31; the reason is that Free OSM ("FOSM") uses
  // IDs > 2^16 for new data which adhere the cc-by-sa license;
  // return: ==0: ok; !=0: eof;
  int r,r2;

  if(rr__bufp>=rr__bufe) {
    r= read(rr__fd,rr__buf,sizeof(rr__buf));
    if(r<=0)
return 1;
    rr__bufe= (int64_t*)((char*)rr__buf+r);
    if((r%8)!=0) { // odd number of bytes
      r2= read(rr__fd,rr__bufe,8-(r%8));  // request the missing bytes
      if(r2<=0)  // did not get the missing bytes
        rr__bufe= (int64_t*)((char*)rr__bufe-(r%8));
      else
        rr__bufe= (int64_t*)((char*)rr__bufe+r2);
      }
    rr__bufp= rr__buf;
    }
  *ip= *rr__bufp++;
  return 0;
  }  // end   rr_read()

//------------------------------------------------------------
// end   Module rr_   relref temporary module
//------------------------------------------------------------
