//------------------------------------------------------------
// Module oo_   osm to osm module
//------------------------------------------------------------
#include "stdinc.hpp"
#include "obj_relref.hpp"
#include "osm_hash.hpp"
#include "osm_border.h"
#include "oo.h"
#include <vector>
#include "process.hpp"
#include "util.h"
#include "read.hpp"
#include "str.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <vector>



class Pbf {
public:
  uint32_t uint32( simple_vec<byte>::iterator & );
  int64_t  sint64( simple_vec<byte>::iterator & ) ;
  int32_t  sint32( simple_vec<byte>::iterator & ) ;
  uint64_t uint64( simple_vec<byte>::iterator & ) ;

  uint32_t uint32( Str::String & );
  int64_t  sint64( Str::String & ) ;
  int32_t  sint32( Str::String & ) ;
  uint64_t uint64( Str::String & ) ;

};

typedef simple_vec<Str::String> stringv_t;  // bytevector

class In {
public:
  
  class Base {
  protected:
    Pbf & pbf;
    Str & str;
  };

  class Tags : public Base {
  public:
    void read(In & in, OO & oo,bytev_t::iterator & bufp) {
      // read node key/val pairs

      stringv_t keys; 
      stringv_t vals;
      
      while(bufp) {
	Str::String key, val;
	str.read(bufp,key,val);
	keys.push_back(key);
	vals.push_back(val);
      }
    };
  } tags;

  class Node : public Base
  {
    int32_t _lon;
    int32_t _lat;
    
  public:
    
    int32_t lon();
    int32_t lat();
    int32_t lon(int32_t&);
    int32_t lat(int32_t&);

    void read(In & in, OO & oo,bytev_t::iterator & bufp)
    {
      // read node body
      lon(oo.ifp->o5lon+= pbf.sint32(bufp));
      lat(oo.ifp->o5lat+= pbf.sint32(bufp));
      
    }
  } node;

  class Way : public Base {
  public:
    void read(In & in, OO & oo,bytev_t::iterator & bufp) {
      int l= pbf.uint32(bufp);
      bytev_t::iterator bp= bufp.limit(l);
      while(bufp!=bp)	{
	int64_t id=pbf.sint64(bufp);
	id = oo.ifp->o5ridadd(id);// delta encoding
	in.refid.push_back(id);
      }
    }
  } way;


  class Rel : public Base {
  public:
    void read(In & in, OO & oo,bytev_t::iterator & bufp) {

      int64_t ri;  // temporary, refid
      int rt;  // temporary, reftype
      charv_t::iterator rr;  // temporary, refrole

      int l= pbf.uint32(bufp);
      bytev_t::iterator bp= bufp.limit(l);            
      while(bufp != bp ) {
	ri= pbf.sint64(bufp);
	str.read(bufp,rr);
	//*reftypee++= 
	rt= (*rr++ -'0')%3;  // (suppress errors)
	//*refide++= 
	oo.ifp->o5ridadd(rt,ri);
	//refrolee++;
	//runassign(refrolee,rr);
      }
    }
  } rel;


  //  char c;  // latest character which has been read
  //  byte b;  // latest byte which has been read
  //  int l;

  int otype;  // type of currently processed object;
  // 0: node; 1: way; 2: relation;

  int64_t id;
  int32_t hisver;
  int64_t histime;
  int64_t histimec;
  int64_t hiscset;
  uint32_t hisuid;
  Str::String hisuser;
  uint32_t hisverc;

  Str::String hisuserc;

  refid_t  refid;  // ids of referenced object
  coord2_t refxy;  // coordinates of referenced object
  reftype_t reftype;  // types of referenced objects
  charv2_t refrole;  // roles of referenced objects

  In () {
    //      in.hisverc= 0;
    //      in.histimec= 0;
    //      in.hisuserc="";
  }
  
};

typedef std::vector<int64_t> int64_v_t; 
int keycount();

class Write {
private:
  const char* _filename;
  const char* _filename_standard;
  const char* _filename_temp;
  char _buf[UINT64_C(16000000)];
  char* _bufe;
  char* _bufp;
  int _fd;  // (initially standard output)
  int _fd_standard;  // (initially standard output)

  void _close();
  void _end();

  bool error;  // an error has occurred
  
public:
  bool testmode();  // no standard output
  void flush();
  int open(const char* filename) ;
  int newfile(const char* filename) ;
  void _char(int c) ;
  void mem(const byte* b,int l) ;
  void str(const char* s) ;
  void xmlstr(const char* s) ;
  void xmlmnstr(const char* s) ;
  void uint32(uint32_t v) ;
  void sint32(int32_t v) ;
  void uint64(uint64_t v) ;
  void sint64(int64_t v) ;
  void createsfix7o(int32_t v,char* s) ;
  void createsbbox(int32_t x_min, int32_t y_min,    int32_t x_max, int32_t y_max, char* s);
  void sfix7(int32_t v);
  void sfix7o(int32_t v);
  void sfix6o(int32_t v);
  void sfix9(int64_t v);
  void createtimestamp(uint64_t v,char* sp) ;
  void timestamp(uint64_t v);

} writer;

class Statistics {
public:
  Process & global;
  OO & oo;
  void init();
  struct {
    int64_t nodes,ways,relations;  // number of objects
    int64_t node_id_min,node_id_max;
    int64_t way_id_min,way_id_max;
    int64_t relation_id_min,relation_id_max;
    int64_t timestamp_min,timestamp_max;
    int32_t lon_min,lon_max;
    int32_t lat_min,lat_max;
    int32_t keyval_pairs_max;
    int keyval_pairs_otype;
    int64_t keyval_pairs_oid;
    int32_t noderefs_max;
    int64_t noderefs_oid;
    int32_t relrefs_max;
    int64_t relrefs_oid;
  } statistics;

  void DoStats(In & in) {

    // care about object statistics
    if(global.statistics() &&
       oo.dependencystage()!=32) {
      // not:
      // 32:     for each relation with a member with a flag
      //           in ht, set the relation's flag in ht;
      //         for each relation,
      //           write its id and its members' ids
      //           into a temporary file (use rr_);
      //         if option --all-to-nodes is set, then
      //           for each relation, write its members'
      //             geopositions into a temporary file (use posr_);

      if(in.otype==0) {  // node
        if(statistics.nodes==0) {  // this is the first node
          statistics.lon_min= statistics.lon_max= in.node.lon();
          statistics.lat_min= statistics.lat_max= in.node.lat();
	}
        statistics.nodes++;
        if(statistics.node_id_min==0 || in.id<statistics.node_id_min)
          statistics.node_id_min= in.id;
        if(statistics.node_id_max==0 || in.id>statistics.node_id_max)
          statistics.node_id_max= in.id;
        if(in.node.lon()<statistics.lon_min)
          statistics.lon_min= in.node.lon();
        if(in.node.lon()>statistics.lon_max)
          statistics.lon_max= in.node.lon();
        if(in.node.lat()<statistics.lat_min)
          statistics.lat_min= in.node.lat();
        if(in.node.lat()>statistics.lat_max)
          statistics.lat_max= in.node.lat();
      }
      else if(in.otype==1) {  // way
        way(in);        
      }
      else if(in.otype==2) {  // relation
	relation(in);        
      }
      if(in.histime!=0) {  // timestamp valid
        if(statistics.timestamp_min==0 ||
	   in.histime<statistics.timestamp_min)
          statistics.timestamp_min= in.histime;
        if(statistics.timestamp_max==0 ||
	   in.histime>statistics.timestamp_max)
          statistics.timestamp_max= in.histime;
      }
      if(keycount()>statistics.keyval_pairs_max) {
        statistics.keyval_pairs_otype= in.otype;
        statistics.keyval_pairs_oid= in.id;
        statistics.keyval_pairs_max= keycount();
      }
    }  // object statistics

  };
  void way(In & in) ;
  void way(int64_t id,refid_t & refid) {
    statistics.ways++;
    if(statistics.way_id_min==0 || id<statistics.way_id_min)
      statistics.way_id_min= id;
    if(statistics.way_id_max==0 || id>statistics.way_id_max)
      statistics.way_id_max= id;
    if(refid.size()>statistics.noderefs_max) {
      statistics.noderefs_oid= id;
      statistics.noderefs_max= refid.size();
    }
  }

