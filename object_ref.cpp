
//------------------------------------------------------------
// Module posr_   object ref temporary module
//------------------------------------------------------------

// this module provides procedures to use a temporary file for
// storing relations' references when --all-to-nodes is used;
// as usual, all identifiers of a module have the same prefix,
// in this case 'posr'; an underline will follow for a global
// accessible identifier, two underlines if the identifier
// is not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----

static char posr__filename[400]= "";
static int posr__fd= -1;  // file descriptor for temporary file
#define posr__bufM 400000
static int64_t posr__buf[posr__bufM],
  *posr__bufp,*posr__bufe,*posr__bufee;
  // buffer - used for write, and later for read;
static bool posr__writemode;  // buffer is used for writing

static inline void posr__flush() {
  if(!posr__writemode || posr__bufp==posr__buf)
return;
  write(posr__fd,posr__buf,(char*)posr__bufp-(char*)posr__buf);
  posr__bufp= posr__buf;
  }  // end   posr__flush()

static inline void posr__write(int64_t i) {
  // write an int64 to tempfile, use a buffer;
//DPv(posr__write %lli,i)
  if(posr__bufp>=posr__bufee) posr__flush();
  *posr__bufp++= i;
  }  // end   posr__write()

static void posr__end() {
  // clean-up for temporary file access;
  // will be called automatically at program end;
  if(posr__fd>2) {
    close(posr__fd);
    posr__fd= -1;
    }
  if(loglevel<2) unlink(posr__filename);
  }  // end   posr__end()

//------------------------------------------------------------

static int posr_ini(const char* filename) {
  // open a temporary file with the given name for random r/w access;
  // return: ==0: ok; !=0: error;
  strcpy(stpmcpy(posr__filename,filename,sizeof(posr__filename)-2),".2");
  if(posr__fd>=0)  // file already open
return 0;  // ignore this call
  unlink(posr__filename);
  posr__fd= open(posr__filename,O_RDWR|O_CREAT|O_TRUNC|O_BINARY,00600);
  if(posr__fd<0) {
    PERRv("could not open temporary file: %.80s",posr__filename)
return 1;
    }
  atexit(posr__end);
  posr__bufee= posr__buf+posr__bufM;
  posr__bufp= posr__bufe= posr__buf;
  posr__writemode= true;
  return 0;
  }  // end   posr_ini()

static inline void posr_rel(int64_t relid,bool is_area) {
  // store the id of a relation in tempfile;
  // relid: id of this relation;
  // is_area: this relation describes an area;
  //          otherwise: it describes a way;
  posr__write(0);
  posr__write(relid);
  posr__write(is_area);
  } // end   posr_rel()

static inline void posr_ref(int64_t refid) {
  // store the id of a reference in tempfile;
  posr__write(refid);
  } // end   posr_ref()

static int posr_rewind() {
  // rewind the file pointer;
  // return: ==0: ok; !=0: error;
  if(posr__writemode) {
    posr__flush(); posr__writemode= false; }
  if(lseek(posr__fd,0,SEEK_SET)<0) {
    PERRv("osmconvert Error: could not rewind temporary file %.80s",
      posr__filename)
return 1;
    }
  posr__bufp= posr__bufe= posr__buf;
  return 0;
  } // end   posr_rewind()

static inline int posr_read(int64_t* ip) {
  // read one integer; meaning of the values of these integers:
  // every value is an interrelation reference id, with one exception:
  // integers which follow a 0-integer directly are relation ids;
  // return: ==0: ok; !=0: eof;
  int r,r2;

  if(posr__bufp>=posr__bufe) {
    r= read(posr__fd,posr__buf,sizeof(posr__buf));
    if(r<=0)
return 1;
    posr__bufe= (int64_t*)((char*)posr__buf+r);
    if((r%8)!=0) { // odd number of bytes
      r2= read(posr__fd,posr__bufe,8-(r%8));
        // request the missing bytes
      if(r2<=0)  // did not get the missing bytes
        posr__bufe= (int64_t*)((char*)posr__bufe-(r%8));
      else
        posr__bufe= (int64_t*)((char*)posr__bufe+r2);
      }
    posr__bufp= posr__buf;
    }
  *ip= *posr__bufp++;
  return 0;
  }  // end   posr_read()

