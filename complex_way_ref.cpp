
//------------------------------------------------------------
// Module cww_   complex way ref temporary module
//------------------------------------------------------------

// this module provides procedures to use a temporary file for
// storing a list of ways which have to be marked as 'inside';
// this is used if option --complex-ways is invoked;
// as usual, all identifiers of a module have the same prefix,
// in this case 'posi'; an underline will follow for a global
// accessible identifier, two underlines if the identifier
// is not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----

static char cww__filename[400]= "";
static int cww__fd= -1;  // file descriptor for temporary file
#define cww__bufM 400000
static int64_t cww__buf[cww__bufM],
  *cww__bufp,*cww__bufe,*cww__bufee;
  // buffer - used for write, and later for read;
static bool cww__writemode;  // buffer is used for writing

static inline void cww__flush() {
  if(!cww__writemode || cww__bufp==cww__buf)
return;
  write(cww__fd,cww__buf,(char*)cww__bufp-(char*)cww__buf);
  cww__bufp= cww__buf;
  }  // end   cww__flush()

static inline void cww__write(int64_t i) {
  // write an int64 to tempfile, use a buffer;
  if(cww__bufp>=cww__bufee) cww__flush();
  *cww__bufp++= i;
  }  // end   cww__write()

static void cww__end() {
  // clean-up for temporary file access;
  // will be called automatically at program end;
  if(cww__fd>2) {
    close(cww__fd);
    cww__fd= -1;
    }
  if(loglevel<2) unlink(cww__filename);
  }  // end   cww__end()

//------------------------------------------------------------

static int cww_ini(const char* filename) {
  // open a temporary file with the given name for random r/w access;
  // return: ==0: ok; !=0: error;
  strcpy(stpmcpy(cww__filename,filename,sizeof(cww__filename)-2),".5");
  if(cww__fd>=0)  // file already open
return 0;  // ignore this call
  unlink(cww__filename);
  cww__fd= open(cww__filename,O_RDWR|O_CREAT|O_TRUNC|O_BINARY,00600);
  if(cww__fd<0) {
    PERRv("could not open temporary file: %.80s",cww__filename)
return 1;
    }
  atexit(cww__end);
  cww__bufee= cww__buf+cww__bufM;
  cww__bufp= cww__bufe= cww__buf;
  cww__writemode= true;
  return 0;
  }  // end   cww_ini()

static inline void cww_ref(int64_t refid) {
  // store the id of a referenced way in tempfile;
  cww__write(refid);
  } // end   cww_ref()

static int cww_rewind() {
  // rewind the file pointer;
  // return: ==0: ok; !=0: error;
  if(cww__writemode) {
    cww__flush(); cww__writemode= false; }
  if(lseek(cww__fd,0,SEEK_SET)<0) {
    PERRv("osmconvert Error: could not rewind temporary file %.80s",
      cww__filename)
return 1;
    }
  cww__bufp= cww__bufe= cww__buf;
  return 0;
  } // end   cww_rewind()

static inline int cww_read(int64_t* ip) {
  // read the id of next referenced node;
  // return: ==0: ok; !=0: eof;
  int r,r2;

  if(cww__bufp>=cww__bufe) {
    r= read(cww__fd,cww__buf,sizeof(cww__buf));
    if(r<=0)
return 1;
    cww__bufe= (int64_t*)((char*)cww__buf+r);
    if((r%8)!=0) { // odd number of bytes
      r2= read(cww__fd,cww__bufe,8-(r%8));
        // request the missing bytes
      if(r2<=0)  // did not get the missing bytes
        cww__bufe= (int64_t*)((char*)cww__bufe-(r%8));
      else
        cww__bufe= (int64_t*)((char*)cww__bufe+r2);
      }
    cww__bufp= cww__buf;
    }
  *ip= *cww__bufp++;
  return 0;
  }  // end   cww_read()

static void cww_processing_set() {
  // process temporary way reference file;
  // the file must already have been written; this procedure
  // sets the a flag in hash table (module hash_) for each way
  // which is referred to by an entry in the temporary file;
  int64_t id;  // way id;

  if(cww__filename[0]==0)  // not initialized
return;
  if(cww_rewind())  // could not rewind
return;
  for(;;) {  // get next id
    if(cww_read(&id))
  break;
    hash_seti(1,id);
    }
  }  // end   cww_processing_set()

static void cww_processing_clear() {
  // process temporary way reference file;
  // the file must already have been written; this procedure
  // clears the a flag in hash table (module hash_) for each way
  // which is referred to by an entry in the temporary file;
  int64_t id;  // way id;

  if(cww__filename[0]==0)  // not initialized
return;
  if(cww_rewind())  // could not rewind
return;
  for(;;) {  // get next id
    if(cww_read(&id))
  break;
    hash_cleari(1,id);
    }
  }  // end   cww_processing_clear()

//------------------------------------------------------------
// end   Module cww_   complex way ref temporary module
//------------------------------------------------------------