  void relation(In & in) ;
  void relation(int64_t id,int32_t refide, int32_t refid){
    statistics.relations++;
    if(statistics.relation_id_min==0 ||
       id<statistics.relation_id_min)
      statistics.relation_id_min= id;
    if(statistics.relation_id_max==0 ||
       id>statistics.relation_id_max)
      statistics.relation_id_max= id;
    if(refide-refid>statistics.relrefs_max) {
      statistics.relrefs_oid= id;
      statistics.relrefs_max= refide-refid;
    }
  }
  void print() 
  {
    if(global.statistics()) {  // print statistics
      FILE* fi;
      if(global.outstatistics()) fi= stdout;
      else fi= stderr;

      if(statistics.timestamp_min!=0) {
	char timestamp[30];
	writer.createtimestamp(statistics.timestamp_min,timestamp);
	fprintf(fi,"timestamp min: %s\n",timestamp);
      }
      if(statistics.timestamp_max!=0) {
	char timestamp[30];

	writer.createtimestamp(statistics.timestamp_max,timestamp);
	fprintf(fi,"timestamp max: %s\n",timestamp);
      }
      if(statistics.nodes>0) {  // at least one node
	char coord[20];

	writer.createsfix7o(statistics.lon_min,coord);
	fprintf(fi,"lon min: %s\n",coord);
	writer.createsfix7o(statistics.lon_max,coord);
	fprintf(fi,"lon max: %s\n",coord);
	writer.createsfix7o(statistics.lat_min,coord);
	fprintf(fi,"lat min: %s\n",coord);
	writer.createsfix7o(statistics.lat_max,coord);
	fprintf(fi,"lat max: %s\n",coord);
      }
      fprintf(fi,"nodes: %ld\n",statistics.nodes);
      fprintf(fi,"ways: %ld\n",statistics.ways);
      fprintf(fi,"relations: %ld\n",statistics.relations);
      if(statistics.node_id_min!=0)
	fprintf(fi,"node id min: %ld\n",statistics.node_id_min);
      if(statistics.node_id_max!=0)
	fprintf(fi,"node id max: %ld\n",statistics.node_id_max);
      if(statistics.way_id_min!=0)
	fprintf(fi,"way id min: %ld\n",statistics.way_id_min);
      if(statistics.way_id_max!=0)
	fprintf(fi,"way id max: %ld\n",statistics.way_id_max);
      if(statistics.relation_id_min!=0)
	fprintf(fi,"relation id min: %ld\n",
		statistics.relation_id_min);
      if(statistics.relation_id_max!=0)
	fprintf(fi,"relation id max: %ld\n",
		statistics.relation_id_max);
      if(statistics.keyval_pairs_max!=0) {
	fprintf(fi,"keyval pairs max: %i\n",
		statistics.keyval_pairs_max);
	fprintf(fi,"keyval pairs max object: %s %ld\n",
		ONAME(statistics.keyval_pairs_otype),
		statistics.keyval_pairs_oid);
      }
      if(statistics.noderefs_max!=0) {
	fprintf(fi,"noderefs max: %i\n",
		statistics.noderefs_max);
	fprintf(fi,"noderefs max object: way %ld\n",
		statistics.noderefs_oid);
      }
      if(statistics.relrefs_max!=0) {
	fprintf(fi,"relrefs max: %i\n",
		statistics.relrefs_max);
	fprintf(fi,"relrefs max object: relation %ld\n",
		statistics.relrefs_oid);
      }
    }
  }  // print statistics
  
};


class Cww{
  char * _filename;
  int _fd;  // file descriptor for temporary file
  int64_v_t cww__bu;
  int64_v_t::iterator _bufp,_bufe,_bufee;
  bool _writemode;  // buffer is used for writing
  void _flush();
  void _write(int64_t i);
  void _end();

public:
  int ini(const char* filename);
  void ref(int64_t refid);
  int rewind();
  int read(int64_t* ip);
  void processing_set();
  void processing_clear();
}; // cww;

class Posi {
  struct _mem_struct {  // element of position array
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
  typedef struct _mem_struct _mem_t;
  _mem_t* _mem;  // start address of position array
  _mem_t* _meme;  // logical end address
  _mem_t* _memee;  // physical end address
  void _end();
  size_t _mem_size;  // size of structure
  size_t _mem_increment;
  // how many increments to ++ when allocating
  size_t _mem_mask;;
public:
  int posi_ini();
  void set(int64_t id,int32_t x,int32_t y);
  int32_t nil();
  //  int32_t xy(int x);  // position of latest read coordinates;
  int32_t xy0();
  int32_t xy1();
  int32_t xy2();
  int32_t xy3();
  int32_t xy4();
  int32_t xy5();
  int32_t xy();  // position of latest read coordinates;
  void get(int64_t id);
};

class Cwn{
  char * _filename;
  int _fd;  // file descriptor for temporary file
  int64_v_t _buf;
  int64_v_t::iterator _bufp,
    _bufe,
    _bufee;
  // buffer - used for write, and later for read;
  bool _writemode;  // buffer is used for writing
  void _flush() ;
  void _write(int64_t i);
public:
  void end(); 
  int ini(const char* filename);
  void ref(int64_t refid) ;
  int rewind() ;
  int read(int64_t* ip);
  void processing();
};// cwn;

class Border {
public:
  bool active();
  bool queryinside(int32_t x,int32_t y);
};// border;

class WO {
  int _format;  // output format;
  // 0: o5m; 11: native XML; 12: pbf2osm; 13: Osmosis; 14: Osmium;
  // 21: csv; -1: PBF;
  bool _logaction;  // write action for change files,
  // e.g. "<create>", "<delete>", etc.
  char* _xmlclosetag;  // close tag for XML output;
  bool _xmlshorttag;
public:
  void flush();
  void reset();
  void end();
  void format(int wformat);
  void start(int format,bool bboxvalid, int32_t x1,int32_t y1,int32_t x2,int32_t y2,int64_t timestamp);  
  void node(In & in);//);
  void node_keyval(const char* key,const char* val);
  void node_close();
  void relation(In &in); //int64_t id,int32_t hisver,int64_t histime,int64_t hiscset,uint32_t hisuid,const char* hisuser);
  void ref(int64_t refid,int reftype,   const char* refrole);
  void relation_close();
  void way(In &in); //int64_t id,
  // int32_t hisver,int64_t histime,int64_t hiscset,
  // uint32_t hisuid,const char* hisuser);
  void noderef(int64_t noderef);
  void wayrel_keyval(const char* key,const char* val);
  void way_close();
  void dodelete(In & in);
  //void dodelete(int&, int64_t&, uint32_t&, int64_t&, int64_t&, uint32_t&, const char*&);
}; // wo;

class Posr {
public:
  void processing(int * maxrewind_posr,coord2_t & refxy);
  int ini(const char* filename);
  void rel(int64_t relid,bool is_area);  
  void ref(int64_t refid);
};

class OO2 {
  OO & oo;
  Process & global;
  Read & read;
  Posr & posr;
  WO & wo;
  Border & border;
  Cwn & cwn;
  Cww & cww;
  Pbf & pbf;
  Statistics & statistics;  
  Posi & posi;

  int wformat;  // output format;
  // 0: o5m; 11,12,13,14: some different XML formats;
  // 21: csv; -1: PBF;
  bool hashactive;
  // must be set to true if border_active OR global_dropbrokenrefs;
  static char o5mtempfile[400];  // must be static because
  // this file will be deleted by an at-exit procedure;
#define oo__maxrewindINI 12
  int maxrewind;  // maximum number of relation-relation dependencies
  int maxrewind_posr;  // same as before, but for --all-to-nodes
  bool writeheader;  // header must be written

  //  uint32_t hisver;
  //  int64_t histime;
  //  int64_t hiscset;
  //  uint32_t hisuid;
  //  const char* hisuser;


  bool diffcompare;  // the next object shall be compared
  // with the object which has just been read;
  //  bool diffdifference;
  // there was a difference in object comparison

  int read_all_input_files ();

