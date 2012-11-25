
//------------------------------------------------------------
// Module pw_   PBF write module
//------------------------------------------------------------

// this module provides procedures which write .pbf objects;
// it uses procedures from module write_;
// as usual, all identifiers of a module have the same prefix,
// in this case 'pw'; an underline will follow in case of a
// global accessible object, two underlines in case of objects
// which are not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----

static int pw__compress(byte* ibuf,uint isiz,byte* obuf,uint osizm,
  uint* osizp) {
  // compress a block of data;
  // return: 0: compression was successful;
  //         !=0: error number from zlib;
  // *osizp: size of compressed data;
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
  r= deflateInit(&strm,Z_DEFAULT_COMPRESSION);
  if(r!=Z_OK)
return r;

  // read data
  strm.next_in = ibuf;
  strm.avail_in= isiz;

  // compress
  strm.next_out= obuf;
  strm.avail_out= osizm;
  r= deflate(&strm,Z_FINISH);
  if(/*r!=Z_OK &&*/ r!=Z_STREAM_END) {
    deflateEnd(&strm);
    *osizp= 0;
    if(r==0) r= 1000;
return r;
    }

  // clean-up
  deflateEnd(&strm);
  obuf+= *osizp= osizm-(i= strm.avail_out);

  // add some zero bytes
  if(i>4) i= 4;
  while(--i>=0) *obuf++= 0;
  return 0;
  }  // end   pw__compress()

// format description: BlobHeader must be less than 64 kilobytes;
// uncompressed length of a Blob must be less than 32 megabytes;

#define pw__compress_bufM (UINT64_C(35) Megabytes)
static byte* pw__compress_buf= NULL;  // buffer for compressed objects

#define pw__bufM (UINT64_C(160) Megabytes)
static byte* pw__buf= NULL;  // buffer for objects in .pbf format
static byte* pw__bufe= NULL;  // logical end of the buffer
static byte* pw__bufee= NULL;  // physical end of the buffer

typedef struct pw__obj_struct {  // type of a pbf hierarchy object
  //struct pw__obj_struct parent;  // parent object; ==NULL: none;
  byte* buf;  // start address of pbf buffer for this hierarchy object;
    // this is where the header starts too;
  int headerlen;  // usually .bufl-.buf;
  byte* bufl;  // start address of object's length
  byte* bufc;  // start address of object's contents
  byte* bufe;  // write pointer in the pbf buffer
  byte* bufee;  // end address of pbf buffer for this hierarchy object
  } pw__obj_t;

#define pw__objM 20
static pw__obj_t pw__obj[pw__objM];
static pw__obj_t* pw__obje= pw__obj;
  // logical end of the object hierarchy array
static pw__obj_t *pw__objee= pw__obj+pw__objM;
  // physical end of the object hierarchy array
static pw__obj_t* pw__objp= NULL;  // currently active hierarchy object

static inline pw__obj_t* pw__obj_open(const char* header) {
  // open a new hierarchy level
  // header[20]: header which is to be written prior to the
  //           contents length; zero-terminated;
  pw__obj_t* op;

  if(pw__obje==pw__obj) {  // first hierarchy object
    pw__bufe= pw__buf;
    //pw__obje->parent= NULL;
    pw__obje->buf= pw__bufe;
    }
  else {  // not the first hierarchy object
    if(pw__obje>=pw__objee) {  // no space left in hierarchy array
      PERR("PBF write: hierarchy overflow.")
return pw__objp;
      }
    op= pw__obje-1;
    if(op->bufee==pw__bufee) {  // object is not a limited one
      pw__obje->buf= op->bufe;
      }
    else  // object is a limited one
      pw__obje->buf= op->bufee;
    if(pw__obje->buf+50>pw__bufee) {  // no space left PBF object buffer
      PERR("PBF write: object buffer overflow.")
return pw__objp;
      }
    }  // not the first hierarchy object
  pw__objp= pw__obje++;
  // write PBF object's header and pointers
  pw__objp->bufl= (byte*)stpncpy((char*)pw__objp->buf,header,20);
  pw__objp->headerlen= (int)(pw__objp->bufl-pw__objp->buf);
  pw__objp->bufc= pw__objp->bufl+10;
  pw__objp->bufe= pw__objp->bufc;
  pw__objp->bufee= pw__bufee;
  return pw__objp;
  }  // pw__obj_open()

static inline void pw__obj_limit(int size) {
  // limit the maximum size of an PBF hierarchy object;
  // this is necessary if two or more PBF objects shall be written
  // simultaneously, e.g. when writing dense nodes;

  if(size>pw__objp->bufee-pw__objp->bufc-50) {
    PERRv("PBF write: object buffer limit too large: %i>%i.",
      size,(int)(pw__objp->bufee-pw__objp->bufc-50))
return;
    }
  pw__objp->bufee= pw__objp->bufc+size;
  }  // pw__obj_limit()

