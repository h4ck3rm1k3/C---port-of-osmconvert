

//------------------------------------------------------------
// Module wo_   write osm module
//------------------------------------------------------------

// this module provides procedures which write osm objects;
// it uses procedures from module o5_;
// as usual, all identifiers of a module have the same prefix,
// in this case 'wo'; an underline will follow in case of a
// global accessible object, two underlines in case of objects
// which are not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----

static int wo__format= 0;  // output format;
  // 0: o5m; 11: native XML; 12: pbf2osm; 13: Osmosis; 14: Osmium;
  // 21: csv; -1: PBF;
static bool wo__logaction= false;  // write action for change files,
  // e.g. "<create>", "<delete>", etc.
static char* wo__xmlclosetag= NULL;  // close tag for XML output;
static bool wo__xmlshorttag= false;
  // write the short tag ("/>") instead of the long tag;
#define wo__CLOSE {  /* close the newest written object; */ \
  if(wo__xmlclosetag!=NULL) { if(wo__xmlshorttag) write_str("\"/>" NL); \
    else write_str(wo__xmlclosetag); \
    wo__xmlclosetag= NULL; wo__xmlshorttag= false; } }
#define wo__CONTINUE {  /* continue an XML object */ \
  if(wo__xmlshorttag) { write_str("\">" NL); wo__xmlshorttag= false; \
      /* from now on: long close tag necessary; */ } }
static int wo__lastaction= 0;  // last action tag which has been set;
  // 0: no action tag; 1: "create"; 2: "modify"; 3: "delete";
  // this is used only in .osc files;



//------------------------------------------------------------
