//------------------------------------------------------------
// Module Global   global variables for this program
//------------------------------------------------------------

// to distinguish global variable from local or module global
// variables, they are preceded by 'global_';

static bool global_diff= false;  // calculate diff between two files
static bool global_diffcontents= false;
  // calculate physical diff between two files; 'physical' means
  // that not only the version number is consulted to determine
  // object differences, the whole object contents is;
static bool global_mergeversions= false;  // accept duplicate versions
static bool global_dropversion= false;  // exclude version
static bool global_dropauthor= false;  // exclude author information
static bool global_fakeauthor= false;  // fake author information
static bool global_fakeversion= false;  // fake just the version number
static bool global_fakelonlat= false;
  // fake longitude and latitude in case of delete actions (.osc);
static bool global_dropbrokenrefs= false;  // exclude broken references
static bool global_dropnodes= false;  // exclude nodes section
static bool global_dropways= false;  // exclude ways section
static bool global_droprelations= false;  // exclude relations section
/*
static bool global_outo5m= false;  // output shall have .o5m format
static bool global_outo5c= false;  // output shall have .o5c format
static bool global_outosm= false;  // output shall have .osm format
static bool global_outosc= false;  // output shall have .osc format
static bool global_outosh= false;  // output shall have .osh format
static bool global_outpbf= false;  // output shall have .pbf format
static bool global_outcsv= false;  // output shall have .csv format
static bool global_outnone= false;  // no standard output at all
*/
static bool global_emulatepbf2osm= false;
  // emulate pbf2osm compatible output
static bool global_emulateosmosis= false;
  // emulate Osmosis compatible output
static bool global_emulateosmium= false;
  // emulate Osmium compatible output
static int64_t global_timestamp= 0;
  // manually chosen file timestamp; ==0: no file timestamp given;
static bool global_outtimestamp= false;
  // print only the file timestamp, nothing else
static bool global_statistics= false;  // print statistics to stderr
static bool global_outstatistics= false;  // print statistics to stdout
static bool global_csvheadline= false;  // headline for csv
static char global_csvseparator[16]= "\t";  // separator for csv
static bool global_completeways= false;  // when applying borders,
  // do not clip ways but include them as whole if at least a single
  // of its nodes lies inside the borders;
static bool global_complexways= false;  // same as global_completeways,
  // but multipolygons are included completely (with all ways and their
  // nodes), even when only a single nodes lies inside the borders;
static bool global_alltonodes= false;
  // convert all ways and all relations to nodes
static bool global_alltonodes_bbox = false;
  // add bBox tag to ways and relations when converting to nodes
static int64_t global_maxobjects= 25000000;
static int64_t global_otypeoffset10= INT64_C(1000000000000000);
  // if global_alltonodes:
  // id offset for ways; *2: id offset for relations;
static int64_t global_otypeoffset05,
  global_otypeoffset15,global_otypeoffset20;
  // (just to save CPU time for calculating the offset of relations)
static int64_t global_otypeoffsetstep= 0;
  // if !=0, the program will not create the new id by adding
  // global_otypeoffset but by starting at global_otypeoffset
  // and adding 1 for every new way, resp. relation:
static char global_tempfilename[350]= "osmconvert_tempfile";
  // prefix of names for temporary files
static int64_t global_maxrefs= 100000;
#define PERR(f) { static int msgn= 3; if(--msgn>=0) \
  fprintf(stderr,"osmconvert Error: " f "\n"); }
  // print error message
#define PERRv(f,...) { static int msgn= 3; if(--msgn>=0) \
  fprintf(stderr,"osmconvert Error: " f "\n",__VA_ARGS__); }
  // print error message with value(s)
#define WARN(f) { static int msgn= 3; if(--msgn>=0) \
  fprintf(stderr,"osmconvert Warning: " f "\n"); }
  // print a warning message, do it maximal 3 times
#define WARNv(f,...) { static int msgn= 3; if(--msgn>=0) \
  fprintf(stderr,"osmconvert Warning: " f "\n",__VA_ARGS__); }
  // print a warning message with value(s), do it maximal 3 times
#define PINFO(f) \
  fprintf(stderr,"osmconvert: " f "\n"); // print info message
#define PINFOv(f,...) \
  fprintf(stderr,"osmconvert: " f "\n",__VA_ARGS__);
#define ONAME(i) \
  (i==0? "node": i==1? "way": i==2? "relation": "unknown object")
#define global_fileM 1002  // maximum number of input files

//------------------------------------------------------------
// end   Module Global   global variables for this program
//------------------------------------------------------------