static inline void pw__obj_limit_parent(pw__obj_t* parent) {
  // limit the size of a PBF hierarchy parent object to the
  // sum of the maximum sizes of its children;
  // parent: must point to the parent object;
  // pw__objp: must point to the last child of the parent;
  parent->bufee= pw__objp->bufee;
  }  // pw__obj_limit_parent()

static inline void pw__obj_compress() {
  // compress the contents of the current PBF hierarchy object;
  // pw__objp: pointer to current object;
  int r;
  unsigned int osiz;  // size of the compressed contents

  r= pw__compress(pw__objp->bufc,pw__objp->bufe-pw__objp->bufc,
    pw__compress_buf,pw__compress_bufM,&osiz);
  if(r!=0) {  // an error has occurred
    PERRv("PBF write: compression error %i.",r)
return;
    }
  if(osiz>pw__objp->bufee-pw__objp->bufc) {
    PERRv("PBF write: compressed contents too large: %i>%i.",
      osiz,(int)(pw__objp->bufee-pw__objp->bufc))
return;
    }
  memcpy(pw__objp->bufc,pw__compress_buf,osiz);
  pw__objp->bufe= pw__objp->bufc+osiz;
  }  // pw__obj_compress()

static inline void pw__obj_add_id(uint8_t pbfid) {
  // append a one-byte PBF id to PBF write buffer;
  // pbfid: PBF id;
  // pw__objp->bufe: write buffer position (will be
  //                 incremented by this procedure);
  if(pw__objp->bufe>=pw__objp->bufee) {
    PERR("PBF write: id memory overflow.")
return;
    }
  *pw__objp->bufe++= pbfid;
  }  // pw__obj_add_id()

static inline void pw__obj_add_id2(uint16_t pbfid) {
  // append a two-byte PBF id to PBF write buffer;
  // pbfid: PBF id, high byte is stored first;
  // pw__objp->bufe: write buffer position (will be
  //                 incremented by 2 by this procedure);
  if(pw__objp->bufe+2>pw__objp->bufee) {
    PERR("PBF write: id2 memory overflow.")
return;
    }
  *pw__objp->bufe++= (byte)(pbfid>>8);
  *pw__objp->bufe++= (byte)(pbfid&0xff);
  }  // pw__obj_add_id2()

static inline void pw__obj_add_uint32(uint32_t v) {
  // append a numeric value to PBF write buffer;
  // pw__objp->bufe: write buffer position
  //                 (will be updated by this procedure);
  uint32_t frac;

  if(pw__objp->bufe+10>pw__objp->bufee) {
    PERR("PBF write: uint32 memory overflow.")
return;
    }
  frac= v&0x7f;
  while(frac!=v) {
    *pw__objp->bufe++= frac|0x80;
    v>>= 7;
    frac= v&0x7f;
    }
  *pw__objp->bufe++= frac;
  }  // pw__obj_add_uint32()

static inline void pw__obj_add_sint32(int32_t v) {
  // append a numeric value to PBF write buffer;
  // pw__objp->bufe: write buffer position
  //                 (will be updated by this procedure);
  uint32_t u;
  uint32_t frac;

  if(pw__objp->bufe+10>pw__objp->bufee) {
    PERR("PBF write: sint32 memory overflow.")
return;
    }
  if(v<0) {
    u= -v;
    u= (u<<1)-1;
    }
  else
    u= v<<1;
  frac= u&0x7f;
  while(frac!=u) {
    *pw__objp->bufe++= frac|0x80;
    u>>= 7;
    frac= u&0x7f;
    }
  *pw__objp->bufe++= frac;
  }  // pw__obj_add_sint32()

static inline void pw__obj_add_uint64(uint64_t v) {
  // append a numeric value to PBF write buffer;
  // pw__objp->bufe: write buffer position
  //                 (will be updated by this procedure);
  uint32_t frac;

  if(pw__objp->bufe+10>pw__objp->bufee) {
    PERR("PBF write: uint64 memory overflow.")
return;
    }
  frac= v&0x7f;
  while(frac!=v) {
    *pw__objp->bufe++= frac|0x80;
    v>>= 7;
    frac= v&0x7f;
    }
  *pw__objp->bufe++= frac;
  }  // pw__obj_add_uint64()

static inline void pw__obj_add_sint64(int64_t v) {
  // append a numeric value to PBF write buffer;
  // pw__objp->bufe: write buffer position
  //                 (will be updated by this procedure);
  uint64_t u;
  uint32_t frac;

  if(pw__objp->bufe+10>pw__objp->bufee) {
    PERR("PBF write: sint64 memory overflow.")
return;
    }
  if(v<0) {
    u= -v;
    u= (u<<1)-1;
    }
  else
    u= v<<1;
  frac= u&0x7f;
  while(frac!=u) {
    *pw__objp->bufe++= frac|0x80;
    u>>= 7;
    frac= u&0x7f;
    }
  *pw__objp->bufe++= frac;
  }  // pw__obj_add_sint64()

