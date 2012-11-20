
//------------------------------------------------------------
// Module border_   OSM border module
//------------------------------------------------------------

// this module provides procedures for reading the border file
// (.poly) and determine if a point lies inside or outside the
// border polygon;
// as usual, all identifiers of a module have the same prefix,
// in this case 'border'; an underline will follow in case of a
// global accessible object, two underlines in case of objects
// which are not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----


static int32_t border__bx1= 2000000000L,border__by1,
  border__bx2,border__by2;
  // in case of a border box:
  // coordinates of southwest and northeast corner;
// in case of a border polygon:
// for the border polygon, every edge is stored in a list;
// to speed-up the inside/outside question we need to sort the edges
// by x1; subsequently, for every edge there must be stored references
// which refer to all that edges which overlap horizontally with
// that region between x1 and the next higher x1 (x1 of the next edge
// in the sorted list);
#define border__edge_M 60004
typedef struct border__edge_t {
  int32_t x1,y1,x2,y2;  // coordinates of the edge; always: x1<x2;
  struct border__chain_t* chain;
  } border__edge_t;
  // the last element in this list will have x1==border__nil;
static border__edge_t* border__edge;
static int border__edge_n= 0;  // number of elements in border__edge[0]
#define border__chain_M (border__edge_M*8)
typedef struct border__chain_t {
  border__edge_t* edge;
  struct border__chain_t* next;
  } border__chain_t;
  // the last element in this list will have edge==NULL;
  // the last element of each chain will be terminated with next==NULL;
static border__chain_t* border__chain;

static void border__end() {
  // close this module;
  // this procedure has no parameters because we want to be able
  // to call it via atexit();

  if(border__edge!=NULL)
    free(border__edge);
  border__edge= NULL;
  border__edge_n= 0;
  if(border__chain!=NULL)
    free(border__chain);
  border__chain= NULL;
  }  // end   border__end()

static inline bool border__ini() {
  // initialize this module;
  // you may call this procedure repeatedly; only the first call
  // will have effect; subsequent calls will be ignored;
  // return: ==true: success, or the call has been ignored;
  //         ==false: an error occurred during initialization;
  static bool firstrun= true;

  if(firstrun) {
    firstrun= false;
    atexit(border__end);
    border__edge= (border__edge_t*)
      malloc((border__edge_M+4)*sizeof(border__edge_t));
    if(border__edge==NULL)
return false;
    border__chain= (border__chain_t*)
      malloc((border__chain_M+4)*sizeof(border__chain_t));
    if(border__chain==NULL)
return false;
    }
  return true;
  }  // end   border__ini()

static int border__qsort_edge(const void* a,const void* b) {
  // edge comparison for qsort()
  int32_t ax,bx;

  ax= ((border__edge_t*)a)->x1;
  bx= ((border__edge_t*)b)->x1;
  if(ax>bx)
return 1;
  if(ax==bx)
return 0;
  return -1;
  }  // end   border__qsort_edge()

//------------------------------------------------------------

static bool border_active= false;  // borders are to be considered;
  // this variable must not be written from outside of the module;

static bool border_box(const char* s) {
  // read coordinates of a border box;
  // s[]: coordinates as a string; example: "11,49,11.3,50"
  // return: success;
  double x1f,y1f;  // coordinates of southwestern corner
  double x2f,y2f;  // coordinates of northeastern corner
  int r;

  x1f= y1f= x2f= y2f= 200.1;
  r= sscanf(s,"%lG,%lG,%lG,%lG",&x1f,&y1f,&x2f,&y2f);
  if(r!=4 || x1f<-180.1 || x1f>180.1 || y1f<-90.1 || y1f>90.1 ||
      x2f<-180.1 || x2f>180.1 || y2f<-90.1 || y2f>90.1)
return false;
  border_active=true;
  border__bx1= (int32_t)(x1f*10000000L);
    // convert floatingpoint to fixpoint
  border__by1= (int32_t)(y1f*10000000L);
  border__bx2= (int32_t)(x2f*10000000L);
  border__by2= (int32_t)(y2f*10000000L);
  return true;
  }  // end   border_box()

