
//------------------------------------------------------------
// Module pstw_   pbf string write module
//------------------------------------------------------------

// this module provides procedures for collecting c-formatted
// strings while eliminating string doublets;
// this is needed to create Blobs for writing data in .pbf format;
// as usual, all identifiers of a module have the same prefix,
// in this case 'pstw'; an underline will follow in case of a
// global accessible object, two underlines in case of objects
// which are not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----

// string processing
// we need a string table to collect every string of a Blob;
// the data entities do not contain stings, they just refer to
// the strings in the string table; hence string doublets need
// not to be stored physically;

// how this is done
//
// there is a string memory; the pointer pstw__mem poits to the start
// of this memory area; into this area every string is written, each
// starting with 0x0a and the string length in pbf unsigned Varint
// format;
//
// there is a string table which contains pointers to the start of each
// string in the string memory area;
//
// there is a hash table which accelerates access to the string table;

#define kilobytes *1000  // unit "kilo"
#define Kibibytes *1024  // unit "Kibi"
#define Megabytes *1000000  // unit "Mega"
#define Mibibytes *1048576  // unit "Mibi"
#define pstw__memM (30 Megabytes)
  // maximum number of bytes in the string memory
#define pstw__tabM (1500000)
  // maximum number of strings in the table
#define pstw__hashtabM 25000009  // (preferably a prime number)
  // --> 150001, 1500007, 5000011, 10000019, 15000017,
  // 20000003, 25000009, 30000049, 40000003, 50000017
static char* pstw__mem= NULL;  // pointer to the string memory
static char* pstw__meme= NULL, *pstw__memee= NULL;  // pointers to
  // the logical end and to the physical end of string memory
typedef struct pstw__tab_struct {
  int index;  // index of this string table element;
  int len;  // length of the string contents
  char* mem0;  // pointer to the string's header in string memory area,
    // i.e., the byte 0x0a and the string's length in Varint format;
  char* mem;  // pointer to the string contents in string memory area
  int frequency;  // number of occurrences of this string
  int hash;
    // hash value of this element, used as a backlink to the hash table;
  struct pstw__tab_struct* next;
    // for chaining of string table rows which match
    // the same hash value; the last element will point to NULL;
  } pstw__tab_t;
static pstw__tab_t pstw__tab[pstw__tabM];  // string table
static int pstw__tabn= 0;  // number of entries in string table
static pstw__tab_t* pstw__hashtab[pstw__hashtabM];
  // hash table; elements point to matching strings in pstw__tab[];
  // []==NULL: no matching element to this certain hash value;

static inline uint32_t pstw__hash(const char* str,int* hash) {
  // get hash value of a string;
  // str[]: string from whose contents the hash is to be retrieved;
  // return: length of the string;
  // *hash: hash value in the range 0..(pstw__hashtabM-1);
  uint32_t c,h;
  const char* s;

  s= str;
  h= 0;
  for(;;) {
    if((c= *s++)==0) break; h+= c;
    if((c= *s++)==0) break; h+= c<<8;
    if((c= *s++)==0) break; h+= c<<16;
    if((c= *s++)==0) break; h+= c<<24;
    if((c= *s++)==0) break; h+= c<<4;
    if((c= *s++)==0) break; h+= c<<12;
    if((c= *s++)==0) break; h+= c<<20;
    }
  *hash= h % pstw__hashtabM;
  return (uint32_t)(s-str-1);
  }  // end   pstw__hash()

static inline pstw__tab_t* pstw__getref(
    pstw__tab_t* tabp,const char* s) {
  // get the string table reference of a string;
  // tabp: presumed index in string table (we got it from hash table);
  //       must be >=0 and <pstw__tabM, there is no boundary check;
  // s[]: string whose reference is to be determined;
  // return: pointer to string table entry;
  //         ==NULL: this string has not been stored yet
  const char* sp,*tp;
  int len;

  do {
    // compare the string with the tab entry
    tp= tabp->mem;
    len= tabp->len;
    sp= s;
    while(*sp!=0 && len>0 && *sp==*tp) { len--; sp++; tp++; }
    if(*sp==0 && len==0)  // string identical to string in table
  break;
    tabp= tabp->next;
    } while(tabp!=NULL);
  return tabp;
  }  // end   pstw__getref()

static void pstw__end() {
  // clean-up string processing;
  if(pstw__mem!=NULL) {
    free(pstw__mem);
    pstw__mem= pstw__meme= pstw__memee= NULL;
    }
  }  // end   pstw__end()

//------------------------------------------------------------

static int pstw_ini() {
  // initialize this module;
  // must be called before any other procedure is called;
  // return: 0: everything went ok;
  //         !=0: an error occurred;
  static bool firstrun= true;

  if(firstrun) {
    firstrun= false;
    pstw__mem= (char*)malloc(pstw__memM);
    if(pstw__mem==NULL)
return 1;
    atexit(pstw__end);
    pstw__memee= pstw__mem+pstw__memM;
    pstw__meme= pstw__mem;
    }
  return 0;
  }  // end   pstw_ini()