#if 0  // not used at present
static inline void pw__obj_add_mem(byte* s,uint32_t sl) {
  // append data to PBF write buffer;
  // s[]: data which are to append;
  // ls: length of the data;
  // pw__objp->bufe: write buffer position
  //                 (will be updated by this procedure);

  if(pw__objp->bufe+sl>pw__objp->bufee) {
    PERR("PBF write: mem memory overflow.")
return;
    }
  memcpy(pw__objp->bufe,s,sl);
  pw__objp->bufe+= sl;
  }  // pw__obj_add_mem()
#endif

static inline void pw__obj_add_str(const char* s) {
  // append a PBF string to PBF write buffer;
  // pw__objp->bufe: write buffer position
  //                 (will be updated by this procedure);
  uint32_t sl;  // length of the string

  sl= strlen(s);
  if(pw__objp->bufe+10+sl>pw__objp->bufee) {
    PERR("PBF write: string memory overflow.")
return;
    }
  /* write the string length into PBF write buffer */ {
    uint32_t v,frac;

    v= sl;
    frac= v&0x7f;
    while(frac!=v) {
      *pw__objp->bufe++= frac|0x80;
      v>>= 7;
      frac= v&0x7f;
      }
    *pw__objp->bufe++= frac;
    }  // write the string length into PBF write buffer
  memcpy(pw__objp->bufe,s,sl);
  pw__objp->bufe+= sl;
  }  // pw__obj_add_str()

static inline void pw__obj_close() {
  // close an object which had been opened with pw__obj_open();
  // pw__objp: pointer to the object which is to close;
  // return:
  // pw__objp: points to the last opened object;
  pw__obj_t* op;
  int i;
  byte* bp;
  uint32_t len;
  uint32_t v,frac;

  if(pw__objp==pw__obj) {  // this is the anchor object
    // write the object's data to standard output
    write_mem(pw__objp->buf,pw__objp->headerlen);  // write header
    write_mem(pw__objp->bufc,(int)(pw__objp->bufe-pw__objp->bufc));
      // write contents
    // delete hierarchy object
    pw__objp= NULL;
    pw__obje= pw__obj;
return;
    }

  // determine the parent object
  op= pw__objp;
  for(;;) {  // search for the parent object
    if(op<=pw__obj) {  // there is no parent object
      PERR("PBF write: no parent object.")
return;
      }
    op--;
    if(op->buf!=NULL)  // found our parent object
  break;
    }

  // write PBF object's header into parent object
  bp= pw__objp->buf;
  i= pw__objp->headerlen;
  while(--i>=0)
    *op->bufe++= *bp++;

  // write PBF object's length into parent object
  len= v= pw__objp->bufe-pw__objp->bufc;
  frac= v&0x7f;
  while(frac!=v) {
    *op->bufe++= frac|0x80;
    v>>= 7;
    frac= v&0x7f;
    }
  *op->bufe++= frac;

  // write PBF object's contents into parent object
  memmove(op->bufe,pw__objp->bufc,len);
  op->bufe+= len;

  // mark this object as deleted
  pw__objp->buf= NULL;

  // free the unused space in object hierarchy array
  while(pw__obje>pw__obj && pw__obje[-1].buf==NULL) pw__obje--;
  pw__objp= pw__obje-1;
  }  // pw__obj_close()

static inline void pw__obj_dispose() {
  // dispose an object which had been opened with pw__obj_open();
  // pw__objp: pointer to the object which is to close;
  // return:
  // pw__objp: points to the last opened object;
  if(pw__objp==pw__obj) {  // this is the anchor object
    // delete hierarchy object
    pw__objp= NULL;
    pw__obje= pw__obj;
return;
    }

  // mark this object as deleted
  pw__objp->buf= NULL;

  // free the unused space in object hierarchy array
  while(pw__obje>pw__obj && pw__obje[-1].buf==NULL) pw__obje--;
  pw__objp= pw__obje-1;
  }  // pw__obj_dispose()

static pw__obj_t* pw__st= NULL,*pw__dn_id= NULL,*pw__dn_his,
  *pw__dn_hisver= NULL,*pw__dn_histime= NULL,*pw__dn_hiscset= NULL,
  *pw__dn_hisuid= NULL,*pw__dn_hisuser= NULL,
  *pw__dn_lat= NULL,*pw__dn_lon= NULL,*pw__dn_keysvals= NULL;

// some variables for delta coding
static int64_t pw__dc_id= 0;
static int32_t pw__dc_lon= 0,pw__dc_lat= 0;
static int64_t pw__dc_histime= 0;
static int64_t pw__dc_hiscset= 0;
static uint32_t pw__dc_hisuid= 0;
static uint32_t pw__dc_hisuser= 0;
static int64_t pw__dc_noderef= 0;
static int64_t pw__dc_ref= 0;