static bool border_file(const char* fn) {
  // read border polygon file, store the coordinates, and determine
  // an enclosing border box to speed-up the calculations;
  // fn[]: file name;
  // return: success;
  static int32_t nil;

  if(!border__ini())
return false;
  nil= border__nil;

  /* get border polygon */ {
    border__edge_t* bep;  // growing pointer in border__edge[]
    border__edge_t* bee;  // memory end of border__edge[]
    FILE* fi;
    char s[80],*sp;
    int32_t x0,y0;  // coordinate of the first point in a section;
      // this is used to close an unclosed polygon;
    int32_t x1,y1;  // last coordinates
    int32_t x,y;

    border__edge[0].x1= nil;
    fi= fopen(fn,"rb");
    if(fi==NULL)
return false;
    bee= border__edge+(border__edge_M-2);
    bep= border__edge;
    x0= nil;  // (sign that there is no first coordinate at the moment)
    x1= nil;  // (sign that there is no last coordinate at the moment)
    for(;;) {  // for every line in border file
      s[0]= 0;
      sp= fgets(s,sizeof(s),fi);
      if(bep>=bee) {
        fclose(fi);
return false;
        }
      if(s[0]!=' ' && s[0]!='\t') {  // not inside a section
        if(x0!=nil && x1!=nil && (x1!=x0 || y1!=y0)) {
            // last polygon was not closed
          if(x1==x0) {  // the edge would be vertical
            // we have to insert an additional edge
            x0+= 3;
            if(x0>x1)
              { bep->x1= x1; bep->y1= y1; bep->x2= x0; bep->y2= y0; }
            else
              { bep->x1= x0; bep->y1= y0; bep->x2= x1; bep->y2= y1; }
            bep->chain= NULL;
            if(loglevel>=1)
              fprintf(stderr,
                "+ %i %i,%i,%i,%i\n",
                (int)(bep-border__edge),
                bep->x1,bep->y1,bep->x2,bep->y2);
            bep++;
            x1= x0; y1= y0;
            x0-= 3;
            }  // the edge would be vertical
          // close the polygon
          if(x0>x1)
            { bep->x1= x1; bep->y1= y1; bep->x2= x0; bep->y2= y0; }
          else
            { bep->x1= x0; bep->y1= y0; bep->x2= x1; bep->y2= y1; }
          bep->chain= NULL;
          if(loglevel>=1)
            fprintf(stderr,
              "c %i %i,%i,%i,%i\n",
              (int)(bep-border__edge),bep->x1,bep->y1,bep->x2,bep->y2);
          bep++;
          }  // end   last polygon was not closed
        x0= x1= nil;
        }  // end   not inside a section
      else {  // inside a section
        double xf,yf;

        xf= yf= 200.1;
        sscanf(s+1,"%lG %lG",&xf,&yf);
        if(xf<-180.1 || xf>180.1 || yf<-90.1 || yf>90.1) x= nil;
        else {
          x= (int32_t)(xf*10000000+0.5);
          y= (int32_t)(yf*10000000+0.5);
          }
        if(x!=nil) {  // data plausible
          if(x1!=nil) {  // there is a preceding coordinate
            if(x==x1) x+= 2;  // do not accept exact north-south
              // lines, because then we may not be able to determine
              // if a point lies inside or outside the polygon;
            if(x>x1)
              { bep->x1= x1; bep->y1= y1; bep->x2= x; bep->y2= y; }
            else
              { bep->x1= x; bep->y1= y; bep->x2= x1; bep->y2= y1; }
            bep->chain= NULL;
            if(loglevel>=1)
              fprintf(stderr,
                "- %i %i,%i,%i,%i\n",
                (int)(bep-border__edge),
                bep->x1,bep->y1,bep->x2,bep->y2);
            bep++;
            }  // end   there is a preceding coordinate
          x1= x; y1= y;
          if(x0==nil)
            { x0= x; y0= y; }
          }  // end   data plausible
        }  // end   inside a section
      if(sp==NULL)  // end of border file
    break;
      }  // end   for every line in border file
    fclose(fi);
    bep->x1= nil;  // set terminator of edge list
    border__edge_n= bep-border__edge;  // set number of edges
    }  // end   get border polygon

  // sort edges ascending by x1 value
  if(loglevel>=1)
    fprintf(stderr,"Border polygons: %i. Now sorting.\n",
      border__edge_n);
  qsort(border__edge,border__edge_n,sizeof(border__edge_t),
    border__qsort_edge);

  /* generate chains for each edge */ {
    int32_t x2;
    border__chain_t* bcp;  // growing pointer in chain storage
    border__edge_t* bep;  // pointer in border__edge[]
    border__edge_t* bep2;  // referenced edge
    border__chain_t* bcp2;  // chain of referenced edge;

    bep= border__edge;
    bcp= border__chain;
    while(bep->x1!=nil) {  // for each edge in list
      if(loglevel>=1)
        fprintf(stderr,
          "> %i %i,%i,%i,%i\n",
          (int)(bep-border__edge),bep->x1,bep->y1,bep->x2,bep->y2);
      /*x1= bep->x1;*/ x2= bep->x2;
      bep2= bep;
      while(bep2>border__edge && (bep2-1)->x1==bep2->x1) bep2--;
        // we must examine previous edges having same x1 too;
      while(bep2->x1!=nil && bep2->x1 <= x2) {
          // for each following overlapping edge in list
        if(bep2==bep) {  // own edge
          bep2++;  // (needs not to be chained to itself)
      continue;
          }
        if(bcp>=border__chain+border__chain_M)
            // no more space in chain storage
return false;
        if(loglevel>=2)
          fprintf(stderr,"+ add to chain of %i\n",
            (int)(bep2-border__edge));
        bcp2= bep2->chain;
        if(bcp2==NULL)  // no chain yet
          bep2->chain= bcp;  // add first chain link
        else {  // edge already has a chain
          // go to the chain's end and add new chain link there

          while(bcp2->next!=NULL) bcp2= bcp2->next;
          bcp2->next= bcp;
          }  // end   edge already has a chain
        bcp->edge= bep;
          // add source edge to chain of overlapping edges
        bcp->next= NULL;  // new chain termination
        bcp++;
        bep2++;
        }  // for each following overlapping  edge in list
      bep++;
      }  // end   for each edge in list
    }  // end   generate chains for each edge

  // test output
  if(loglevel>=2) {
    border__edge_t* bep,*bep2;  // pointers in border__edge[]
    border__chain_t* bcp;  // pointer in chain storage

    fprintf(stderr,"Chains:\n");
    bep= border__edge;
    while(bep->x1!=nil) {  // for each edge in list
      fprintf(stderr,
        "> %i %i,%i,%i,%i\n",
        (int)(bep-border__edge),bep->x1,bep->y1,bep->x2,bep->y2);
      bcp= bep->chain;
      while(bcp!=NULL) {  // for each chain link in edge
        bep2= bcp->edge;
        fprintf(stderr,
          "  %i %i,%i,%i,%i\n",
          (int)(bep2-border__edge),
          bep2->x1,bep2->y1,bep2->x2,bep2->y2);
        bcp= bcp->next;
        }  // end   for each chain link in edge
      bep++;
      }  // end   for each edge in list
    }  // end   test output

  /* determine enclosing border box */ {
    border__edge_t* bep;  // pointer in border__edge[]

    border__bx1= border__edge[0].x1;
    border__bx2= -2000000000L;  // (default)
    border__by1= 2000000000L; border__by2= -2000000000L;  // (default)
    bep= border__edge;
    while(bep->x1!=nil) {  // for each coordinate of the polygon
      if(bep->x2>border__bx2) border__bx2= bep->x2;
      if(bep->y1<border__by1) border__by1= bep->y1;
      if(bep->y2<border__by1) border__by1= bep->y2;
      if(bep->y1>border__by2) border__by2= bep->y1;
      if(bep->y2>border__by2) border__by2= bep->y2;
      bep++;
      }  // end   for each coordinate of the polygon
    }  // end   determine enclosing border box
  border_active=true;
  if(loglevel>=1)
    fprintf(stderr,"End of border initialization.\n");
  return true;
  }  // end   border_file()

