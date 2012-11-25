#ifndef INC_PROCESS_HPP
#define INC_PROCESS_HPP

// commands
class Process {
public:
  bool alltonodes();
  bool alltonodes_bbox();
  bool border_box();
  bool border_file();
  bool completeways();
  bool completeways(bool v);
  bool complexways();
  bool complexways(bool v);
  bool csv_ini(const char *);
  bool csvheadline();
  const char * csvseparator();
  bool diff();
  bool diffcontents();
  bool dropauthor();
  bool dropbrokenrefs();
  bool dropnodes();
  bool droprelations();
  bool dropversion();
  bool dropways();
  bool emulateosmium();
  bool emulateosmosis();
  bool emulatepbf2osm();
  bool fakeauthor();
  bool fakelonlat();
  bool fakeversion();
  size_t maxobjects();
  size_t maxrefs();
  bool mergeversions();
  bool otypeoffset10();
  bool otypeoffset20();
  bool otypeoffsetstep();
  bool outcsv();
  bool outnone();
  bool outo5c();
  bool outo5m();
  bool outosc();
  bool outosh();
  bool outosm();
  bool outpbf();
  bool outstatistics();
  bool outtimestamp();
  bool statistics();
  char * tempfilename();
  bool timestamp();
  bool write_testmode();
  int oo_ifn();
  bool otypeoffset05();
  
};

#endif