static void pw__data(int otype) {
  // prepare or complete an 'OSMData fileblock';
  // should be called prior to writing each OSM object;
  // otype: type of the OSM object which is going to be written;
  //        0: node; 1: way; 2: relation; -1: none;
  static int otype_old= -1;
  static const int max_object_size= (250 kilobytes);
    // assumed maximum size of one OSM object
  #define pw__data_spaceM (31 Megabytes)
    // maximum size of one 'fileblock'
  static int used_space= pw__data_spaceM;
    // presently used memory space in present 'OSMData fileblock',
    // not including the strings
  int string_space;  // memory space used by strings
  int remaining_space;
    // remaining memory space in present 'OSMData fileblock'
  int i;

  // determine remaining space in current 'OSMData fileblock';
  // the remaining space is usually guessed in a pessimistic manner;
  // if this estimate shows too less space, then a more exact
  // calculation is made;
  // this strategy has been chosen for performance reasons;
  used_space+= 64000;  // increase used-space variable by the assumed
    // maximum size of one OSM object, not including the strings
  string_space= pstw_queryspace();
  remaining_space= pw__data_spaceM-used_space-string_space;
  if(remaining_space<max_object_size) {  // might be too less space
    // calculate used space more exact
    if(otype_old==0) {  // node
      used_space= (int)((pw__dn_id->bufe-pw__dn_id->buf)+
        (pw__dn_lat->bufe-pw__dn_lat->buf)+
        (pw__dn_lon->bufe-pw__dn_lon->buf)+
        (pw__dn_keysvals->bufe-pw__dn_keysvals->buf));
      if(!global_dropversion) {
        used_space+= (int)(pw__dn_hisver->bufe-pw__dn_hisver->buf);
        if(!global_dropauthor) {
          used_space+= (int)((pw__dn_histime->bufe-pw__dn_histime->buf)+
            (pw__dn_hiscset->bufe-pw__dn_hiscset->buf)+
            (pw__dn_hisuid->bufe-pw__dn_hisuid->buf)+
            (pw__dn_hisuser->bufe-pw__dn_hisuser->buf));
          }
        }
      }
    else if(otype_old>0)  // way or relation
      used_space= (int)(pw__objp->bufe-pw__objp->buf);
    remaining_space= pw__data_spaceM-used_space-string_space;
    }  // might be too less space

  // conclude or start an 'OSMData fileblock'
  if(otype!=otype_old || remaining_space<max_object_size) {
      // 'OSMData fileblock' must be concluded or started
    if(otype_old>=0) {  // there has been object processing
      // complete current 'OSMData fileblock'
      used_space= pw__data_spaceM;  // force new calculation next time
      i= pstw_queryspace();
      if(i>pw__st->bufee-pw__st->bufe)
        PERR("PBF write: string table memory overflow.")
      else
        pstw_write(&pw__st->bufe);
      pw__objp= pw__st; pw__obj_close();  // 'stringtable'
      switch(otype_old) {  // select by OSM object type
      case 0:  // node
        pw__objp= pw__dn_id; pw__obj_close();
        if(!global_dropversion) {  // version number is to be written
          pw__objp= pw__dn_hisver; pw__obj_close();
          if(!global_dropauthor) {  // author information  is to be written
            pw__objp= pw__dn_histime; pw__obj_close();
            pw__objp= pw__dn_hiscset; pw__obj_close();
            pw__objp= pw__dn_hisuid; pw__obj_close();
            pw__objp= pw__dn_hisuser; pw__obj_close();
            }  // author information  is to be written
          pw__objp= pw__dn_his; pw__obj_close();
          }  // version number is to be written
        pw__objp= pw__dn_lat; pw__obj_close();
        pw__objp= pw__dn_lon; pw__obj_close();
        pw__objp= pw__dn_keysvals; pw__obj_close();
        pw__obj_close();  // 'dense'
        break;
      case 1:  // way
        break;
      case 2:  // relation
        break;
        }  // select by OSM object type
      pw__obj_close();  // 'primitivegroup'
      /* write 'raw_size' into hierarchy object's header */ {
        uint32_t v,frac;
        byte* bp;

        v= pw__objp->bufe-pw__objp->bufc;
        bp= pw__objp->buf+1;
        frac= v&0x7f;
        while(frac!=v) {
          *bp++= frac|0x80;
          v>>= 7;
          frac= v&0x7f;
          }
        *bp++= frac;
        *bp++= 0x1a;
        pw__objp->headerlen= bp-pw__objp->buf;
        }
      pw__obj_compress();
      pw__obj_close();  // 'zlib_data'
      pw__obj_close();  // 'datasize'
      /* write 'length of BlobHeader message' into object's header */ {
        byte* bp;

        bp= pw__objp->bufc+pw__objp->bufc[1]+3;
        while((*bp & 0x80)!=0) bp++;
        bp++;
        pw__objp->buf[0]= pw__objp->buf[1]= pw__objp->buf[2]= 0;
        pw__objp->buf[3]= bp-pw__objp->bufc;
        }
      pw__obj_close();  // 'Blobheader'
      otype_old= -1;
      }  // there has been object processing

    // prepare new 'OSMData fileblock' if necessary
    if(otype!=otype_old) {
      pw__obj_open("----");
        // open anchor hierarchy object for 'OSMData fileblock'
        // (every fileblock starts with four zero-bytes;
        // the fourth zero-byte will be overwritten later
        // by the length of the BlobHeader;)
      pw__obj_add_id(0x0a);  // S 1 'type'
      pw__obj_add_str("OSMData");
      pw__obj_open("\x18");  // V 3 'datasize'
      pw__obj_open("\x10----------\x1a");  // S 3 'zlib_data'
        // in the header: V 2 'raw_size'
      pw__st= pw__obj_open("\x0a");  // S 1 'stringtable'
      pw__obj_limit(30 Megabytes);
      pstw_reset();
      pw__obj_open("\x12");  // S 2 'primitivegroup'
      switch(otype) {  // select by OSM object type
      case 0:  // node
        pw__obj_open("\x12");  // S 2 'dense'
        pw__dn_id= pw__obj_open("\x0a");  // S 1 'id'
        pw__obj_limit(10 Megabytes);
        if(!global_dropversion) {  // version number is to be written
          pw__dn_his= pw__obj_open("\x2a");  // S 5 'his'
          pw__dn_hisver= pw__obj_open("\x0a");  // S 1 'his.ver'
          pw__obj_limit(4 Megabytes);
          if(!global_dropauthor) {  // author information  is to be written
            pw__dn_histime= pw__obj_open("\x12");  // S 2 'his.time'
            pw__obj_limit(10 Megabytes);
            pw__dn_hiscset= pw__obj_open("\x1a");  // S 3 'his.cset'
            pw__obj_limit(10 Megabytes);
            pw__dn_hisuid= pw__obj_open("\x22");  // S 4 'his.uid'
            pw__obj_limit(8 Megabytes);
            pw__dn_hisuser= pw__obj_open("\x2a");  // S 5 'his.user'
            pw__obj_limit(6 Megabytes);
            }  // author information  is to be written
          pw__obj_limit_parent(pw__dn_his);
          }  // version number is to be written
        pw__dn_lat= pw__obj_open("\x42");  // S 8 'lat'
        pw__obj_limit(30 Megabytes);
        pw__dn_lon= pw__obj_open("\x4a");  // S 9 'lon'
        pw__obj_limit(30 Megabytes);
        pw__dn_keysvals= pw__obj_open("\x52");  // S 10 'tags'
        pw__obj_limit(20 Megabytes);
        // reset variables for delta coding
        pw__dc_id= 0;
        pw__dc_lat= pw__dc_lon= 0;
        pw__dc_histime= 0;
        pw__dc_hiscset= 0;
        pw__dc_hisuid= 0;
        pw__dc_hisuser= 0;
        break;
      case 1:  // way
        break;
      case 2:  // relation
        break;
        }  // select by OSM object type
      otype_old= otype;
      }  // prepare new 'OSMData fileblock' if necessary
    }  // 'OSMData fileblock' must be concluded or started
  }  // pw__data()