  int main() {
    // start reading osm objects;
    // return: ==0: ok; !=0: error;
    // this procedure must only be called once;
    // before calling this procedure you must open an input file
    // using oo_open();

    // procedure initialization
    atexit(oo.end);
    //  memset(&statistics,0,sizeof(statistics));
    statistics.init();
    oo.bbvalid= false;
    hashactive= border.active() || global.dropbrokenrefs();
    oo.dependencystage(0);  // 0: no recursive processing at all;
    maxrewind= maxrewind_posr= oo.maxrewindINI;
    writeheader= true;
    if(global.outo5m()) wformat= 0;
    else if(global.outpbf()) wformat= -1;
    else if(global.emulatepbf2osm()) wformat= 12;
    else if(global.emulateosmosis()) wformat= 13;
    else if(global.emulateosmium()) wformat= 14;
    else if(global.outcsv()) wformat= 21;
    else wformat= 11;
    //  refid= (int64_t*)oo.malloc(sizeof(int64_t)*global.maxrefs);
    //refidee= refid.end();
    //refxy= (int32_t**)oo.malloc(sizeof(int32_t*)*global.maxrefs);
    //  reftype= (byte*)oo.malloc(global.maxrefs);
    // refrole= (char**)oo.malloc(sizeof(char*)*global.maxrefs);
    //  keyee= key+oo.keyvalM;
    diffcompare= false;
    //xsdiffdifference= false;

    // get input file format and care about tempfile name
    if(oo.getformat())
      return 5;
    if((hashactive && !global.droprelations()) ||
       global.alltonodes()) {
      // (borders to apply AND relations are required) OR
      // user wants ways and relations to be converted to nodes
      // initiate recursive processing;
      if(global.complexways()) {
	oo.dependencystage(11);
	// 11:     no output;
	//         for each node which is inside the borders,
	//           set flag in ht;
	//         store start of ways in read_setjump();
	//         for each way which has a member with flag in ht,
	//           set the way's flag in ht;
	//         for each relation with a member with flag in ht,
	//           store the relation's flag and write the ids
	//           of member ways which have no flag in ht
	//           (use cww_);
	if(cwn.ini(global.tempfilename()))
	  return 28;
	if(cww.ini(global.tempfilename()))
	  return 28;
      }
      else if(global.completeways()) {
	oo.dependencystage(21);
	// 21:     no output;
	//         for each node inside the borders,
	//           set flag in ht;
	//         for each way with a member with a flag in ht,
	//           set the way's flag in ht and write the ids
	//           of all the way's members (use cwn_);
	if(cwn.ini(global.tempfilename()))
	  return 28;
      }
      else
	oo.dependencystage(31);
      // 31:     for each node inside the borders,
      //           set flag in ht;
      //         for each way with a member with a flag in ht,
      //           set the way's flag in ht;
      strcpy(stpncpy(o5mtempfile,global.tempfilename(),
		     sizeof(o5mtempfile)-2),".1");
    }
    else {
      oo.dependencystage(0);  // no recursive processing
      global.completeways(false);
      global.complexways(false);
    }

    // print file timestamp and nothing else if requested
    if(global.outtimestamp()) {
      if(oo.timestamp!=0)  // file timestamp is valid
	writer.timestamp(oo.timestamp);
      else
	writer.str("(invalid timestamp)");
      writer.str("\n");
      return 0;  // nothing else to do here
    }

    // process the file
    for(;;) {  // read all input files
      read_all_input_files () ;
    }  // end   read all input files
    if(!global.outnone()) {
      if(writeheader)
	wo.start(wformat,oo.bbvalid,
		 oo.bbx1,oo.bby1,oo.bbx2,oo.bby2,oo.timestamp);
      wo.end();
    }
    return oo.error;
  }  // end   oo_main()



  void docontinue() {};
  void dobreak() {};

  struct Box {
    int32_t x_min;
    int32_t y_min;
    int32_t x_max;
    int32_t y_max;
  };

  Str str;
  In in;
  Box box;

  void process_all_referenced_nodes(In & in) 
  {

    int32_t * refxyp;
    int32_t refxy;
    int n =0;
    refid_t::iterator refidp=in.refid.begin();
    while(refidp<in.refid.end()) {  // for every referenced node
      refxyp= NULL;  // (default)
      if(!global.dropbrokenrefs() || hash_geti(0,*refidp)) {
	// referenced node lies inside the borders
	posi.get(*refidp);  // get referenced node's coordinates
	refxy= posi.xy();
	if(refxyp!=NULL) {  // coordinate is valid
	  if(n==0) {  // first coordinate
	    // just store it as min and max
	    box.x_min= box.x_max= posi.xy0();
	    box.y_min= box.y_max= posi.xy1();
	  }
	  else {  // additional coordinate
	    // adjust extrema
	    if(posi.xy0()<box.x_min && box.x_min-posi.xy0()<900000000)
	      box.x_min= posi.xy0();
	    else if(posi.xy0()>
		    box.x_max && posi.xy0()-box.x_max<900000000)
	      box.x_max= posi.xy0();
	    if(posi.xy1()<box.y_min)
	      box.y_min= posi.xy1();
	    else if(posi.xy1()>box.y_max)
	      box.y_max= posi.xy1();
	  }
	  n++;
	}  // coordinate is valid
      }  // referenced node lies inside the borders
      refidp++; refxyp++;
    }  // end   for every referenced node
  }

  
  void read_o5m (In & in,bytev_t::iterator & bufp) {
    byte  b=*bufp;
    if(b<0x10 || b>0x12) {  // not a regular dataset id
      if(b>=0xf0) {  // single byte dataset
	if(b==0xff) {  // file start, resp. o5m reset
	  if(read.setjump())
	    oo.ifp->deleteobjectjump(oo.ifp->deleteobject());
	  oo.reset_ifp();
	}
	else if(b==0xfe)
	  oo.ifp->endoffile= true;
	else if(writer.testmode())
	  WARNv("unknown .o5m short dataset id: 0x%02x",b);
	read.bufpinc();
	docontinue();
      }  // end   single byte dataset
      else {  // unknown multibyte dataset
	if(b!=0xdb && b!=0xdc && b!=0xe0)
	  // not border box AND not header
	  WARNv("unknown .o5m dataset id: 0x%02x",b);
	read.bufpinc();
	//	  byte * pstr=;
	long l= pbf.uint32(read.bufpbv());  // length of this dataset
	read.bufpinc(l);  // jump over this dataset
	docontinue();
      }  // end   unknown multibyte dataset
    }  // end   not a regular dataset id
    in.otype= b&3;
  }


  void read_author (bytev_t::iterator & bufp){
    // read author
    in.hisver= pbf.uint32(bufp);
    if(in.hisver!=0) {  // author information available
      in.histime= oo.ifp->o5histime+= pbf.sint64(bufp);
      if(in.histime!=0) {
	in.hiscset= oo.ifp->o5hiscset+= pbf.sint32(bufp);
	Str::String sp;
	str.read(bufp,sp,in.hisuser);
	in.hisuid= pbf.uint64(sp);
      }
    }  // end   author information available
  }
  void rundiff(); // run a comparison... 
  void read_o5m2( bytev_t::iterator & bufp) {
    bufp++;
    uint32_t l= pbf.uint32(bufp);
    read.bufp(bufp,l);
    //    bufe= bufp+l;
    if(diffcompare) {  // just compare, do not store the object
      rundiff();

    }  // just compare, do not store the object
    else {  // regularly read the object
      // read object id
      int offset=pbf.sint64(bufp);
      in.id= oo.ifp->o5id+= offset;

      // read author
      read_author(bufp);

      if(!bufp) {
	// just the id and author, i.e. this is a delete request
	oo.ifp->deleteobject(1);
      } else {  // not a delete request
	oo.ifp->deleteobject(0);
	// read coordinates (for nodes only)
	if(in.otype==0) {  // node
	  in.node.read(in,oo,bufp);
	}  // end   node
	// read noderefs (for ways only)
	if(in.otype==1) {  // way
	  in.way.read(in,oo,bufp);
	}  // end   way
	// read refs (for relations only)
	else if(in.otype==2) {  // relation
	  in.rel.read(in,oo,bufp);
	}  // end   relation
	in.tags.read(in,oo,bufp);

      }  // end   not a delete request
    }  // regularly read the object

  };

  void read_xml();

  void      merge_multiple_files() {
    if((
	(oo.ifp>oo.inf.begin())
	|| 
	oo.tyidold()>0) && oo.dependencystage()!=33)
      // input file switch necessary;
      // not:
      // 33:     write each relation which has a flag in ht
      //           to output;
      oo._switch();
    else if(global.mergeversions())
      oo.gettyid();
    else
      oo.ifp->tyid= 1;
    if(diffcompare && 
       oo.ifp>oo.inf.begin()) {
      // comparison must be made with the first file but presently
      // the second file is active
      // switch to the first file
      oo.ifp= oo.inf.begin();
      read.do_switch(oo.ifp->ri);
      str.doswitch(oo.ifp->str);
    }
  };



