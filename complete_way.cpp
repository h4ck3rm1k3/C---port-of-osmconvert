//------------------------------------------------------------
// Module cwn_   complete way ref temporary module
//------------------------------------------------------------

// this module provides procedures to use a temporary file for
// storing a list of nodes which have to be marked as 'inside';
// this is used if option --complete-ways is invoked;
// as usual, all identifiers of a module have the same prefix,
// in this case 'posi'; an underline will follow for a global
// accessible identifier, two underlines if the identifier
// is not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----

static char cwn__filename[400]= "";
static int cwn__fd= -1;  // file descriptor for temporary file
#define cwn__bufM 400000
static int64_t cwn__buf[cwn__bufM],
  *cwn__bufp,*cwn__bufe,*cwn__bufee;
  // buffer - used for write, and later for read;
static bool cwn__writemode;  // buffer is used for writing

static inline void cwn__flush() {
  if(!cwn__writemode || cwn__bufp==cwn__buf)
return;
  write(cwn__fd,cwn__buf,(char*)cwn__bufp-(char*)cwn__buf);
  cwn__bufp= cwn__buf;
  }  // end   cwn__flush()

static inline void cwn__write(int64_t i) {
  // write an int64 to tempfile, use a buffer;
  if(cwn__bufp>=cwn__bufee) cwn__flush();
  *cwn__bufp++= i;
  }  // end   cwn__write()

static void cwn__end() {
  // clean-up for temporary file access;
  // will be called automatically at program end;
  if(cwn__fd>2) {
    close(cwn__fd);
    cwn__fd= -1;
    }
  if(loglevel<2) unlink(cwn__filename);
  }  // end   cwn__end()

//------------------------------------------------------------

static int cwn_ini(const char* filename) {
  // open a temporary file with the given name for random r/w access;
  // return: ==0: ok; !=0: error;
  strcpy(stpncpy(cwn__filename,filename,sizeof(cwn__filename)-2),".3");
  if(cwn__fd>=0)  // file already open
return 0;  // ignore this call
  unlink(cwn__filename);
  cwn__fd= open(cwn__filename,O_RDWR|O_CREAT|O_TRUNC|O_BINARY,00600);
  if(cwn__fd<0) {
    PERRv("could not open temporary file: %.80s",cwn__filename)
return 1;
    }
  atexit(cwn__end);
  cwn__bufee= cwn__buf+cwn__bufM;
  cwn__bufp= cwn__bufe= cwn__buf;
  cwn__writemode= true;
  return 0;
  }  // end   cwn_ini()

static inline void cwn_ref(int64_t refid) {
  // store the id of a referenced node in tempfile;
  cwn__write(refid);
  } // end   cwn_ref()

static int cwn_rewind() {
  // rewind the file pointer;
  // return: ==0: ok; !=0: error;
  if(cwn__writemode) {
    cwn__flush(); cwn__writemode= false; }
  if(lseek(cwn__fd,0,SEEK_SET)<0) {
    PERRv("osmconvert Error: could not rewind temporary file %.80s",
      cwn__filename)
return 1;
    }
  cwn__bufp= cwn__bufe= cwn__buf;
  return 0;
  } // end   cwn_rewind()

static inline int cwn_read(int64_t* ip) {
  // read the id of next referenced node;
  // return: ==0: ok; !=0: eof;
  int r,r2;

  if(cwn__bufp>=cwn__bufe) {
    r= read(cwn__fd,cwn__buf,sizeof(cwn__buf));
    if(r<=0)
return 1;
    cwn__bufe= (int64_t*)((char*)cwn__buf+r);
    if((r%8)!=0) { // odd number of bytes
      r2= read(cwn__fd,cwn__bufe,8-(r%8));
        // request the missing bytes
      if(r2<=0)  // did not get the missing bytes
        cwn__bufe= (int64_t*)((char*)cwn__bufe-(r%8));
      else
        cwn__bufe= (int64_t*)((char*)cwn__bufe+r2);
      }
    cwn__bufp= cwn__buf;
    }
  *ip= *cwn__bufp++;
  return 0;
  }  // end   cwn_read()

static void cwn_processing() {
  // process temporary node reference file;
  // the file must already have been written; this procedure
  // sets the a flag in hash table (module hash_) for each node
  // which is referred to by an entry in the temporary file;
  int64_t id;  // node id;

  if(cwn_rewind())  // could not rewind
return;
  for(;;) {  // get next id
    if(cwn_read(&id))
  break;
    hash_seti(0,id);
    }
  }  // end   cwn_processing()

//------------------------------------------------------------
// end   Module cwn_   complete way ref temporary module
//------------------------------------------------------------