static void pw__end() {
  // clean-up this module;
  if(pw__obje!=pw__obj)
    PERR("PBF write: object hierarchy still open.")
  if(pw__buf!=NULL) {
    free(pw__buf);
    pw__buf= pw__bufe= pw__bufee= NULL;
    }
  pw__obje= pw__obj;
  pw__objp= NULL;
  if(pw__compress_buf!=NULL) {
    free(pw__compress_buf);
    pw__compress_buf= NULL;
    }
  }  // end   pw__end()

//------------------------------------------------------------

static inline int pw_ini() {
  // initialize this module;
  // must be called before any other procedure is called;
  // return: 0: everything went ok;
  //         !=0: an error occurred;
  static bool firstrun= true;
  int r;

  if(firstrun) {
    firstrun= false;
    atexit(pw__end);
    pw__buf= (byte*)malloc(pw__bufM);
    pw__bufe= pw__buf;
    pw__bufee= pw__buf+pw__bufM;
    pw__compress_buf= (byte*)malloc(pw__compress_bufM);
    r= pstw_ini();
    if(pw__buf==NULL || pw__compress_buf==NULL || r!=0) {
      PERR("PBF write: not enough memory.")
return 1;
      }
    }
  return 0;
  }  // end   pw_ini()

static void pw_header(bool bboxvalid,
    int32_t x1,int32_t y1,int32_t x2,int32_t y2,int64_t timestamp) {
  // start writing PBF objects, i.e., write the 'OSMHeader fileblock';
  // bboxvalid: the following bbox coordinates are valid;
  // x1,y1,x2,y2: bbox coordinates (base 10^-7);
  // timestamp: file timestamp; ==0: no timestamp given;
  pw__obj_open("----");
    // open anchor hierarchy object for 'OSMHeader fileblock'
    // (every fileblock starts with four zero-bytes;
    // the fourth zero-byte will be overwritten later
    // by the length of the BlobHeader;)
  pw__obj_add_id(0x0a);  // S 1 'type'
  pw__obj_add_str("OSMHeader");
  pw__obj_open("\x18");  // V 3 'datasize'
  pw__obj_open("\x10----------\x1a");  // S 3 'zlib_data'
    // in the header: V 2 'raw_size'
  if(bboxvalid) {
    pw__obj_open("\x0a");  // S 1 'bbox'
    pw__obj_add_id(0x08);  // V 1 'minlon'
    pw__obj_add_sint64((int64_t)x1*100);
    pw__obj_add_id(0x10);  // V 2 'maxlon'
    pw__obj_add_sint64((int64_t)x2*100);
    pw__obj_add_id(0x18);  // V 3 'maxlat'
    pw__obj_add_sint64((int64_t)y2*100);
    pw__obj_add_id(0x20);  // V 4 'minlat'
    pw__obj_add_sint64((int64_t)y1*100);
    pw__obj_close();
    }
  pw__obj_add_id(0x22);  // S 4 'required_features'
  pw__obj_add_str("OsmSchema-V0.6");
  pw__obj_add_id(0x22);  // S 4 'required_features'
  pw__obj_add_str("DenseNodes");
  pw__obj_add_id(0x2a);  // S 5 'optional_features'
  pw__obj_add_str("Sort.Type_then_ID");
  if(timestamp!=0) {  // file timestamp given
    char s[40],*sp;

    sp= strcpy(s,"timestamp=");
    write_createtimestamp(timestamp,sp);
    pw__obj_add_id(0x2a);  // S 5 'optional_features'
    pw__obj_add_str(s);
    }  // file timestamp given
  pw__obj_add_id2(0x8201);  // S 16 'writingprogram'
  pw__obj_add_str("osmconvert " VERSION);
  pw__obj_add_id2(0x8a01);  // S 17 'source'
  pw__obj_add_str("http://www.openstreetmap.org/api/0.6");
  /* write 'raw_size' into hierarchy object's header */ {
    uint32_t v,frac;
    byte* bp;

    v= pw__objp->bufe-pw__objp->bufc;
    bp= pw__objp->buf+1;
    frac= v&0x7f;
    while(frac!=v) {
      *bp++= frac|0x80;
      v>>= 7;
      frac= v&0x7f;
      }
    *bp++= frac;
    *bp++= 0x1a;
    pw__objp->headerlen= bp-pw__objp->buf;
    }
  pw__obj_compress();
  pw__obj_close();  // 'zlib_data'
  pw__obj_close();  // 'datasize'
  /* write 'length of BlobHeader message' into object's header */ {
    byte* bp;

    bp= pw__objp->bufc+pw__objp->bufc[1]+3;
    while((*bp & 0x80)!=0) bp++;
    bp++;
    pw__objp->buf[0]= pw__objp->buf[1]= pw__objp->buf[2]= 0;
    pw__objp->buf[3]= bp-pw__objp->bufc;
    }
  pw__obj_close();  // 'Blobheader'
  }  // end   pw_header()

