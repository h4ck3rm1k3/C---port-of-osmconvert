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
#define read_GZ 3  // determines which read procedure set will be used;
  // ==0: use open();  ==1: use fopen();
  // ==2: use gzopen() (accept gzip compressed input files);
  // ==3: use gzopen() with increased gzip buffer;
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
static bool read__jumplock= false;  // do not change .jumppos anymore;

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
  // note that you should close every opened file with read_close()
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
  #if read_GZ==0
    read_infop->fd= 0;  // (default) standard input
  #else
    read_infop->fi= NULL;  // (default) file not opened
  #endif
  if((read_infop->isstdin= filename==NULL))
    strcpy(read_infop->filename,"standard input");
  else
    strMcpy(read_infop->filename,filename);
  read_infop->eof= false;  // we are not at the end of input file
  read_infop->bufp= read_infop->bufe= read__buf;  // pointer in buf[]
    // pointer to the end of valid input in buf[]
  read_infop->counter= 0;
  read_infop->jumppos= 0;
    // store start of file as default jump destination

  // set modul-global variables which are associated with this file
  read_bufp= read_infop->bufp;
  read_bufe= read_infop->bufe;

  // open the file
  if(loglevel>=2)
    fprintf(stderr,"Read-opening: %s\n",read_infop->filename);
  if(read_infop->isstdin) {  // stdin shall be used
    #if read_GZ==0
      read_infop->fd= 0;
    #elif read_GZ==1
      read_infop->fi= stdin;
    #else
      read_infop->fi= gzdopen(0,"rb");
      #if read_GZ==3 && ZLIB_VERNUM>=0x1235
        gzbuffer(read_infop->fi,128*1024);
      #endif
    #endif
    }
  else if(filename!=NULL) {  // a real file shall be opened
    #if read_GZ==0
      read_infop->fd= open(filename,O_RDONLY|O_BINARY);
    #elif read_GZ==1
      read_infop->fi= fopen(filename,"rb");
    #else
      read_infop->fi= gzopen(filename,"rb");
      #if read_GZ==3 && ZLIB_VERNUM>=0x1235
        if(loglevel>=2)
          fprintf(stderr,"Read-opening: increasing gzbuffer.\n");
        gzbuffer(read_infop->fi,128*1024);
      #endif
    #endif
    #if read_GZ==0
    if(read_infop->fd<0) {
    #else
    if(read_infop->fi==NULL) {
    #endif
      fprintf(stderr,
        "osmconvert Error: could not open input file: %.80s\n",
        read_infop->filename);
      free(read_infop); read_infop= NULL;
      read_bufp= read_bufe= NULL;
return 1;
      }
    }  // end   a real file shall be opened
return 0;
  }  // end   read_open()

static void read_close() {
  // close an opened file;
  // read_infop: handle of the file which is to close;
  if(read_infop==NULL)  // handle not valid;
return;
  if(loglevel>=2)
    fprintf(stderr,"Read-closing: %s\n",read_infop->filename);
  #if read_GZ==0
    if(read_infop->fd>0)  // not standard input
      close(read_infop->fd);
  #elif read_GZ==1
    if(!read_infop->isstdin)  // not standard input
      fclose(read_infop->fi);
  #else
    gzclose(read_infop->fi);
  #endif
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
        #if read_GZ==0
          r= read(read_infop->fd,read_bufe,l);
        #elif read_GZ==1
          r= read(fileno(read_infop->fi),read_bufe,l);
        #else
          r= gzread(read_infop->fi,read_bufe,l);
        #endif
        if(r<=0) {  // no more bytes in the file
          read_infop->eof= true;
            // memorize that there we are at end of file
          l= (read__buf+read__bufM)-read_bufe;
            // reminding space in buffer
          if(l>read_PREFETCH) l= read_PREFETCH;
          memset(read_bufe,0,l);  // 2011-12-24
            // set reminding space up to prefetch bytes in buffer to 0
      break;
          }
        read_infop->counter+= r;
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

static inline int read_rewind() {
  // rewind the file, i.e., the file pointer is set
  // to the first byte in the file;
  // read_infop: handle of the file which is to rewind;
  // return: ==0: ok; !=0: rewind error;
  bool err;

  #if read_GZ==0
    err= lseek(read_infop->fd,0,SEEK_SET)<0;
  #elif read_GZ==1
    err= fseek(read_infop->fi,0,SEEK_SET)<0;
  #else
    err= gzseek(read_infop->fi,0,SEEK_SET)<0;
  #endif
  if(err) {
    PERRv("could not rewind file: %-80s",read_infop->filename)
return 1;
    }
  read_infop->counter= 0;
  read_bufp= read_bufe;  // force refetch
  read_infop->eof= false;  // force retest for end of file
  read_input();  // ensure prefetch
return 0;
  }  // end   read_rewind()

static inline bool read_setjump() {
  // store the current position in the file as a destination
  // for a jump which will follow later;
  // if global_complexways is false, the call will be ignored;
  // the position is not stored anew if it has been locked
  // with read_infop->lockpos;
  // return: jump position has been stored;
  if(!global_complexways)
return false;
  if(read__jumplock)
return false;
  read_infop->jumppos= read_infop->counter-(read_bufe-read_bufp);
  return true;
  }  // end   read_setjump()

static inline void read_lockjump() {
  // prevent a previously stored jump position from being overwritten;
  read__jumplock= true;
  }  // end   read_lockjump()

static int read_jump() {
  // jump to a previously stored location it;
  // return: 0: jump ok;
  //         1: did not actually jump because we already were
  //            at the desired position;
  //         <0: error;
  #if read_GZ<2
    off_t pos;  // present position in the file;
  #else
    #if __WIN32__
      z_off64_t pos;  // position to jump to; -1: invalid
    #else
      z_off_t pos;  // position to jump to; -1: invalid
    #endif
  #endif
  bool err;

  pos= read_infop->counter-(read_bufe-read_bufp);
  if(read_infop->jumppos==-1) {
    PERRv("no jump destination in file: %.80s",read_infop->filename)
return -1;
    }
  #if read_GZ==0
    err= lseek(read_infop->fd,read_infop->jumppos,SEEK_SET)<0;
  #elif read_GZ==1
    err= fseek(read_infop->fi,read_infop->jumppos,SEEK_SET)<0;
  #else
    err= gzseek(read_infop->fi,read_infop->jumppos,SEEK_SET)<0;
  #endif
  if(err) {
    PERRv("could not jump in file: %.80s",read_infop->filename)
return -2;
    }
  if(read_infop->jumppos!=pos) {  // this was a real jump
    read_infop->counter= read_infop->jumppos;
    read_bufp= read_bufe;  // force refetch
    read_infop->eof= false;  // force retest for end of file
    read_input();  // ensure prefetch
return 0;
    }
  // here: did not actually jump because we already were
  // at the desired position
return 1;
  }  // end   read_jump()

//------------------------------------------------------------
// end Module read_   OSM file read module
//------------------------------------------------------------