static void posr_processing(int* maxrewindp,int32_t** refxy) {
  // process temporary relation reference file;
  // the file must already have been written; this procedure
  // processes the interrelation references of this file and updates
  // the georeference table of module posi_ accordingly;
  // maxrewind: maximum number of rewinds;
  // refxy: memory space provided by the caller;
  //        this is a temporarily used space for the coordinates
  //        of the relations' members;
  // return:
  // maxrewind: <0: maximum number of rewinds was not sufficient;
  int changed;
    // number of relations whose flag has been changed, i.e.,
    // the recursive processing will continue;
    // if none of the relations' flags has been changed,
    // this procedure will end;
  int h;  // counter for interrelational hierarchies
  int64_t relid;  // relation id;
  int64_t refid;  // interrelation reference id;
  bool jump_over;  // jump over the presently processed relation
  int32_t* xy_rel;  // geocoordinate of the processed relation;
  int32_t x_min,x_max,y_min,y_max;
  int32_t x_middle,y_middle,xy_distance,new_distance;
  int n;  // number of referenced objects with coordinates
  int64_t temp64;
  bool is_area;  // the relation describes an area
  int32_t** refxyp;  // pointer in refxy array
  int r;

  h= 0; n=0;
  jump_over= true;
  relid= 0;
  while(*maxrewindp>=0) {  // for every recursion
    changed= 0;
    if(posr_rewind())  // could not rewind
  break;
    for(;;) {  // for every reference
      for(;;) {  // get next id
        r= posr_read(&refid);
        if((r || refid==0) && n>0) {  // (EOF OR new relation) AND
            // there have been coordinates for this relation
          x_middle= x_max/2+x_min/2;
          y_middle= (y_max+y_min)/2;
          // store the coordinates for this relation
//DPv(is_area %i refxy %i,is_area,refxyp==refxy)
          if(global_alltonodes_bbox) {
            xy_rel[2]= x_min;
            xy_rel[3]= y_min;
            xy_rel[4]= x_max;
            xy_rel[5]= y_max;
            }
          if(is_area || refxyp==refxy) {
            // take the center as position for this relation
            xy_rel[0]= x_middle;
            xy_rel[1]= y_middle;
            }
          else {  // not an area
            int32_t x,y;

            // get the member position which is the nearest
            // to the center
            posi_xy= *--refxyp;
            x= posi_xy[0];
            y= posi_xy[1];
            xy_distance= abs(x-x_middle)+abs(y-y_middle);
            while(refxyp>refxy) {
              refxyp--;
              new_distance= abs(posi_xy[0]-x_middle)+
                abs(posi_xy[1]-y_middle);
              if(new_distance<xy_distance) {
                x= posi_xy[0];
                y= posi_xy[1];
                xy_distance= new_distance;
                }
              }
            xy_rel[0]= x;
            xy_rel[1]= y;
            }  // not an area
          n= 0;
          changed++;  // memorize that we calculated
            // at least one relation's position
          }
        if(r)
          goto rewind;  // if at file end, rewind
        if(refid!=0)
      break;
        // here: a relation id will follow
        posr_read(&relid);  // get the relation's id
        posr_read(&temp64);  // get the relation's area flag
        is_area= temp64;
        posi_get(relid+global_otypeoffset20);
          // get the relation's geoposition
        xy_rel= posi_xy;  // save address of relation's coordinate
        refxyp= refxy;  // restart writing coordinate buffer
        jump_over= xy_rel==NULL || xy_rel[0]!=posi_nil;
        }  // end   get next id
      if(jump_over)  // no element allocated for this very relation OR
        // position of this relation already known
    continue;  // go on until next relation
      posi_get(refid);  // get the reference's geoposition
      if(posi_xy==NULL || posi_xy[0]==posi_nil) {
          // position is unknown
        if(refid>global_otypeoffset15) {  // refers to a relation
          n= 0;  // ignore previously collected coordinates
          jump_over= true;  // no yet able to determine the position
          }
    continue;  // go on and examine next reference of this relation
        }
      *refxyp++= posi_xy;  // store coordinate for reprocessing later
      if(n==0) {  // first coordinate
        if(global_alltonodes_bbox) {
          x_min = posi_xy[2];
          y_min = posi_xy[3];
          x_max = posi_xy[4];
          y_max = posi_xy[5];
          }
        else {
          // just store it as min and max
          x_min= x_max= posi_xy[0];
          y_min= y_max= posi_xy[1];
          }
        }
      else if(global_alltonodes_bbox) {
        // adjust extrema
        if(posi_xy[2]<x_min && x_min-posi_xy[2]<900000000)
          x_min= posi_xy[2];
        else if(posi_xy[4]>x_max && posi_xy[4]-x_max<900000000)
          x_max= posi_xy[4];
        if(posi_xy[3]<y_min)
          y_min= posi_xy[3];
        else if(posi_xy[5]>y_max)
          y_max= posi_xy[5];
        }
      else {  // additional coordinate
        // adjust extrema
        if(posi_xy[0]<x_min && x_min-posi_xy[0]<900000000)
          x_min= posi_xy[0];
        else if(posi_xy[0]>x_max && posi_xy[0]-x_max<900000000)
          x_max= posi_xy[0];
        if(posi_xy[1]<y_min)
          y_min= posi_xy[1];
        else if(posi_xy[1]>y_max)
          y_max= posi_xy[1];
        }
      n++;
      }  // end   for every reference
    rewind:
    if(loglevel>0) fprintf(stderr,
      "Interrelational hierarchy %i: %i dependencies.\n",++h,changed);
    if(changed==0)  // no changes have been made in last recursion
  break;  // end the processing
    (*maxrewindp)--;
    }  // end   for every recursion
  }  // end   posr_processing()

//------------------------------------------------------------
// end   Module posr_   object ref temporary module
//------------------------------------------------------------