static inline void pw_foot() {
  // end writing a PBF file;
  pw__data(-1);
  }  // end   pw_foot()

static inline void pw_flush() {
  // end writing a PBF dataset;
  pw__data(-1);
  }  // end   pw_flush()

static inline void pw_node(int64_t id,
    int32_t hisver,int64_t histime,int64_t hiscset,
    uint32_t hisuid,const char* hisuser,int32_t lon,int32_t lat) {
  // start writing a PBF dense node dataset;
  // id: id of this object;
  // hisver: version; 0: no author information is to be written
  // histime: time (seconds since 1970)
  // hiscset: changeset
  // hisuid: uid
  // hisuser: user name
  // lon: latitude in 100 nanodegree;
  // lat: latitude in 100 nanodegree;
  int stid;  // string id

  pw__data(0);
  pw__objp= pw__dn_id; pw__obj_add_sint64(id-pw__dc_id);
  pw__dc_id= id;
  if(!global_dropversion) {  // version number is to be written
    if(hisver==0) hisver= 1;
    pw__objp= pw__dn_hisver; pw__obj_add_uint32(hisver);
    if(!global_dropauthor) {  // author information is to be written
      if(histime==0) { histime= 1; hiscset= 1; hisuser= 0; }
      pw__objp= pw__dn_histime;
      pw__obj_add_sint64(histime-pw__dc_histime);
      pw__dc_histime= histime;
      pw__objp= pw__dn_hiscset;
      pw__obj_add_sint64(hiscset-pw__dc_hiscset);
      pw__dc_hiscset= hiscset;
      pw__objp= pw__dn_hisuid;
      pw__obj_add_sint32(hisuid-pw__dc_hisuid);
      pw__dc_hisuid= hisuid;
      pw__objp= pw__dn_hisuser;
      if(hisuid==0) hisuser= "";
      stid= pstw_store(hisuser);
      pw__obj_add_sint32(stid-pw__dc_hisuser);
      pw__dc_hisuser= stid;
      }  // author information  is to be written
    }  // version number is to be written
  pw__objp= pw__dn_lat; pw__obj_add_sint64(lat-pw__dc_lat);
  pw__dc_lat= lat;
  pw__objp= pw__dn_lon;
    pw__obj_add_sint64((int64_t)lon-pw__dc_lon);
  pw__dc_lon= lon;
  }  // end   pw_node()

