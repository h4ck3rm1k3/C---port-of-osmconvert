
//------------------------------------------------------------
// Module posi_   OSM position module
//------------------------------------------------------------

// this module provides a geocoordinate table for to store
// the coordinates of all OSM objects;
// the procedures posi_set() and posi_get() allow access to
// this tables;
// as usual, all identifiers of a module have the same prefix,
// in this case 'posi'; an underline will follow for a global
// accessible identifier, two underlines if the identifier
// is not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----

struct posi__mem_struct {  // element of position array
  int64_t id;
  int32_t data[];
  } __attribute__((__packed__));
  // (do not change this structure; the search algorithm expects
  // the size of this structure to be 16 or 32 bytes)
  // data[] stands for either
  //   int32_t x,y;
  // or
  //   int32_t x,y,x1,y1,x2,y2;  // (including bbox)
  // remarks to .x:
  // if you get posi_nil as value for x, you may assume that
  // the object has been stored, but its geoposition is unknown;
  // remarks to .id:
  // the caller of posi_set() and posi_get() has to care about adding
  // global_otypeoffset10 to the id if the object is a way and
  // global_otypeoffset20 to the id if the object is a relation;
typedef struct posi__mem_struct posi__mem_t;
static posi__mem_t* posi__mem= NULL;  // start address of position array
static posi__mem_t* posi__meme= NULL;  // logical end address
static posi__mem_t* posi__memee= NULL;  // physical end address

static void posi__end() {
  // clean-up for posi module;
  // will be called at program's end;
  if(posi__mem==NULL)
    PERR("not enough memory. Reduce --max-objects=")
  else {  // was initialized
    if(posi__meme>=posi__memee)  // not enough space in position array
      PERR("not enough space. Increase --max-objects=")
    else {
      int64_t siz;

      siz= (char*)posi__memee-(char*)posi__mem;
      siz= siz/4*3;
      if((char*)posi__meme-(char*)posi__mem>siz)
          // low space in position array
        WARN("low space. Try to increase --max-objects=")
      }
    free(posi__mem);
    posi__mem= NULL;
    }
  }  // end   posi__end()

//------------------------------------------------------------

static size_t posi__mem_size= 0;  // size of structure
static size_t posi__mem_increment= 0;
  // how many increments to ++ when allocating
static size_t posi__mem_mask= 0;
  // bitmask to start at base of structure

static int posi_ini() {
  // initialize the posi module;
  // return: 0: OK; 1: not enough memory;
  int64_t siz;

  global_otypeoffset05= global_otypeoffset10/2;
  global_otypeoffset15= global_otypeoffset10+global_otypeoffset05;
  global_otypeoffset20= global_otypeoffset10*2;
  if(global_otypeoffsetstep!=0)
    global_otypeoffsetstep= global_otypeoffset10;
  if(posi__mem!=NULL)  // already initialized
return 0;
  atexit(posi__end);  // chain-in the clean-up procedure
  // allocate memory for the positions array
  if (global_alltonodes_bbox) {
    posi__mem_size = 32;
    posi__mem_mask = ~0x1f;
    posi__mem_increment = 4;
    }
  else {
    posi__mem_size = 16;
    posi__mem_mask = ~0x0f;
    posi__mem_increment = 2;
  }
  siz= posi__mem_size*global_maxobjects;
  posi__mem= (posi__mem_t*)malloc(siz);
  if(posi__mem==NULL)  // not enough memory
return 1;
  posi__meme= posi__mem;
  posi__memee= (posi__mem_t*)((char*)posi__mem+siz);
  return 0;
  }  // end   posi_ini()

static inline void posi_set(int64_t id,int32_t x,int32_t y) {
  // set geoposition for a specific object ID;
  // id: id of the object;
  // x,y: geocoordinates in 10^-7 degrees;
  if(posi__meme>=posi__memee)  // not enough space in position array
    exit(70001);
  posi__meme->id= id;
  posi__meme->data[0]= x;
  posi__meme->data[1]= y;
  if (global_alltonodes_bbox) {
    posi__meme->data[2]= x; // x_min
    posi__meme->data[3]= y; // y_min
    posi__meme->data[4]= x; // x_max
    posi__meme->data[5]= y; // y_max
    }
  posi__meme+= posi__mem_increment;
  }  // end   posi_set()

static const int32_t posi_nil= 2000000000L;
static int32_t* posi_xy= NULL;  // position of latest read coordinates;
  // posi_xy[0]: x; posi_xy[1]: y;
  // posi_xy==NULL: no geoposition available for this id;

static inline void posi_get(int64_t id) {
  // get the previously stored geopositions of an object;
  // id: id of the object;
  // return: posi_xy[0]: x; posi_xy[1]: y;
  //         the caller may change the values for x and y;
  //         posi_xy==NULL: no geoposition available for this id;
  char* min,*max,*middle;
  int64_t middle_id;

  min= (char*)posi__mem;
  max= (char*)posi__meme;
  while(max>min) {  // binary search
    middle= (((max-min-posi__mem_size)/2)&(posi__mem_mask))+min;
    middle_id= *(int64_t*)middle;
    if(middle_id==id) {  // we found the right object
      posi_xy= (int32_t*)(middle+8);
return;
      }
    if(middle_id>id)
      max= middle;
    else
      min= middle+posi__mem_size;
    }  // binary search
  // here: did not find the geoposition of the object in question
  posi_xy= NULL;
  }  // end   posi_geti();

//------------------------------------------------------------
// end   Module posi_   OSM position module
//------------------------------------------------------------
