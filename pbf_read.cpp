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

static inline int64_t pb__strtimetosint64(const char* s) {
  // read a timestamp in OSM format, e.g.: "2010-09-30T19:23:30Z",
  // and convert it to a signed 64-bit integer;
  // return: time as a number (seconds since 1970);
  //         ==0: syntax error;
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
return mktime(&tm)-timezone;
    #else
return timegm(&tm);
    #endif
    }  // regular timestamp
  }  // end   pb__strtimetosint64()


//------------------------------------------------------------

static void pb_ini() {
  // initialize this module;
  // must be called as first procedure of this module;
  }  // end   pb_ini()

static int pb_input(bool reset) {
  // read next pbf object and make it available via other
  // procedures of this mudule;
  // pb_ini() must have been called before calling this procedure;
  // reset: just reset al buffers, do nothing else;
  //        this is if the file has been rewound;
  // return: >=0: OK; -1: end of file; <=-10: error; 
  // pb_type: type of the object which has been read;
  // in dependence of object's type the following information
  // will be available:
  // pb_bbvalid: the following bbox coordinates are valid;
  // pb_bbx1,pb_bby1,pb_bbx2,pb_bby2: bbox coordinates (base 10^-7);
  // pb_filetimestamp: timestamp of the file; 0: no file timestamp;
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
  #define END(r) {pb_type= (r); goto end;}
    // jump to procedure's end and provide a return code
  #define ENDE(r,f) { PERR(f) END(r) }
    // print error message, then jump to end
  #define ENDEv(r,f,...) { PERRv(f,__VA_ARGS__) END(r) }
    // print error message with value(s), then jump to end
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
  static byte* nodevis= NULL,*nodevise= NULL;  // visible
  static byte* nodelat= NULL,*nodelate= NULL;  // node latitudes
  static byte* nodelon= NULL,*nodelone= NULL;  // node longitudes
  static uint32_t hisuser= 0;  // string index of user name (delta coded)
  static bool waycomplete= false,relcomplete= false;

  if(reset) {
    zbufp= zbuf,zbufe= zbuf;
    groupp= zbuf,groupe= zbuf;
    nodeid= NULL,nodeide= NULL;
    nodever= NULL,nodevere= NULL;
    nodetime= NULL,nodetimee= NULL;
    nodecset= NULL,nodecsete= NULL;
    nodeuid= NULL,nodeuide= NULL;
    nodeuser= NULL,nodeusere= NULL;
    nodevis= NULL,nodevise= NULL;
    nodelat= NULL,nodelate= NULL;
    nodelon= NULL,nodelone= NULL;
    hisuser= 0;
    waycomplete= false,relcomplete= false;
    pb_type= 99;
return 0;
    }
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
          nodeuser>=nodeusere)  // no author information available
        pb_hisver= 0;
      else {  // author information available
        pb_hisver= pbf_uint32(&nodever);
        pb_histime+= pbf_sint64(&nodetime);
        pb_hiscset+= pbf_sint64(&nodecset);
        pb_hisuid+= pbf_sint32(&nodeuid);
        hisuser+= pbf_sint32(&nodeuser);
        if(hisuser<pb__strm)  // string index ok
          pb_hisuser= pb__str[hisuser];
        else {  // string index overflow
          WARNv("node %Ld user string index overflow: %u>=%i",
            pb_id,hisuser,pb__strm)
          hisuser= 0; pb_hisuser= "";
          }
        pb_hisvis= -1;
        if(nodevis!=NULL && nodevis<nodevise) {
          pb_hisvis= pbf_uint32(&nodevis);
          }
        }  // end   author information available
      END(0)
      }  // dense nodes left
    if(waycomplete) {  // ways left
      // provide a way
      waycomplete= false;
      // (already got id and author integers)
      if(pb_hisver!=0 && hisuser>0) {
          // author information available
        if(hisuser<pb__strm)  // string index ok
          pb_hisuser= pb__str[hisuser];
        else {  // string index overflow
          WARNv("way %Ld user string index overflow: %u>=%i",
            pb_id,hisuser,pb__strm)
          hisuser= 0; pb_hisuser= "";
          }
        }  // end   author information available
      END(1)
      }  // ways left
    if(relcomplete) {  // relations left
      // provide a relation
      relcomplete= false;
      // (already got id and author integers)
      if(pb_hisver!=0 && hisuser>0) {
          // author information available
        if(hisuser<pb__strm)  // string index ok
          pb_hisuser= pb__str[hisuser];
        else {  // string index overflow
          WARNv("rel %Ld user string index overflow: %u>=%i",
            pb_id,hisuser,pb__strm)
          hisuser= 0; pb_hisuser= "";
          }
        }  // end   author information available
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
            byte* bhise;  // end of author section in buf[]

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
              case 0x2a:  // S 5, author - with subelements
                bp++;
                l= pbf_uint32(&bp);
                if(bp+l>dne)
                  ENDEv(-302,"node author section too large: %u",l)
                if(global_dropversion) {
                    // version number is not required
                  bp+= l;  // jump over this section and ignore it
                  break;
                  }
                bhise= bp+l;
                nodevis= NULL;
                while(bp<bhise) {  // for each author subelement
                  switch(bp[0]) {
                      // first byte of element in author section
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
                  case 0x32:  // S 6, visible
                    bp++;
                    l= pbf_uint32(&bp);
                    if(bp+l>bhise)
                      ENDEv(-308,"node version table too large: %u",l)
                    nodevis= bp;
                    nodevise= (bp+= l);
                    break;
                  default:
                    WARNv("node author element type unknown: "
                          "0x%02X 0x%02X.",bp[0],bp[1])
                    if(pbf_jump(&bp))
                      END(-308)
                    }  // end   first byte of element
                  }  // end   for each author subelement
                if(bp>bhise)
                  ENDE(-309,"node author format length.")
                bp= bhise;
                break;  // end   author - with subelements
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
            byte* bhise;  // end of author section in zbuf[]
            int complete;
              // flags which determine if the dataset is complete
            int hiscomplete;
              // flags which determine if the author is complete

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
              case 0x22:  // S 4, author - with subelements
                bp++;
                l= pbf_uint32(&bp);
                if(bp+l>bpe)
                  ENDEv(-404,"way author section too large: %u",l)
                if(global_dropversion) {
                    // version number is not required
                  bp+= l;  // jump over this section and ignore it
                  break;
                  }
                bhise= bp+l;
                pb_hisvis= -1;
                while(bp<bhise) {  // for each author subelement
                  switch(bp[0]) {
                      // first byte of element in author section
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
                  case 0x30:  // V 6, visible
                    bp++;
                    pb_hisvis= pbf_uint32(&bp);
                    break;
                  default:
                    WARNv("way author element type unknown: "
                      "0x%02X 0x%02X.",bp[0],bp[1])
                    if(pbf_jump(&bp))
                      END(-408)
                    }  // end   first byte of element
                  }  // end   for each author subelement
                if(bp>bhise)
                  ENDE(-411,"way author format length.")
                bp= bhise;
                complete|= 8;
                break;  // end   author - with subelements
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
            if((hiscomplete&7)!=7)  // author information not available
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
          /* get relation data */ {
            byte* bpe;  // end of ways memory area
            uint l;
            byte* bhise;  // end of author section in zbuf[]
            int complete;
              // flags which determine if the dataset is complete
            int hiscomplete;  // flags which determine
              // if the author information is complete

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
              case 0x22:  // S 4, author - with subelements
                bp++;
                l= pbf_uint32(&bp);
                if(bp+l>bpe)
                  ENDEv(-503,"rel author section too large: %u",l)
                if(global_dropversion) {
                    // version number is not required
                  bp+= l;  // jump over this section and ignore it
                  break;
                  }
                bhise= bp+l;
                pb_hisvis= -1;
                while(bp<bhise) {  // for each author subelement
                  switch(bp[0]) {
                    // first byte of element in author section
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
                  case 0x30:  // V 6, visible
                    bp++;
                    pb_hisvis= pbf_uint32(&bp);
                    break;
                  default:
                    WARNv("rel author element type unknown: "
                      "0x%02X 0x%02X.",bp[0],bp[1])
                    if(pbf_jump(&bp))
                      END(-509)
                    }  // end   first byte of element
                  }  // end   for each author subelement
                if(bp>bhise)
                  ENDE(-510,"rel author format length.")
                bp= bhise;
                complete|= 8;
                break;  // end   author - with subelements
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
            if((hiscomplete&7)!=7)  // author information not available
              pb_hisver= 0;
            else if((hiscomplete&24)!=24)  // no user information
              pb_hisuid= 0;
            #if 1
            if((complete & 1)==1) {  // minimum contents available (id)
            #else
            if((complete & 113)==113 ||
                (complete & 7)==7) {  // minimum contents available
                // have at least id and refs (1|16|32|64) OR
                // have at least id and keyval (1|2|4)
            #endif
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
            else if(memcmp(bp-1,"\x15""HistoricalInformation",21)==0)
              ;
            else  // unsupported feature
              ENDEv(-44,"unsupported feature: %.*s",l>50? 50: l,bp)
            bp+= l;
            break;
          case 0x2a:  // 0x01 S 5, optional features
            bp++;
            l= pbf_uint32(&bp);
            if(memcmp(bp-1,"\x1e""timestamp=",11)==0) {
                // file timestamp available
              pb_filetimestamp= pb__strtimetosint64((char*)bp+10);
              }  // file timestamp available
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
      byte* p;
      int r;

      // initialization
      blocktype= datasize= rawsize= zdata= -1;
      read_setjump();

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
          WARNv("block type unknown: %.40s",read_bufp)
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
  int n;

  n= 0;
  if(pb_type==0) {  // node
    int key,val;

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
        PERRv("way key string index overflow: %u,%u>=%i",
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
        PERRv("rel key string index overflow: %u,%u>=%i",
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