static inline void pstw_reset() {
  // clear string table and string hash table;
  // must be called before the first string is stored;
  memset(pstw__hashtab,0,sizeof(pstw__hashtab));
  pstw__meme= pstw__mem;

  // write string information of zero-string into string table
  pstw__tab->index= 0;
  pstw__tab->len= 0;
  pstw__tab->frequency= 0;
  pstw__tab->next= NULL;
  pstw__tab->hash= 0;

  // write zero-string into string information memory area
  pstw__tab->mem0= pstw__meme;
  *pstw__meme++= 0x0a;  // write string header into string memory
  *pstw__meme++= 0;  // write string length
  pstw__tab->mem= pstw__meme;

  pstw__tabn= 1;  // start with index 1
  }  // end   pstw_reset()

static inline int pstw_store(const char* s) {
  // store a string into string memory and return the string's index;
  // if an identical string has already been stored, omit writing,
  // just return the index of the stored string;
  // s[]: string to write;
  // return: index of the string in string memory;
  //         <0: string could not be written (e.g. not enough memory);
  uint32_t sl;  // length of the string
  int h;  // hash value
  pstw__tab_t* tabp;

  sl= pstw__hash(s,&h);
  tabp= pstw__hashtab[h];
  if(tabp!=NULL)  // string presumably stored already
    tabp= pstw__getref(tabp,s);  // get the right one
      // (if there are more than one with the same hash value)
  if(tabp!=NULL) {  // we found the right string in the table
    tabp->frequency++;  // mark that the string has (another) duplicate
return tabp->index;
    }
  // here: there is no matching string in the table

  // check for string table overflow
  if(pstw__tabn>=pstw__tabM) {  // no entry left in string table
    PERR("PBF write: string table overflow.")
return -1;
    }
  if(sl+10>(pstw__memee-pstw__meme)) {
      // not enough memory left in string memory area
    PERR("PBF write: string memory overflow.")
return -2;
    }

  // write string information into string table
  tabp= pstw__tab+pstw__tabn;
  tabp->index= pstw__tabn++;
  tabp->len= sl;
  tabp->frequency= 1;

  // update hash table references accordingly
  tabp->next= pstw__hashtab[h];
  pstw__hashtab[h]= tabp;  // link the new element to hash table
  tabp->hash= h;  // back-link to hash table element

  // write string into string information memory area
  tabp->mem0= pstw__meme;
  *pstw__meme++= 0x0a;  // write string header into string memory
  /* write the string length into string memory */ {
    uint32_t v,frac;

    v= sl;
    frac= v&0x7f;
    while(frac!=v) {
      *pstw__meme++= frac|0x80;
      v>>= 7;
      frac= v&0x7f;
      }
    *pstw__meme++= frac;
    }  // write the string length into string memory
  tabp->mem= pstw__meme;
  strcpy(pstw__meme,s);  // write string into string memory
  pstw__meme+= sl;
  return tabp->index;
  }  // end   pstw_store()

#if 1
static inline void pstw_write(byte** bufpp) {
  // write the string table in PBF format;
  // *bufpp: start address where to write the string table;
  // return:
  // *bufpp: address of the end of the written string table;
  size_t size;

  if(pstw__tabn==0)  // not a single string in memory
return;
  size= pstw__meme-pstw__mem;
  memcpy(*bufpp,pstw__mem,size);
  *bufpp+= size;
  }  // end   pstw_write()

#else
// remark:
// in the present program structure the bare sorting of the
// string table will lead to false output because string indexes
// in data fields are not updated accordingly;
// there would be an easy way to accomplish this for dense nodes,
// but I don't know if it's worth the effort in the first place;

static int pstw__qsort_write(const void* a,const void* b) {
  // string occurrences comparison for qsort() in pstw_write()
  int ax,bx;

  ax= ((pstw__tab_t**)a)->frequency;
  bx= ((pstw__tab_t**)b)->frequency;
  if(ax>bx)
return 1;
  if(ax==bx)
return 0;
  return -1;
  }  // end   pstw__qsort_write()

static inline int pstw_write(byte** bufpp) {
  // write the string table in PBF format;
  // *bufpp: start address where to write the string table;
  // return: number of bytes written;
  // *bufpp: address of the end of the written string table;
  // not used at present:
  // before the string table is written, it has to be ordered by
  // the number of occurrences of the strings; the most frequently
  // used strings must be written first;
  pstw__tab_t* tabp,*taborder[pstw__tabM],**taborderp;
  int i;
  byte* bufp;
  int l;

  if(pstw__tabn==0)  // not a single string in memory
return;

  // sort the string table, using an index list
  taborderp= taborder;
  tabp= pstw__tab;
  for(i= 0; i<pstw__tabn; i++)  // for every string in string table
    *taborderp++= tabp++;  // create an index list of the string table
  qsort(taborder,pstw__tabn,sizeof(taborder[0]),pstw__qsort_write);

  // write the string table, using the list of sorted indexes
  bufp= *bufpp;
  taborderp= taborder;
  for(i= 0; i<pstw__tabn; i++) {  // for every string in string table
    tabp= *taborder++;
    l= (int)(tabp->mem-tabp->mem0)+tabp->len;
    memcpy(bufp,tabp->mem0,l);
    bufp+= l;
    }  // for every string in string table
  l= bufp-*bufpp;
  *bufpp= bufp;
  return l;
  }  // end   pstw_write()
#endif

static inline int pstw_queryspace() {
  // query how much memory space is presently used by the strings;
  // this is useful before calling pstw_write();
  return (int)(pstw__meme-pstw__mem);
  }  // end   pstw_queryspace()

//------------------------------------------------------------
// end Module pstw_   pbf string write module
//------------------------------------------------------------