static inline void pw_node_keyval(const char* key,const char* val) {
  // write node object's keyval;
  int stid;  // string id

  pw__objp= pw__dn_keysvals;
  stid= pstw_store(key);
  pw__obj_add_uint32(stid);
  stid= pstw_store(val);
  pw__obj_add_uint32(stid);
  }  // end   pw_node_keyval()

static inline void pw_node_close() {
  // close writing node object;
  pw__objp= pw__dn_keysvals;
  pw__obj_add_uint32(0);
  }  // end   pw_node_close()

static pw__obj_t* pw__wayrel_keys= NULL,*pw__wayrel_vals= NULL,
  *pw__wayrel_his= NULL,*pw__way_noderefs= NULL,
  *pw__rel_roles= NULL,*pw__rel_refids= NULL,*pw__rel_types= NULL;

static inline void pw_way(int64_t id,
    int32_t hisver,int64_t histime,int64_t hiscset,
    uint32_t hisuid,const char* hisuser) {
  // start writing a PBF way dataset;
  // id: id of this object;
  // hisver: version; 0: no author information is to be written;
  // histime: time (seconds since 1970)
  // hiscset: changeset
  // hisuid: uid
  // hisuser: user name;
  int stid;  // string id

  pw__data(1);
  pw__obj_open("\x1a");  // S 3 'ways'
  pw__obj_add_id(0x08);  // V 1 'id'
  pw__obj_add_uint64(id);
  pw__wayrel_keys= pw__obj_open("\x12");  // S 2 'keys'
  pw__obj_limit(20 Megabytes);
  pw__wayrel_vals= pw__obj_open("\x1a");  // S 3 'vals'
  pw__obj_limit(20 Megabytes);
  pw__wayrel_his= pw__obj_open("\x22");  // S 4 'his'
  pw__obj_limit(2000);
  pw__way_noderefs= pw__obj_open("\x42");  // S 8 'noderefs'
  pw__obj_limit(30 Megabytes);
  if(!global_dropversion) {  // version number is to be written
    pw__objp= pw__wayrel_his;
    if(hisver==0) hisver= 1;
    pw__obj_add_id(0x08);  // V 1 'hisver'
    pw__obj_add_uint32(hisver);
    if(!global_dropauthor) {  // author information  is to be written
      if(histime==0) {
        histime= 1; hiscset= 1; hisuser= 0; }
      pw__obj_add_id(0x10);  // V 2 'histime'
      pw__obj_add_uint64(histime);
      pw__obj_add_id(0x18);  // V 3 'hiscset'
      pw__obj_add_uint64(hiscset);
      pw__obj_add_id(0x20);  // V 4 'hisuid'
      pw__obj_add_uint32(hisuid);
      pw__obj_add_id(0x28);  // V 5 'hisuser'
      if(hisuid==0) hisuser= "";
      stid= pstw_store(hisuser);
      pw__obj_add_uint32(stid);
      }  // author information  is to be written
    }  // version number is to be written
  pw__dc_noderef= 0;
  }  // end   pw_way()

static inline void pw_wayrel_keyval(const char* key,const char* val) {
  // write a ways or a relations object's keyval;
  int stid;  // string id

  pw__objp= pw__wayrel_keys;
  stid= pstw_store(key);
  pw__obj_add_uint32(stid);
  pw__objp= pw__wayrel_vals;
  stid= pstw_store(val);
  pw__obj_add_uint32(stid);
  }  // end   pw_wayrel_keyval()

static inline void pw_way_ref(int64_t noderef) {
  // write a ways object's noderefs;
  pw__objp= pw__way_noderefs;
  pw__obj_add_sint64(noderef-pw__dc_noderef);
  pw__dc_noderef= noderef;
  }  // end   pw_way_ref()