  void oo_dependencystage_ne_32() {
    if(oo.dependencystage()!=32) {
      // not:
      // 32:     for each relation with a member with a flag
      //           in ht, set the relation's flag in ht;
      //         for each relation,
      //           write its id and its members' ids
      //           into a temporary file (use rr_);
      //         if option --all-to-nodes is set, then
      //           for each relation, write its members'
      //             geopositions into a temporary file
      //             (use posr_);
      if(oo.dependencystage()==33) {
	// 33:     write each relation which has a flag in ht
	//           to output; use temporary .o5m file as input;
	if(oo.ifp!=oo.inf.end())
	  oo.ifp->endoffile= true;
	// this is because the file we have read
	// has been created as temporary file by the program
	// and does not contain an eof object;
	if(maxrewind_posr<maxrewind) maxrewind= maxrewind_posr;
	if(loglevel>0) fprintf(stderr,
			       "Relation hierarchies: %i of maximal %i.\n",
			       oo.maxrewindINI-maxrewind,oo.maxrewindINI);
	if(maxrewind<0)
	  fprintf(stderr,
		  "osmconvert Warning: relation dependencies too complex\n"
		  "         (more than %i hierarchy levels).\n"
		  "         A few relations might have been excluded\n"
		  "         although lying within the borders.\n",
		  oo.maxrewindINI);
      }
      dobreak();
    }  // end   dependencystage!=32
  }

  void dependencystage_12() {
    // if(oo.dependencystage()==12) {
    //   // 12:     no output;
    //   //         for each way with a member with a flag in ht,
    //   //           set the way's flag in ht and write the ids
    //   //           of all the way's members (use cwn_);
    //   if(!oo.ifp->deleteobject()) {  // object is not to delete
    //     if(in.otype==1 && hash_geti(1,in.id)) {
    // 	  // way AND is marked in ht
    //       // store ids of all referenced nodes of this way
    //       refidp= refid;
    //       while(refidp<refide) {  // for every referenced node
    //         cwn.ref(*refidp);
    //         refidp++;
    // 	  }  // end   for every referenced node
    // 	}  // way
    //   }  // object is not to delete
    //   docontinue();  // do not write this object
    // }  // dependencystage 12
  };

  void dependencystage_21() {
    //if(oo.dependencystage()==21) {
    // 21:     no output;
    //         for each node inside the borders,
    //           set flag in ht;
    //         for each way with a member with a flag in ht,
    //           set the way's flag in ht and write the ids
    //           of all the way's members (use cwn_);
    //   if(oo.ifp->deleteobject==0) {  // object is not to delete
    //     if(in.otype==0) {  // node
    //       if(!border.active() || border.queryinside(lon,lat))
    // 	    // no border to be applied OR node lies inside
    //         hash_seti(0,id);  // mark this node id as 'inside'
    // 	}  // node
    //     else if(in.otype==1) {  // way
    //       refidp= refid;
    //       while(refidp<refide) {  // for every referenced node
    //         if(hash_geti(0,*refidp))
    // 	      dobreak();
    //         refidp++;
    // 	  }  // end   for every referenced node
    //       if(refidp<refide) {  // at least on node lies inside
    //         hash_seti(1,id);  // memorize that this way lies inside
    //         // store ids of all referenced nodes of this way
    //         refidp= refid;
    //         while(refidp<refide) {  // for every referenced node
    //           cwn.ref(*refidp);
    //           refidp++;
    // 	    }  // end   for every referenced node
    // 	  }  // at least on node lies inside
    // 	}  // way
    //     else {  // relation
    //       oo.ifp->endoffile= true;  // suppress warnings
    //       oo.close();  // the next stage will be entered as soon as
    // 	  // all files have been closed;
    // 	  // 21->22: as soon as first relation shall be written:
    // 	  //         rewind all files;
    // 	  //         set flags for nodes, use cwn_processing();
    // 	}  // relation
    //   }  // object is not to delete
    //   docontinue();  // do not write this object
    // }  // dependencystage 21
  };


  void dependencystage_31() {
    // if(!global.droprelations() &&
    // 	 (oo.dependencystage()==31 || oo.dependencystage()==22)) {
    // 	// not relations to drop AND
    // 	// 22:     write each node which has a flag in ht to output;
    // 	//         write each way which has a flag in ht to output;
    // 	// 31:     for each node inside the borders,
    // 	//           set flag in ht;
    // 	//         for each way with a member with a flag in ht,
    // 	//           set the way's flag in ht;
    //     // 22->32: as soon as first relation shall be written:
    //     //         clear flags for ways, use cww_processing_clear();
    //     //         switch output to temporary file;
    //     // 31->32: as soon as first relation shall be written:
    //     //         switch output to temporary .o5m file;
    //     wo.flush();
    //     if(writer.newfile(o5mtempfile))
    // 	  return 24;
    //     wo.format(0);
    //     if(hashactive)
    //       if(rr.ini(global.tempfilename()))
    // 	    return 25;
    //     if(dependencystage()==22)
    //       cww_processing_clear();
    //     if(global.alltonodes())
    //       if(posr.ini(global.tempfilename()))
    // 	    return 26;
    //     oo.dependencystage(32);
    // 	// 32:     for each relation with a member with a flag
    // 	//           in ht, set the relation's flag in ht;
    // 	//         for each relation,
    // 	//           write its id and its members' ids
    // 	//           into a temporary file (use rr_);
    // 	//         if option --all-to-nodes is set, then
    // 	//           for each relation, write its members'
    // 	//             geopositions into a temporary file (use posr_);
    //   }  // dependencystage was 31
  }

  void dependancy_stages () {
    if(oo.dependencystage()==11) {
      // 11:     no output;
      //         for each node which is inside the borders,
      //           set flag in ht;
      //         store start of ways in read_setjump();
      //         for each way which has a member with flag in ht,
      //           set the way's flag in ht;
      //         for each relation with a member with flag in ht,
      //           store the relation's flag and write the ids
      //           of member ways which have no flag in ht
      //           (use cww_);
      if(in.otype>=1)  // way or relation
        read.lockjump();
      if(oo.ifp->deleteobject()==0) {  // object is not to delete
        if(in.otype==0) {  // node
          if(!border.active() || border.queryinside(in.node.lon(),in.node.lat()))
	    // no border to be applied OR node lies inside
            hash_seti(0,in.id);  // mark this node id as 'inside'
	}  // node
        else if(in.otype==1) {  // way
          refidp=refid.begin();
          while(refidp<refide) {  // for every referenced node
            if(hash_geti(0,*refidp))
	      dobreak();
            refidp++;
	  }  // end   for every referenced node
          if(refidp<refide)  // at least on node lies inside
            hash_seti(1,in.id);  // memorize that this way lies inside
	}  // way
        else {  // relation
          int64_t ri;  // temporary, refid
          int rt;  // temporary, reftype
          char* rr;  // temporary, refrole
          bool relinside;  // this relation lies inside
          bool wayinside;  // at least one way lies inside
          bool ismp;  // this relation is a multipolygon

          relinside= wayinside= ismp= false;
          refidp= refid.begin(); 
	  reftypep= reftype.begin(); 
	  refrolep= refrole;
          while(refidp<refide) {  // for every referenced object
            ri= *refidp; rt= *reftypep; rr= *refrolep;
            if(!relinside && hash_geti(rt,ri))
              relinside= true;
            if(!wayinside && rt==1 && (strcmp(rr,"outer")==0 ||
				       strcmp(rr,"inner")==0) && hash_geti(1,ri))
	      // referenced object is a way and part of
	      // a multipolygon AND lies inside
              wayinside= true;
            refidp++; reftypep++; refrolep++;
	  }  // end   for every referenced object
          if(relinside) {  // relation lies inside
            hash_seti(2,in.id);
            if(wayinside) {  // at least one way lies inside
              keyp= key; valp= val;
              while(keyp<keye) {  // for all key/val pairs of this object
                if(strcmp(*keyp,"type")==0 &&
		   strcmp(*valp,"multipolygon")==0) {
                  ismp= true;
		  dobreak();
		}
                keyp++; valp++;
	      }  // for all key/val pairs of this object
              if(ismp) {  // is multipolygon
                refidp= refid; reftypep= reftype; refrolep= refrole;
                while(refidp<refide) {  // for every referenced object
                  ri= *refidp; rt= *reftypep; rr= *refrolep;
                  if(rt==1 && (strcmp(rr,"outer")==0 ||
			       strcmp(rr,"inner")==0) &&
		     !hash_geti(1,ri)) {  // referenced object
		    // is a way and part of the multipolygon AND
		    // has not yet a flag in ht
                    cww.ref(ri);  // write id of the way
		  }
                  refidp++; reftypep++; refrolep++;
		}  // end   for every referenced object
	      }  // is multipolygon
	    }  // at least one way lies inside
	  }  // relation lies inside
	}  // relation
      }  // object is not to delete
      docontinue();  // do not write this object
    }  // dependencystage 11
    else if(oo.dependencystage()==12) {
      dependencystage_12();
    }  // dependencystage 12
    else if(oo.dependencystage()==21) {
      dependencystage_21();
    }  // dependencystage 21
    else if(in.otype==2) {  // relation
      dependencystage_31();
    }  // relation
    else {  // node or way
    }  // node or way
    // end   care about dependency stages

  }