static bool border_queryinside(int32_t x,int32_t y) {
  // determine if the given coordinate lies inside or outside the
  // border polygon(s);
  // x,y: coordinates of the given point in 0.0000001 degrees;
  // return: point lies inside the border polygon(s);
  static int32_t nil;

  nil= border__nil;

  #if MAXLOGLEVEL>=3
  if(loglevel>=3)
    fprintf(stderr,"# %li,%li\n",x,y);
  #endif
  // first, consider border box (if any)
  if(border__bx1!=nil) {  // there is a border box
    if(x<border__bx1 || x>border__bx2 ||
        y<border__by1 || y>border__by2)
        // point lies outside the border box
return false;
    }  // end   there is a border box

  /* second, consider border polygon (if any) */ {
    border__edge_t* bep;  // pointer in border__edge[]
    border__chain_t* bcp;  // pointer in border__chain[]
    int cross;  // number of the crossings a line from the point
      // to the north pole would have ageinst the border lines
      // in border__coord[][];

    if(border__edge==NULL)
return true;
    cross= 0;

    /* binary-search the edge with the closest x1 */ {
      int i,i1,i2;  // iteration indexes

      i1= 0; i2= border__edge_n;
      while(i2>i1+1) {
        i= (i1+i2)/2;
        bep= border__edge+i;
//fprintf(stderr,"s %i %i %i   %li\n",i1,i,i2,bep->x1); ///
        if(bep->x1 > x) i2= i;
        else i1= i;
//fprintf(stderr,"  %i %i %i\n",i1,i,i2); ///
        }
      bep= border__edge+i1;
      }  // end   binary-search the edge with the closest x1

    bcp= NULL;
      // (default, because we want to examine the own edge first)
    for(;;) {  // for own edge and each edge in chain
      if(bep->x1 <= x && bep->x2 > x) {  // point lies inside x-range
        if(bep->y1 > y && bep->y2 > y) {
            // line lies completely north of point
          cross++;
          #if MAXLOGLEVEL>=3
          if(loglevel>=3)
            fprintf(stderr,"= %i %li,%li,%li,%li\n",
              (int)(bep-border__edge),bep->x1,bep->y1,bep->x2,bep->y2);
          #endif
          }
        else if(bep->y1 > y || bep->y2 > y) {
            // one line end lies north of point
          if( (int64_t)(y-bep->y1)*(int64_t)(bep->x2-bep->x1) <
              (int64_t)(x-bep->x1)*(int64_t)(bep->y2-bep->y1) ) {
              // point lies south of the line
            cross++;
            #if MAXLOGLEVEL>=3
            if(loglevel>=3)
              fprintf(stderr,"/ %i %li,%li,%li,%li\n",
                (int)(bep-border__edge),
                bep->x1,bep->y1,bep->x2,bep->y2);
            #endif
            }
          #if MAXLOGLEVEL>=3
          else if(loglevel>=3)
            fprintf(stderr,". %i %li,%li,%li,%li\n",
              (int)(bep-border__edge),
              bep->x1,bep->y1,bep->x2,bep->y2);
          #endif
          }  // end   one line end north of point
        #if MAXLOGLEVEL>=3
        else if(loglevel>=3)
            fprintf(stderr,"_ %i %li,%li,%li,%li\n",
              (int)(bep-border__edge),bep->x1,bep->y1,bep->x2,bep->y2);
        #endif
        }  // end   point lies inside x-range
      if(bcp==NULL)  // chain has not been examined
        bcp= bep->chain;  // get the first chain link
      else
        bcp= bcp->next;  // get the next chain link
      if(bcp==NULL)  // no more chain links
    break;
      bep= bcp->edge;
      }  // end   for own edge and each edge in chain
//if(loglevel>=3) fprintf(stderr,"# %li,%li cross %i\n",x,y,cross);
return (cross&1)!=0;  // odd number of crossings
    }  // end   second, consider border polygon (if any)
  }  // end   border_queryinside()

static void border_querybox(int32_t* x1p,int32_t* y1p,
    int32_t* x2p,int32_t* y2p) {
  // get the values of a previously defined border box;
  // border_box() or border_file() must have been called;
  // return values are valid only if border_active==true;
  // *x1p,*y1p;  // coordinates of southwestern corner;
  // *x2p,*y2p;  // coordinates of northeastern corner;
  int32_t x1,y1,x2,y2;

  if(!border_active) {
    *x1p= *y1p= *x2p= *y2p= 0;
return;
    }
  x1= border__bx1; y1= border__by1;
  x2= border__bx2; y2= border__by2;
  // round coordinates a bit
  #define D(x) { if(x%1000==1) { if(x>0) x--; else x++; } \
    else if((x)%1000==999) { if((x)>0) x++; else x--; } }
  D(x1) D(y1) D(x2) D(y2)
  #undef D
  *x1p= x1; *y1p= y1; *x2p= x2; *y2p= y2;
  }  // end   border_querybox()

//------------------------------------------------------------
// end Module border_   OSM border module
//------------------------------------------------------------