static inline void pw_way_close() {
  // close writing way object;
  pw__objp= pw__wayrel_keys;
  if(pw__objp->bufe==pw__objp->bufc)  // object is empty
    pw__obj_dispose();
  else
    pw__obj_close();
  pw__objp= pw__wayrel_vals;
  if(pw__objp->bufe==pw__objp->bufc)  // object is empty
    pw__obj_dispose();
  else
    pw__obj_close();
  pw__objp= pw__wayrel_his;
  if(pw__objp->bufe==pw__objp->bufc)  // object is empty
    pw__obj_dispose();
  else
    pw__obj_close();
  pw__objp= pw__way_noderefs;
  if(pw__objp->bufe==pw__objp->bufc)  // object is empty
    pw__obj_dispose();
  else
    pw__obj_close();
  pw__obj_close();
  }  // end   pw_way_close()

static inline void pw_relation(int64_t id,
    int32_t hisver,int64_t histime,int64_t hiscset,
    uint32_t hisuid,const char* hisuser) {
  // start writing a PBF way dataset;
  // id: id of this object;
  // hisver: version; 0: no author information is to be written;
  // histime: time (seconds since 1970)
  // hiscset: changeset
  // hisuid: uid
  // hisuser: user name;
  int stid;  // string id

  pw__data(2);
  pw__obj_open("\x22");  // S 4 'relations'
  pw__obj_add_id(0x08);  // V 1 'id'
  pw__obj_add_uint64(id);
  pw__wayrel_keys= pw__obj_open("\x12");  // S 2 'keys'
  pw__obj_limit(20 Megabytes);
  pw__wayrel_vals= pw__obj_open("\x1a");  // S 3 'vals'
  pw__obj_limit(20 Megabytes);
  pw__wayrel_his= pw__obj_open("\x22");  // S 4 'his'
  pw__obj_limit(2000);
  pw__rel_roles= pw__obj_open("\x42");  // S 8 'role'
  pw__obj_limit(20 Megabytes);
  pw__rel_refids= pw__obj_open("\x4a");  // S 9 'refid'
  pw__obj_limit(20 Megabytes);
  pw__rel_types= pw__obj_open("\x52");  // S 10 'type'
  pw__obj_limit(20 Megabytes);
  if(!global_dropversion) {  // version number is to be written
    pw__objp= pw__wayrel_his;
    if(hisver==0) hisver= 1;
    pw__obj_add_id(0x08);  // V 1 'hisver'
    pw__obj_add_uint32(hisver);
    if(!global_dropauthor) {  // author information  is to be written
      if(histime==0) {
        histime= 1; hiscset= 1; hisuser= 0; }
      pw__obj_add_id(0x10);  // V 2 'histime'
      pw__obj_add_uint64(histime);
      pw__obj_add_id(0x18);  // V 3 'hiscset'
      pw__obj_add_uint64(hiscset);
      pw__obj_add_id(0x20);  // V 4 'hisuid'
      pw__obj_add_uint32(hisuid);
      pw__obj_add_id(0x28);  // V 5 'hisuser'
      if(hisuid==0) hisuser= "";
      stid= pstw_store(hisuser);
      pw__obj_add_uint32(stid);
      }  // author information  is to be written
    }  // version number is to be written
  pw__dc_ref= 0;
  }  // end   pw_relation()

static inline void pw_relation_ref(int64_t refid,int reftype,
    const char* refrole) {
  // write a relations object's refs
  int stid;  // string id

  pw__objp= pw__rel_roles;
  stid= pstw_store(refrole);
  pw__obj_add_uint32(stid);
  pw__objp= pw__rel_refids;
  pw__obj_add_sint64(refid-pw__dc_ref);
  pw__dc_ref= refid;
  pw__objp= pw__rel_types;
  pw__obj_add_uint32(reftype);
  }  // end   pw_relation_ref()

static inline void pw_relation_close() {
  // close writing relation object;
  pw__objp= pw__wayrel_keys;
  if(pw__objp->bufe==pw__objp->bufc)  // object is empty
    pw__obj_dispose();
  else
    pw__obj_close();
  pw__objp= pw__wayrel_vals;
  if(pw__objp->bufe==pw__objp->bufc)  // object is empty
    pw__obj_dispose();
  else
    pw__obj_close();
  pw__objp= pw__wayrel_his;
  if(pw__objp->bufe==pw__objp->bufc)  // object is empty
    pw__obj_dispose();
  else
    pw__obj_close();
  pw__objp= pw__rel_roles;
  if(pw__objp->bufe==pw__objp->bufc)  // object is empty
    pw__obj_dispose();
  else
    pw__obj_close();
  pw__objp= pw__rel_refids;
  if(pw__objp->bufe==pw__objp->bufc)  // object is empty
    pw__obj_dispose();
  else
    pw__obj_close();
  pw__objp= pw__rel_types;
  if(pw__objp->bufe==pw__objp->bufc)  // object is empty
    pw__obj_dispose();
  else
    pw__obj_close();
  pw__obj_close();
  }  // end   pw_relation_close()

//------------------------------------------------------------
// end   Module pw_   PBF write module
//------------------------------------------------------------