  void write_objects()
  {
    // write the object
    if(in.otype==0) {  // write node
      bool inside;  // node lies inside borders, if appl.

      if(!border.active())  // no borders shall be applied
        inside= true;
      else if(oo.dependencystage()==22)
	// 22:     write each node which has a flag in ht to output;
	//         write each way which has a flag in ht to output;
        inside= hash_geti(0,in.id);
      else {
        inside= border.queryinside(in.node.lon(),in.node.lat());  // node lies inside
        if(inside)
          hash_seti(0,in.id);  // mark this node id as 'inside'
      }
      if(inside) {  // node lies inside
        if(global.alltonodes()) {
          // check id range
          if(in.id>=global.otypeoffset05() || in.id<=-global.otypeoffset05())
            WARNv("node id %Ld"
		  " out of range. Increase --object-type-offset",in.id);
	  posi.set(in.id,in.node.lon(),in.node.lat());  // store position
	}
        if(!global.dropnodes()) {  // not to drop
          wo.node(in);
          keyp= key; valp= val;
          while(keyp<keye)  // for all key/val pairs of this object
            wo.node_keyval(*keyp++,*valp++);
          wo.node_close();
	}  // end   not to drop
      }  // end   node lies inside
    }  // write node
    else if(in.otype==1) {  // write way
      bool inside;  // way lies inside borders, if appl.

      if(!hashactive)  // no borders shall be applied
        inside= true;
      else if(oo.dependencystage()==22)
	// 22:     write each node which has a flag in ht to output;
	//         write each way which has a flag in ht to output;
        inside= hash_geti(1,in.id);
      else {  // borders are to be applied
        inside= false;  // (default)
        refidp= refid;
        while(refidp<refide) {  // for every referenced node
          if(hash_geti(0,*refidp)) {
            inside= true;
	    dobreak();
	  }
          refidp++;
	}  // end   for every referenced node
      }  // end   borders are to be applied
      if(inside) {  // no borders OR at least one node inside
        if(hashactive)
          hash_seti(1,in.id);  // memorize that this way lies inside
        if(!global.dropways()) {  // not ways to drop
          if(global.alltonodes()) {
	    // all ways are to be converted to nodes
            int32_t x_min,x_max,y_min,y_max;
            int32_t x_middle,y_middle,xy_distance,new_distance;
            bool is_area;
            int n;  // number of referenced nodes with coordinates

            // check id range
            if(in.id>=global.otypeoffset05() || in.id<=-global.otypeoffset05())
              WARNv("way id %Ld"
		    " out of range. Increase --object-type-offset",in.id);

	    // determine the center of the way's bbox
	    n= 0;
            refidp= refid; refxyp= refxy;

	    process_all_referenced_nodes() ;
            // determine if the way is an area
            is_area= 
	      refide!=refid 
	      && 
	      refide[-1]==refid[0];
	    // first node is the same as the last one

            // determine the valid center of the way
            x_middle= x_max/2+x_min/2;
            y_middle= (y_max+y_min)/2;
            if(is_area) {
              lon= x_middle;
              lat= y_middle;
	    }
            else {  // the way is not an area
	      // determine the node with has the smallest distance
	      // to the center of the bbox ,,,
              n= 0;
              refidp= refid; refxyp= refxy;
              while(refidp<refide) {  // for every referenced node
                posi.xy= *refxyp;
                if(posi.xy!=NULL) {
		  // there is a coordinate for this reference
                  if(n==0) {  // first coordinate
                    // just store it as min and max
                    lon= posi.xy0();
                    lat= posi.xy1();
                    xy_distance= abs(lon-x_middle)+abs(lat-y_middle);
		  }
                  else {  // additional coordinate
                    new_distance= abs(posi.xy0()-x_middle)+
                      abs(posi.xy1()-y_middle);
                    if(new_distance<xy_distance) {
                      lon= posi.xy0();
                      lat= posi.xy1();
                      xy_distance= new_distance;
		    }
		  }  // additional coordinate
                  n++;
		}  // there is a coordinate for this reference
                refidp++; refxyp++;
	      }  // end   for every referenced node
	    }  // the way is not an area

            // write a node as a replacement for the way
            if(n>0) {  // there is at least one coordinate available
              int64_t id_new;

              if(global.otypeoffsetstep()!=0)
                id_new= global.otypeoffsetstep()++;
              else
                id_new= id+global.otypeoffset10();
              wo.node(id_new,
		      hisver,histime,hiscset,in.hisuid,hisuser,lon,lat);
              if (global.alltonodes().bbox()) {
                char bboxbuf[84];
                writer.createsbbox(x_min, y_min, x_max, y_max, bboxbuf);
                wo.node_keyval("bBox", bboxbuf);
	      }
              keyp= key; valp= val;
              while(keyp<keye)  // for all key/val pairs of this object
                wo.node_keyval(*keyp++,*valp++);
              wo.node_close();
              posi.set(id+global.otypeoffset10(),lon,lat);
	    }  // there is at least one coordinate available
	  }  // ways are to be converted to nodes
          else  {  // not --all-to-nodes
            wo.way(id,hisver,histime,hiscset,in.hisuid,hisuser);
            refidp= refid;
            while(refidp<refide) {  // for every referenced node
              if(!global.dropbrokenrefs() || hash_geti(0,*refidp))
		// referenced node lies inside the borders
                wo.noderef(*refidp);
              refidp++;
	    }  // end   for every referenced node
            keyp= key; valp= val;
            while(keyp<keye)  // for all key/val pairs of this object
              wo.wayrel_keyval(*keyp++,*valp++);
            wo.way_close();
	  }  // not --all-to-nodes
	}  // end   not ways to drop
      }  // end   no border OR at least one node inside
    }  // write way
    else if(in.otype==2) {  // write relation
      if(!global.droprelations()) {  // not relations to drop
        bool inside;  // relation may lie inside borders, if appl.
        bool in;  // relation lies inside borders
        int64_t ri;  // temporary, refid
        int rt;  // temporary, reftype
        char* rr;  // temporary, refrole

        in= hash_geti(2,id);
        if(oo.dependencystage()==32) {
          // 32:     for each relation with a member with a flag
          //           in ht, set the relation's flag in ht;
          //         for each relation,
          //           write its id and its members' ids
          //           into a temporary file (use rr_);
          //         if option --all-to-nodes is set, then
          //           for each relation, write its members'
          //             geopositions into a temporary file (use posr_);
          bool has_highway,has_area;  // relation has certain tags
          bool is_area;  // the relation is assumed to represent an area
          bool idwritten,posridwritten;

          // determine if this relation is assumed to represent
          // an area or not
          has_highway= has_area= false;
          keyp= key; valp= val;
          while(keyp<keye) {  // for all key/val pairs of this object
            if(strcmp(*keyp,"highway")==0 ||
	       strcmp(*keyp,"waterway")==0 ||
	       strcmp(*keyp,"railway")==0 ||
	       strcmp(*keyp,"aerialway")==0 ||
	       strcmp(*keyp,"power")==0 ||
	       strcmp(*keyp,"route")==0
	       )
              has_highway= true;
            else if(strcmp(*keyp,"area")==0 &&
		    strcmp(*valp,"yes")==0)
              has_area= true;
            keyp++,valp++;
	  }
          is_area= !has_highway || has_area;

          // write the id of this relation and its members
          // to a temporary file
          idwritten= posridwritten= false;
          refidp= refid; reftypep= reftype;
          while(refidp<refide) {  // for every referenced object
            ri= *refidp;
            rt= *reftypep;
            if(hashactive) {
              if(rt==2) {  // referenced object is a relation
                if(!idwritten) {  // did not yet write our relation's id
                  rr.rel(id);  // write it now
                  idwritten= true;
		}
                rr.ref(ri);
	      }
	    }
            if(global.alltonodes()) {
              if(!posridwritten) {
		// did not yet write our relation's id
                // write it now
                posr.rel(id,is_area);
                posi.set(id+global.otypeoffset20(),posi.nil,0);
		// reserve space for this relation's coordinates
                posridwritten= true;
	      }
              if(rt==1)  // way
                ri+= global.otypeoffset10();
              else if(rt==2)  // relation
                ri+= global.otypeoffset20();
              posr.ref(ri);
	    }
            refidp++; reftypep++;
	  }  // end   for every referenced object
          inside= true;
	}
        else if(oo.dependencystage()==33) {
	  // 33:     write each relation which has a flag in ht
	  //           to output; use temporary .o5m file as input;
          inside= in;
	}
        else
          inside= true;
        if(inside) {  // no borders OR at least one node inside
          if(global.alltonodes() && oo.dependencystage()==33) {
	    // all relations are to be converted to nodes AND
	    // 33:     write each relation which has a flag in ht
	    //           to output; use temporary .o5m file as input;
            if(id>=global.otypeoffset05() || id<=-global.otypeoffset05())
              WARNv("relation id %Ld"
		    " out of range. Increase --object-type-offset",id)
		posi.get(id+global.otypeoffset20());  // get coorinates
            if(posi.xy!=NULL && posi.xy0()!=posi.nil) {
	      // stored coordinates are valid
              int64_t id_new;

              if(global.otypeoffsetstep()!=0)
                id_new= global.otypeoffsetstep()++;
              else
                id_new= id+global.otypeoffset20();
              // write a node as a replacement for the relation
              wo.node(id_new,
		      hisver,histime,hiscset,in.hisuid,hisuser,
		      posi.xy0(),posi.xy1());
              if (global.alltonodes().bbox()) {
                char bboxbuf[84];
                writer.createsbbox(posi.xy2(), posi.xy3(),
				   posi.xy4(), posi.xy5(), bboxbuf);
                wo.node_keyval("bBox", bboxbuf);
	      }
              keyp= key; valp= val;
              while(keyp<keye)  // for all key/val pairs of this object
                wo.node_keyval(*keyp++,*valp++);
              wo.node_close();
	    }  // stored coordinates are valid
	  }  // relations are to be converted to nodes
          else {  // oo.dependencystage()!=33 OR not --all-to-nodes
            wo.relation(id,hisver,histime,hiscset,in.hisuid,hisuser);
            refidp= refid; reftypep= reftype; refrolep= refrole;
            while(refidp<refide) {  // for every referenced object
              ri= *refidp;
              rt= *reftypep;
              rr= *refrolep;

              if(oo.dependencystage()<33) {
		// not:
		// 33:     write each relation which has a flag in ht
		//           to output;
		//         use temporary .o5m file as input;
                if(rt==2 || hash_geti(rt,ri)) {
		  // referenced object is a relation OR
		  // lies inside the borders
                  wo.ref(ri,rt,rr);
                  if(rt!=2 && !in) {
                    hash_seti(2,id); in= true; }
		}
                else {  // referenced object lies outside the borders
                  if(!global.dropbrokenrefs()) {
                    wo.ref(ri,rt,rr);
		  }
		}
	      }
              else {  // oo.dependencystage()==33
                // 33:     write each relation which has a flag in ht
                //           to output;
                //         use temporary .o5m file as input;
                if(!global.dropbrokenrefs() || hash_geti(rt,ri)) {
		  // broken refs are to be listed anyway OR
		  // referenced object lies inside the borders
                  wo.ref(ri,rt,rr);
		}
	      }
              refidp++; reftypep++; refrolep++;
	    }  // end   for every referenced object
            keyp= key; valp= val;
            while(keyp<keye)  // for all key/val pairs of this object
              wo.wayrel_keyval(*keyp++,*valp++);
            wo.relation_close();
	  }  // stage!=3 OR not --all-to-nodes
	}  // end   no borders OR at least one node inside
      }  // end   not relations to drop
    }  // write relation
  }
};



int OO2::read_all_input_files () {
  In in;
  if(oo.ifp > oo.inf.begin()) {  // at least one input file open

    // get next object - if .pbf
    //read_input(); (must not be here because of diffcompare)
    if(oo.ifp->format==-1) {
      if(!oo.alreadyhavepbfobject)
	pb_input(false);
      while(read.pbf.type>2)  // unknown pbf object
	pb_input(false);  // get next object
    }

    // merging - if more than one file
    merge_multiple_files();

    // get next data
    read.input();

  }  // at least one input file open

  // care about end of input file
  if(
     (oo.ifp> oo.inf.begin())
     || 
     (read.bufp()>=read.bufe()
      && 
      (oo.ifp->format>=0)	
      ) ||
     (oo.ifp->format==-1 && read.pbf.type<0)) {  // at end of input file
    if(oo.ifp> oo.inf.begin()) {
      if(oo.ifp->format==-1 && read.pbf.type<0) {
	if(read.pbf.type<-1)  // error
	  return 1000-read.pbf.type;
	oo.ifp->endoffile= true;
      }
      oo.close();
    }
    if(oo.ifp> oo.inf.begin())  // still input files
      oo._switch();
    else {  // at end of all input files
      // care about recursive processing
      if(oo.dependencystage()==11) {
	// 11:     no output;
	//         for each node which is inside the borders,
	//           set flag in ht;
	//         store start of ways in read_setjump();
	//         for each way which has a member with flag in ht,
	//           set the way's flag in ht;
	//         for each relation with a member with flag in ht,
	//           store the relation's flag and write the ids
	//           of member ways which have no flag in ht
	//           (use cww_);
	// 11->12: at all files' end:
	//         let all files jump to start of ways,
	//         use read_jump();
	//         set flags for ways, use cww_processing_set();
	if(oo.jumpall())
	  return 28;
	cww_processing_set();
	oo.dependencystage(12);
	// 12:     no output;
	//         for each way with a member with a flag in ht,
	//           set the way's flag in ht and write the ids
	//           of all the way's members (use cwn_);
	docontinue();  // do not write this object
      }
      if(oo.dependencystage()==21 || oo.dependencystage()==12) {
	// 12:     no output;
	//         for each way with a member with a flag in ht,
	//           set the way's flag in ht and write the ids
	//           of all the way's members (use cwn_);
	// 21:     no output;
	//         for each node inside the borders,
	//           set flag in ht;
	//         for each way with a member with a flag in ht,
	//           set the way's flag in ht and write the ids
	//           of all the way's members (use cwn_);
	// 12->22: as soon as first relation shall be written:
	//         rewind all files;
	//         set flags for nodes, use cwn_processing();
	// 21->22: as soon as first relation shall be written:
	//         rewind all files;
	//         set flags for nodes, use cwn_processing();
	if(oo.rewindall())
	  return 28;
	cwn.processing();
	oo.dependencystage(22);
	// 22:     write each node which has a flag in ht to output;
	//         write each way which has a flag in ht to output;
	docontinue();  // do not write this object
      }

      oo_dependencystage_ne_32();
      // here: dependencystage==32
      // 32:     for each relation with a member with a flag
      //           in ht, set the relation's flag in ht;
      //         for each relation,
      //           write its id and its members' ids
      //           into a temporary file (use rr_);
      //         if option --all-to-nodes is set, then
      //           for each relation, write its members'
      //             geopositions into a temporary file (use posr_);
      // 32->33: at all files' end:
      //         process all interrelation references (use rr_);
      //         if option --all-to-nodes is set, then
      //           process position array (use posr_);
      //         switch input to the temporary .o5m file;
      //         switch output to regular output file;
      if(!global.outnone()) {
	wo.flush();
	wo.reset();
	wo.end();
	wo.flush();
      }
      if(writer.newfile(NULL))
	return 21;
      if(!global.outnone()) {
	wo.format(wformat);
	wo.reset();
      }
      if(hashactive)
	oo.rrprocessing(&maxrewind);
      if(global.alltonodes())
	posr.processing(&maxrewind_posr,in.refxy);
      oo.dependencystage(33);  // enter next stage
      oo.tyidold(0);  // allow the next object to be written
      if(oo_open(o5mtempfile))
	return 22;
      if(oo.getformat())
	return 23;
      read.input();
      docontinue();
    }  // at end of all input files
  }  // at end of input file

  // care about unexpected contents at file end
  if(oo.dependencystage()<=31)
    // 31:     for each node inside the borders,
    //           set flag in ht;
    //         for each way with a member with a flag in ht,
    //           set the way's flag in ht;
    if(oo.ifp->endoffile) {  // after logical end of file
      WARNv("osmconvert Warning: unexpected contents "
	    "after logical end of file: %.80s",oo.ifp->filename);
      dobreak();
    }
    
 readobject:
  bufp= read.bufpbv();
  b= *bufp; 
  c= (char)b;

  // care about header and unknown objects
  if(oo.ifp->format<0) {  // -1, pbf
    read_pbf();
  }  // end   pbf
  else if(oo.ifp->format==0) {  // o5m
    read_o5m();
  }  // end   o5m
  else {  // xml
    read_xml();
  }  // end   xml

  // write header
  if(writeheader) {
    writeheader= false;
    if(!global.outnone())
      wo.start(wformat,oo.bbvalid,
	       oo.bbx1,oo.bby1,oo.bbx2,oo.bby2,oo.timestamp);
  }

  // object initialization
  if(!diffcompare) {  // regularly read the object
    in.hisver= 0;
    in.histime= 0;
    in.hiscset= 0;
    in.hisuid= 0;
    //      hisuser="";
    //refide= refid.end();
    //      reftypee= reftype.end();
    //      refrolee= refrole.end();
    //      keye= key;
    vale= val;
  }  // regularly read the object
  if(oo.ifp->deleteobject==1) oo.ifp->deleteobject= 0;


  // read one osm object
  if(oo.ifp->format<0) {  // pbf
    // read id
    in.id= read.pbf.id;
    // read coordinates (for nodes only)
    if(in.otype==0) {  // node
      in.node.lon()= read.pbf.lon; 
      in.node.lat()= read.pbf.lat;
    }  // node
    // read author
    in.hisver= read.pbf.hisver;
    if(in.hisver!=0) {  // author information available
      in.histime= read.pbf.histime;
      if(in.histime!=0) {
	in.hiscset= read.pbf.hiscset;
	in.hisuid= read.pbf.hisuid;
	in.hisuser= read.pbf.hisuser;
      }
    }  // end   author information available
    oo.ifp->deleteobject= read.pbf.hisvis==0? 1: 0;
    // read noderefs (for ways only)
    if(in.otype==1)  // way
      read.pbf.noderef(refid,global.maxrefs());
    // read refs (for relations only)
    else if(in.otype==2) {  // relation
      l= read.pbf.ref(refid,reftype,refrole,global.maxrefs());
      calcendref(refid,l);
      calcendtype(reftype,l);
      calcendrole(refrole,l);
    }  // end   relation
    // read node key/val pairs
    l= read.pbf.keyval(key,val);
    //      keye= key+l; vale= val+l;
  }  // end   pbf
  else if(oo.ifp->format==0) {  // o5m
    read_o5m2();
  }  // end   o5m
  else {  // xml
    read_xml2();
  }
  // care about multiple occurrences of one object within one file
  if(global.mergeversions()) {
    // user allows duplicate objects in input file; that means
    // we must take the last object only of each duplicate
    // because this is assumed to be the newest;
    uint64_t tyidold;

    tyidold= oo.ifp->tyid;
    if(oo.gettyid()==0) {
      if(oo.ifp->tyid==tyidold)  // next object has same type and id
	goto readobject;  // dispose this object and take the next
    }
    oo.ifp->tyid= tyidold;
  }

  // care about possible array overflows
  if(refide>=refidee)
    PERRv1("%s %ld has too many refs.",ONAME(in.otype),in.id);

  //TODO: if(keye>=keyee)
  //   PERRv2("%s %Ld has too many key/val pairs.",
  // 	     ONAME(otype),id);

  // care about diffs and sequence
  if(global.diffcontents()) {  // diff contents is to be considered
    // care about identical contents if calculating a diff
    if(oo.ifp_ne_inf()) {
      // second file and there is a similar object in the first file
      // and version numbers are different
      diffcompare= true;  // compare with next object, do not read
      diffdifference= false;  // (default)
      docontinue();  // no check the first file
    }
  }  // diff contents is to be considered
  else {  // no diff contents is to be considered
    // stop processing if object is to ignore because of duplicates
    // in same or other file(s)
    if(oo.ifp->tyid<=oo.tyidold())
      docontinue();
    oo.tyidold(0);
    if(oo.ifp_not_first())
      oo.tyidold((*oo.ifp).tyid);
    // stop processing if in wrong stage for nodes or ways
    if(oo.dependencystage()>=32 && in.otype<=1)
      // 32:     for each relation with a member with a flag
      //           in ht, set the relation's flag in ht;
      //         for each relation,
      //           write its id and its members' ids
      //           into a temporary file (use rr_);
      //         if option --all-to-nodes is set, then
      //           for each relation, write its members'
      //             geopositions into a temporary file (use posr_);
      // 33:     write each relation which has a flag in ht
      //           to output; use temporary .o5m file as input;
      docontinue();  // ignore this object
    // check sequence, if necessary
    if(oo.ifp == oo.inf.begin() && oo.dependencystage()!=33) {
      // not:
      // 33:     write each relation which has a flag in ht
      //           to output; use temporary .o5m file as input;
      if(in.otype<=oo.sequencetype() &&
	 (in.otype<oo.sequencetype() || in.id<oo.sequenceid() ||
	  (oo.ifp> oo.inf.begin() && in.id<=oo.sequenceid()))) {
	oo.error= 92;
	WARNv("wrong sequence at %s %Ld",
	      ONAME(oo.sequencetype()),oo.sequenceid());
	WARNv("next object is %s %Ld",ONAME(in.otype),in.id);
      }
    }  // dependencystage>=32
  }  // no diff contents is to be considered
  oo.sequencetype(in.otype); oo.sequenceid(in.id);

  // care about calculating a diff file
  if(global.diff()) {  // diff
    if(oo.ifp==oo.inf.begin()) {  // first file has been chosen
      if(diffcompare) {
	diffcompare= false;
	if(!diffdifference)
	  docontinue();  // there has not been a change in object's contents
	oo.ifp->deleteobject= 0;
      }
      else {
	//if(global.diffcontents && oo_ifn<2) docontinue(); TODO
	oo.ifp->deleteobject= 1;  // add "delete request";
      }
    }
    else {  // second file has been chosen
      if(oo.ifp->tyid==oo.inf.begin()->tyid &&
	 oo.ifp->hisver==oo.inf.begin()->hisver)
	// there is a similar object in the first file
	docontinue();  // ignore this object
    }  // end   second file has been chosen
  }  // end   diff

  // care about dependency stages
  dependancy_stages();
  // process object deletion
  if(oo.ifp->deleteobject!=0) {  // object is to delete
    if((in.otype==0 && !global.dropnodes()) ||
       (in.otype==1 && !global.dropways()) ||
       (in.otype==2 && !global.droprelations()))
      // section is not to drop anyway
      if(global.outo5c() || global.outosc() || global.outosh())
	// write o5c, osc or osh file
	wo.dodelete(in);
    // write delete request
    docontinue();  // end processing for this object
  }  // end   object is to delete


  // abort writing if user does not want any standard output
  if(global.outnone())
    docontinue();

  write_objects();
}



void OO2::read_xml2(){
  int64_t ri;  // temporary, refid, rcomplete flag 1
  int rt;  // temporary, reftype, rcomplete flag 2
  charv_t::iterator rr;  // temporary, refrole, rcomplete flag 3
  int rcomplete;
  char* k;  // temporary, key
  char* v;  // temporary, val
  int r;
    
  read.bufpinc();  // jump over '<'
  oo.xmlheadtag= true;  // (default)
  rcomplete= 0;
  k= v= NULL;
  for(;;) {  // until break;
    r= oo.xmltag();
    if(oo.xmlheadtag) {  // still in object header
      if(oo.xmlkey(0)=='i' && oo.xmlkey(1)=='d') // id
	in.id= oo.strtosint64(oo.xmlvals());
      else if(oo.xmlkey(0)=='l') {  // letter l
	if(oo.xmlkey(1)=='o') // lon
	  in.node.lon()= oo.strtodeg(oo.xmlvals());
	else if(oo.xmlkey(1)=='a') // lon
	  in.node.lat()= oo.strtodeg(oo.xmlvals());
      }  // end   letter l
      else if(oo.xmlkey(0)=='v' && oo.xmlkey(1)=='i') {  // visible
	if(oo.xmlval(0)=='f' || oo.xmlval(0)=='n')
	  if(oo.ifp->deleteobject==0)
	    oo.ifp->deleteobject= 1;
      }  // end   visible
      else if(oo.xmlkey(0)=='a' && oo.xmlkey(1)=='c') {  // action
	if(oo.xmlval(0)=='d' && oo.xmlval(1)=='e')
	  if(oo.ifp->deleteobject==0)
	    oo.ifp->deleteobject= 1;
      }  // end   action
      else if(!global.dropversion()) {  // version not to drop
	if(oo.xmlkey(0)=='v' && oo.xmlkey(1)=='e') // hisver
	  in.hisver= oo.strtouint32(oo.xmlvals());
	if(!global.dropauthor()) {  // author not to drop
	  if(oo.xmlkey(0)=='t') // histime
	    in.histime= oo.strtimetosint64(oo.xmlvals());
	  else if(oo.xmlkey(0)=='c') // hiscset
	    in.hiscset= oo.strtosint64(oo.xmlvals());
	  else if(oo.xmlkey(0)=='u' && oo.xmlkey(1)=='i') // hisuid
	    in.hisuid= oo.strtouint32(oo.xmlvals());
	  else if(oo.xmlkey(0)=='u' && oo.xmlkey(1)=='s') //hisuser
	    in.hisuser= oo.xmlvals();
	}  // end   author not to drop
      }  // end   version not to drop
    }  // end   still in object header
    else {  // in object body
      if(oo.xmlkey(0)==0) {  // xml tag completed
	if(rcomplete>=3) {  // at least refid and reftype
	  *refide++= ri;
	  *reftypee++= rt;
	  if(rcomplete<4)  // refrole is missing
	    setrr(rr,"");  // assume an empty string as refrole
	  refrolee++;
	  //TODO:*refrolee= rr;
	}  // end   at least refid and reftype
	rcomplete= 0;
	if(v!=NULL && k!=NULL) {  // key/val available
	  //TODO:              *keye++= k; 
	  //*vale++= v;
	  k= v= NULL;
	}  // end   key/val available
      }  // end   xml tag completed
      else {  // inside xml tag
	if(in.otype!=0 && refide<refidee) {
	  // not a node AND still space in refid array
	  if(oo.xmlkey(0)=='r' && oo.xmlkey(1)=='e') { // refid
	    ri= oo.strtosint64(oo.xmlvals()); 
	    rcomplete|= 1;
		
	    if(in.otype==1) {
	      rt= 0; 
	      rcomplete|= 2;
	    } 
	  }
	  else if(oo.xmlkey(0)=='t' && oo.xmlkey(1)=='y') {
	    // reftype
	    rt= oo.xmlval(0)=='n'? 0: oo.xmlval(0)=='w'? 1: 2;
	    rcomplete|= 2; }
	  else if(oo.xmlkey(0)=='r' && oo.xmlkey(1)=='o') {
	    // refrole
	    //TODO:rr= oo.xmlvals(); 
	    rcomplete|= 4; 
	  }
	}  // end   still space in refid array

	// TODO:// if(keye<keyee) {  // still space in key/val array
	// //   if(oo.xmlkey(0)=='k') // key
	// //     k= oo.xmlval;
	// //   else if(oo.xmlkey(0)=='v') // val
	// //     v= oo.xmlval;
	// // }  // end   still space in key/val array

      }  // end   inside xml tag

    }  // end   in object body
    if(r)
      dobreak();
  }  // end   until break;
}  // end   xml


void OO2::read_xml2(In & in){
  int64_t ri;  // temporary, refid, rcomplete flag 1
  int rt;  // temporary, reftype, rcomplete flag 2
  charv_t::iterator rr;  // temporary, refrole, rcomplete flag 3
  int rcomplete;
  char* k;  // temporary, key
  char* v;  // temporary, val
  int r;
    
  read.bufpinc();  // jump over '<'
  oo.xmlheadtag= true;  // (default)
  rcomplete= 0;
  k= v= NULL;
  for(;;) {  // until break;
    r= oo.xmltag();
    if(oo.xmlheadtag) {  // still in object header
      if(oo.xmlkey(0)=='i' && oo.xmlkey(1)=='d') // id
	in.id= oo.strtosint64(oo.xmlvals());
      else if(oo.xmlkey(0)=='l') {  // letter l
	if(oo.xmlkey(1)=='o') // lon
	  in.node.lon()= oo.strtodeg(oo.xmlvals());
	else if(oo.xmlkey(1)=='a') // lon
	  in.node.lat()= oo.strtodeg(oo.xmlvals());
      }  // end   letter l
      else if(oo.xmlkey(0)=='v' && oo.xmlkey(1)=='i') {  // visible
	if(oo.xmlval(0)=='f' || oo.xmlval(0)=='n')
	  if(oo.ifp->deleteobject==0)
	    oo.ifp->deleteobject= 1;
      }  // end   visible
      else if(oo.xmlkey(0)=='a' && oo.xmlkey(1)=='c') {  // action
	if(oo.xmlval(0)=='d' && oo.xmlval(1)=='e')
	  if(oo.ifp->deleteobject==0)
	    oo.ifp->deleteobject= 1;
      }  // end   action
      else if(!global.dropversion()) {  // version not to drop
	if(oo.xmlkey(0)=='v' && oo.xmlkey(1)=='e') // hisver
	  in.hisver= oo.strtouint32(oo.xmlvals());
	if(!global.dropauthor()) {  // author not to drop
	  if(oo.xmlkey(0)=='t') // histime
	    in.histime= oo.strtimetosint64(oo.xmlvals());
	  else if(oo.xmlkey(0)=='c') // hiscset
	    in.hiscset= oo.strtosint64(oo.xmlvals());
	  else if(oo.xmlkey(0)=='u' && oo.xmlkey(1)=='i') // hisuid
	    in.hisuid= oo.strtouint32(oo.xmlvals());
	  else if(oo.xmlkey(0)=='u' && oo.xmlkey(1)=='s') //hisuser
	    in.hisuser= oo.xmlvals();
	}  // end   author not to drop
      }  // end   version not to drop
    }  // end   still in object header
    else {  // in object body
      if(oo.xmlkey(0)==0) {  // xml tag completed
	if(rcomplete>=3) {  // at least refid and reftype
	  refid.push_back(ri);

	  if(rcomplete<4)  // refrole is missing
	    setrr(rr,"");  // assume an empty string as refrole
	  refrolee++;
	  //TODO:*refrolee= rr;
	}  // end   at least refid and reftype
	rcomplete= 0;
	if(v!=NULL && k!=NULL) {  // key/val available
	  //TODO:              *keye++= k; 
	  //*vale++= v;
	  k= v= NULL;
	}  // end   key/val available
      }  // end   xml tag completed
      else {  // inside xml tag
	if(in.otype!=0 && refide<refidee) {
	  // not a node AND still space in refid array
	  if(oo.xmlkey(0)=='r' && oo.xmlkey(1)=='e') { // refid
	    ri= oo.strtosint64(oo.xmlvals()); 
	    rcomplete|= 1;
		
	    if(in.otype==1) {
	      rt= 0; 
	      rcomplete|= 2;
	    } 
	  }
	  else if(oo.xmlkey(0)=='t' && oo.xmlkey(1)=='y') {
	    // reftype
	    rt= oo.xmlval(0)=='n'? 0: oo.xmlval(0)=='w'? 1: 2;
	    rcomplete|= 2; }
	  else if(oo.xmlkey(0)=='r' && oo.xmlkey(1)=='o') {
	    // refrole
	    //TODO:rr= oo.xmlvals(); 
	    rcomplete|= 4; 
	  }
	}  // end   still space in refid array

	// TODO:// if(keye<keyee) {  // still space in key/val array
	// //   if(oo.xmlkey(0)=='k') // key
	// //     k= oo.xmlval;
	// //   else if(oo.xmlkey(0)=='v') // val
	// //     v= oo.xmlval;
	// // }  // end   still space in key/val array

      }  // end   inside xml tag

    }  // end   in object body
    if(r)
      dobreak();
  }  // end   until break;
}  // end   xml
}

void OO2::read_pbf () {
  if(read.pbf.type<0 || read.pbf.type>2)  // not a regular dataset id
    docontinue;
  in.otype= read.pbf.type;
  oo.alreadyhavepbfobject= false;
}
