static inline void wo__author(int32_t hisver,int64_t histime,
    int64_t hiscset,uint32_t hisuid,const char* hisuser) {
  // write osm object author;
  // must not be called if writing PBF format;
  // hisver: version; 0: no author is to be written
  //                     (necessary if o5m format);
  // histime: time (seconds since 1970)
  // hiscset: changeset
  // hisuid: uid
  // hisuser: user name
  // global_fakeauthor: the author contents will be faked that way
  //                     that the author data will be as short as
  //                     possible;
  // global fakeversion: same as global_fakeauthor, but for .osm
  //                     format: just the version will be written;
  // note that when writing o5m format, this procedure needs to be
  // called even if there is no author information to be written;
  // PBF and csv: this procedure is not called;
  if(global_fakeauthor|global_fakeversion) {
    hisver= 1; histime= 1; hiscset= 1; hisuid= 0; hisuser= "";
    }
  if(wo__format==0) {  // o5m
    if(hisver==0 || global_dropversion)  // no version number
      o5_byte(0x00);
    else {  // version number available
      o5_uvar32(hisver);
      if(global_dropauthor) histime= 0;
      o5_svar64(histime-o5_time); o5_time= histime;
      if(histime!=0) {
          // author information available
        o5_svar64(hiscset-o5_cset); o5_cset= hiscset;
        if(hisuid==0 || hisuser==NULL || hisuser[0]==0)
            // user identification not available
          stw_write("","");
        else {  // user identification available
          byte uidbuf[30];

          uidbuf[o5_uvar32buf(uidbuf,hisuid)]= 0;
          stw_write((const char*)uidbuf,hisuser);
          }  // end   user identification available
        }  // end   author information available
      }  // end   version number available
return;
    }  // end   o5m
  // here: XML format
  if(global_fakeversion) {
    write_str("\" version=\"1");
return;
    }
  if(hisver==0 || global_dropversion)  // no version number
return;
  switch(wo__format) {  // depending on output format
  case 11:  // native XML
    write_str("\" version=\""); write_uint32(hisver);
    if(histime!=0 && !global_dropauthor) {
      write_str("\" timestamp=\""); write_timestamp(histime);
      write_str("\" changeset=\""); write_uint64(hiscset);
      if(hisuid!=0 && hisuser[0]!=0) {  // user information available
        write_str("\" uid=\""); write_uint32(hisuid);
        write_str("\" user=\""); write_xmlstr(hisuser);
        }
      }
    break;
  case 12:  // pbf2osm XML
    write_str("\" version=\""); write_uint32(hisver);
    if(histime!=0 && !global_dropauthor) {
      write_str("\" changeset=\""); write_uint64(hiscset);
      if(hisuid!=0 && hisuser[0]!=0) {  // user information available
        write_str("\" user=\""); write_xmlstr(hisuser);
        write_str("\" uid=\""); write_uint32(hisuid);
        }
      write_str("\" timestamp=\""); write_timestamp(histime);
      }
    break;
  case 13:  // Osmosis XML
    write_str("\" version=\""); write_uint32(hisver);
    if(histime!=0 && !global_dropauthor) {
      write_str("\" timestamp=\""); write_timestamp(histime);
      if(hisuid!=0 && hisuser[0]!=0) {  // user information available
        write_str("\" uid=\""); write_uint32(hisuid);
        write_str("\" user=\""); write_xmlmnstr(hisuser);
        }
      write_str("\" changeset=\""); write_uint64(hiscset);
      }
    break;
  case 14:  // Osmium XML
    write_str("\" version=\""); write_uint32(hisver);
    if(histime!=0 && !global_dropauthor) {
      write_str("\" changeset=\""); write_uint64(hiscset);
      write_str("\" timestamp=\""); write_timestamp(histime);
      if(hisuid!=0 && hisuser[0]!=0) {  // user information available
        write_str("\" uid=\""); write_uint32(hisuid);
        write_str("\" user=\""); write_xmlstr(hisuser);
        }
      }
    break;
    }  // end   depending on output format
  if(global_outosh) {
    if(wo__lastaction==3)
      write_str("\" visible=\"false");
    else
      write_str("\" visible=\"true");
    }
  }  // end   wo__author()

static inline void wo__action(int action) {
  // set one of these action tags: "create", "modify", "delete";
  // write tags only if 'global_outosc' is true;
  // must only be called if writing XML format;
  // action: 0: no action tag; 1: "create"; 2: "modify"; 3: "delete";
  //         caution: there is no check for validity of this parameter;
  static const char* starttag[]=
    {"","<create>" NL,"<modify>" NL,"<delete>" NL};
  static const char* endtag[]=
    {"","</create>" NL,"</modify>" NL,"</delete>" NL};

  if(global_outosc && action!=wo__lastaction) {  // there was a change
    write_str(endtag[wo__lastaction]);  // end last action
    write_str(starttag[action]);  // start new action
    }
  wo__lastaction= action;
  }  // end   wo__action()

static void wo_start(int format,bool bboxvalid,
    int32_t x1,int32_t y1,int32_t x2,int32_t y2,int64_t timestamp) {
  // start writing osm objects;
  // format: 0: o5m; 11: native XML;
  //         12: pbf2osm; 13: Osmosis; 14: Osmium; 21:csv; -1: PBF;
  // bboxvalid: the following bbox coordinates are valid;
  // x1,y1,x2,y2: bbox coordinates (base 10^-7);
  // timestamp: file timestamp; ==0: no timestamp given;
  if(format<-1 || (format >0 && format<11) ||
      (format >14 && format<21) || format>21) format= 0;
  wo__format= format;
  wo__logaction= global_outosc || global_outosh;
  if(wo__format==0) {  // o5m
    static const byte o5mfileheader[]= {0xff,0xe0,0x04,'o','5','m','2'};
    static const byte o5cfileheader[]= {0xff,0xe0,0x04,'o','5','c','2'};

    if(global_outo5c)
      write_mem(o5cfileheader,sizeof(o5cfileheader));
    else
      write_mem(o5mfileheader,sizeof(o5mfileheader));
    if(timestamp!=0) {  // timestamp has been supplied
      o5_byte(0xdc);  // timestamp
      o5_svar64(timestamp);
      o5_write();  // write this object
      }
    if(border_active)  // borders are to be applied
      border_querybox(&x1,&y1,&x2,&y2);
    if(border_active || bboxvalid) {
        // borders are to be applied OR bbox has been supplied
      o5_byte(0xdb);  // border box
      o5_svar32(x1); o5_svar32(y1);
      o5_svar32(x2); o5_svar32(y2);
      o5_write();  // write this object
      }
return;
    }  // end   o5m
  if(wo__format<0) {  // PBF
    if(border_active)  // borders are to be applied
      border_querybox(&x1,&y1,&x2,&y2);
    bboxvalid= bboxvalid || border_active;
    pw_ini();
    pw_header(bboxvalid,x1,y1,x2,y2,timestamp);
return;
    }
  if(wo__format==21) {  // csv
    csv_headline();
return;
    }
  // here: XML
  if(wo__format!=14)
    write_str("<?xml version=\'1.0\' encoding=\'UTF-8\'?>" NL);
  else  // Osmium XML
    write_str("<?xml version=\"1.0\" encoding=\"UTF-8\"?>" NL);
  if(global_outosc)
      write_str("<osmChange version=\"0.6\"");
  else
      write_str("<osm version=\"0.6\"");
  switch(wo__format) {  // depending on output format
  case 11:  // native XML
    write_str(" generator=\"osmconvert " VERSION 
"\"");
    break;
  case 12:  // pbf2osm XML
    write_str(" generator=\"pbf2osm\"");
    break;
  case 13:  // Osmosis XML
    write_str(" generator=\"Osmosis 0.40\"");
    break;
  case 14:  // Osmium XML
    write_str(" generator="
      "\"Osmium (http://wiki.openstreetmap.org/wiki/Osmium)\"");
    break;
    }  // end   depending on output format
  if(timestamp!=0) {
    write_str(" timestamp=\""); write_timestamp(timestamp);
    write_char('\"');
    }
  write_str(">" NL);
  if(wo__format!=12) {  // bbox may be written
    if(border_active)  // borders are to be applied
      border_querybox(&x1,&y1,&x2,&y2);
    if(border_active || bboxvalid) {  // borders are to be applied OR
        // bbox has been supplied
      if(wo__format==13) {  // Osmosis
        // <bound box="53.80000,10.50000,54.00000,10.60000"
        //  origin="0.40.1"/>
        write_str("  <bound box=\""); write_sfix7(y1);
        write_str(","); write_sfix7(x1);
        write_str(","); write_sfix7(y2);
        write_str(","); write_sfix7(x2);
        write_str("\" origin=\"0.40\"/>" NL);
        }  // Osmosis
      else {  // not Osmosis
        // <bounds minlat="53.8" minlon="10.5" maxlat="54."
        //  maxlon="10.6"/>
        write_str("\t<bounds minlat=\""); write_sfix7(y1);
        write_str("\" minlon=\""); write_sfix7(x1);
        write_str("\" maxlat=\""); write_sfix7(y2);
        write_str("\" maxlon=\""); write_sfix7(x2);
        write_str("\"/>" NL);
        }  // not Osmosis
      }
    }  // end   bbox may be written
  }  // end   wo_start()

static void wo_end() {
  // end writing osm objects;
  switch(wo__format) {  // depending on output format
  case 0:  // o5m
    o5_write();  // write last object - if any
    write_char(0xfe);  // write o5m eof indicator
    break;
  case 11:  // native XML
  case 12:  // pbf2osm XML
  case 13:  // Osmosis XML
  case 14:  // Osmium XML
    wo__CLOSE
    wo__action(0);
    write_str(global_outosc? "</osmChange>" NL: "</osm>" NL);
    if(wo__format>=12)
      write_str("<!--End of emulated output.-->" NL);
    break;
  case 21:  // csv
    csv_write();
      // (just in case the last object has not been terminated)
    break;
  case -1:  // PBF
    pw_foot();
    break;
    }  // end   depending on output format
  }  // end   wo_end()

static inline void wo_flush() {
  // write temporarily stored object information;
  if(wo__format==0)  // o5m
    o5_write();  // write last object - if any
  else if(wo__format<0)  // PBF format
    pw_flush();
  else if(wo__format==21)  // csv
    csv_write();
  else  // any XML output format
    wo__CLOSE
  write_flush();
  }  // end   wo_flush()

static int wo_format(int format) {
  // get or change output format;
  // format: -9: return the currently used format, do not change it;
  if(format==-9)  // do not change the format
return wo__format;
  wo_flush();
  if(format<-1 || (format >0 && format<11) ||
      (format >14 && format<21) || format>21) format= 0;
  wo__format= format;
  wo__logaction= global_outosc || global_outosh;
  return wo__format;
  }  // end   wo_format()

static inline void wo_reset() {
  // in case of o5m format, write a Reset;
  // note that this is done automatically at every change of
  // object type; this procedure offers to write additional Resets
  // at every time you want;
  if(wo__format==0)
    o5_reset();
  }  // end   wo_reset()

static inline void wo_node(int64_t id,
    int32_t hisver,int64_t histime,int64_t hiscset,
    uint32_t hisuid,const char* hisuser,int32_t lon,int32_t lat) {
  // write osm node body;
  // id: id of this object;
  // hisver: version; 0: no author information is to be written
  //                     (necessary if o5m format);
  // histime: time (seconds since 1970)
  // hiscset: changeset
  // hisuid: uid
  // hisuser: user name
  // lon: latitude in 100 nanodegree;
  // lat: latitude in 100 nanodegree;
  if(wo__format==0) {  // o5m
    o5_write();  // write last object - if any
    o5_type(0);
    o5_byte(0x10);  // data set id for node
    o5_svar64(id-o5_id); o5_id= id;
    wo__author(hisver,histime,hiscset,hisuid,hisuser);
    o5_svar32(lon-o5_lon); o5_lon= lon;
    o5_svar32(lat-o5_lat); o5_lat= lat;
return;
    }  // end   o5m
  if(wo__format<0) {  // PBF
    pw_node(id,hisver,histime,hiscset,hisuid,hisuser,lon,lat);
return;
    }
  if(wo__format==21) {  // csv
    char s[20];

    if(csv_key_otype)
      csv_add("@otype","0");
    if(csv_key_oname)
      csv_add("@oname",ONAME(0));
    if(csv_key_id) {
      int64toa(id,s);
      csv_add("@id",s);
      }
    if(csv_key_uid) {
      uint32toa(hisuid,s);
      csv_add("@uid",s);
      }
    if(csv_key_user)
      csv_add("@user",hisuser);
    if(csv_key_lon) {
      write_createsfix7o(lon,s);
      csv_add("@lon",s);
      }
    if(csv_key_lat) {
      write_createsfix7o(lat,s);
      csv_add("@lat",s);
      }
return;
    }
  // here: XML format
  wo__CLOSE
  if(wo__logaction)
    wo__action(hisver==1? 1: 2);
  switch(wo__format) {  // depending on output format
  case 11:  // native XML
    write_str("\t<node id=\""); write_sint64(id);
    write_str("\" lat=\""); write_sfix7(lat);
    write_str("\" lon=\""); write_sfix7(lon);
    wo__author(hisver,histime,hiscset,hisuid,hisuser);
    wo__xmlclosetag= "\t</node>" NL;  // preset close tag
    break;
  case 12:  // pbf2osm XML
    write_str("\t<node id=\""); write_sint64(id);
    write_str("\" lat=\""); write_sfix7o(lat);
    write_str("\" lon=\""); write_sfix7o(lon);
    wo__author(hisver,histime,hiscset,hisuid,hisuser);
    wo__xmlclosetag= "\t</node>" NL;  // preset close tag
    break;
  case 13:  // Osmosis XML
    write_str("  <node id=\""); write_sint64(id);
    wo__author(hisver,histime,hiscset,hisuid,hisuser);
    write_str("\" lat=\""); write_sfix7(lat);
    write_str("\" lon=\""); write_sfix7(lon);
    wo__xmlclosetag= "  </node>" NL;  // preset close tag
    break;
  case 14:  // Osmium XML
    write_str("  <node id=\""); write_sint64(id);
    wo__author(hisver,histime,hiscset,hisuid,hisuser);
    if(lon>=0) lon= (lon+5)/10;
    else lon= (lon-5)/10;
    if(lat>=0) lat= (lat+5)/10;
    else lat= (lat-5)/10;
    write_str("\" lon=\""); write_sfix6o(lon);
    write_str("\" lat=\""); write_sfix6o(lat);
    wo__xmlclosetag= "  </node>" NL;  // preset close tag
    break;
    }  // end   depending on output format
  wo__xmlshorttag= true;  // (default)
  }  // end   wo_node()

static inline void wo_node_close() {
  // complete writing an OSM node;
  if(wo__format<0)
    pw_node_close();
  else if(wo__format==21)
    csv_write();
  }  // end   wo_node_close()

static inline void wo_way(int64_t id,
    int32_t hisver,int64_t histime,int64_t hiscset,
    uint32_t hisuid,const char* hisuser) {
  // write osm way body;
  // id: id of this object;
  // hisver: version; 0: no author information is to be written
  //                     (necessary if o5m format);
  // histime: time (seconds since 1970)
  // hiscset: changeset
  // hisuid: uid
  // hisuser: user name
  if(wo__format==0) {  // o5m
    o5_write();  // write last object - if any
    o5_type(1);
    o5_byte(0x11);  // data set id for way
    o5_svar64(id-o5_id); o5_id= id;
    wo__author(hisver,histime,hiscset,hisuid,hisuser);
    o5_markref(0);
return;
    }  // end   o5m
  if(wo__format<0) {  // PBF
    pw_way(id,hisver,histime,hiscset,hisuid,hisuser);
return;
    }
  if(wo__format==21) {  // csv
    char s[20];

    if(csv_key_otype)
      csv_add("@otype","1");
    if(csv_key_oname)
      csv_add("@oname",ONAME(1));
    if(csv_key_id) {
      int64toa(id,s);
      csv_add("@id",s);
      }
    if(csv_key_uid) {
      uint32toa(hisuid,s);
      csv_add("@uid",s);
      }
    if(csv_key_user)
      csv_add("@user",hisuser);
return;
    }
  // here: XML format
  wo__CLOSE
  if(wo__logaction)
    wo__action(hisver==1? 1: 2);
  switch(wo__format) {  // depending on output format
  case 11:  // native XML
    write_str("\t<way id=\""); write_sint64(id);
    wo__author(hisver,histime,hiscset,hisuid,hisuser);
    wo__xmlclosetag= "\t</way>" NL;  // preset close tag
    break;
  case 12:  // pbf2osm XML
    write_str("\t<way id=\""); write_sint64(id);
    wo__author(hisver,histime,hiscset,hisuid,hisuser);
    wo__xmlclosetag= "\t</way>" NL;  // preset close tag
    break;
  case 13:  // Osmosis XML
  case 14:  // Osmium XML
    write_str("  <way id=\""); write_sint64(id);
    wo__author(hisver,histime,hiscset,hisuid,hisuser);
    wo__xmlclosetag= "  </way>" NL;  // preset close tag
    break;
    }  // end   depending on output format
  wo__xmlshorttag= true;  // (default)
  }  // end   wo_way()

static inline void wo_way_close() {
  // complete writing an OSM way;
  if(wo__format<0)
    pw_way_close();
  else if(wo__format==21)
    csv_write();
  }  // end   wo_way_close()

static inline void wo_relation(int64_t id,
    int32_t hisver,int64_t histime,int64_t hiscset,
    uint32_t hisuid,const char* hisuser) {
  // write osm relation body;
  // id: id of this object;
  // hisver: version; 0: no author information is to be written
  //                     (necessary if o5m format);
  // histime: time (seconds since 1970)
  // hiscset: changeset
  // hisuid: uid
  // hisuser: user name
  if(wo__format==0) {  // o5m
    o5_write();  // write last object - if any
    o5_type(2);
    o5_byte(0x12);  // data set id for relation
    o5_svar64(id-o5_id); o5_id= id;
    wo__author(hisver,histime,hiscset,hisuid,hisuser);
    o5_markref(0);
return;
    }  // end   o5m
  if(wo__format<0) {  // PBF
    pw_relation(id,hisver,histime,hiscset,hisuid,hisuser);
return;
    }
  if(wo__format==21) {  // csv
    char s[20];

    if(csv_key_otype)
      csv_add("@otype","2");
    if(csv_key_oname)
      csv_add("@oname",ONAME(2));
    if(csv_key_id) {
      int64toa(id,s);
      csv_add("@id",s);
      }
    if(csv_key_uid) {
      uint32toa(hisuid,s);
      csv_add("@uid",s);
      }
    if(csv_key_user)
      csv_add("@user",hisuser);
return;
    }
  // here: XML format
  wo__CLOSE
  if(wo__logaction)
    wo__action(hisver==1? 1: 2);
  switch(wo__format) {  // depending on output format
  case 11:  // native XML
    write_str("\t<relation id=\""); write_sint64(id);
    wo__author(hisver,histime,hiscset,hisuid,hisuser);
    wo__xmlclosetag= "\t</relation>" NL;  // preset close tag
    break;
  case 12:  // pbf2osm XML
    write_str("\t<relation id=\""); write_sint64(id);
    wo__author(hisver,histime,hiscset,hisuid,hisuser);
    wo__xmlclosetag= "\t</relation>" NL;  // preset close tag
    break;
  case 13:  // Osmosis XML
  case 14:  // Osmium XML
    write_str("  <relation id=\""); write_sint64(id);
    wo__author(hisver,histime,hiscset,hisuid,hisuser);
    wo__xmlclosetag= "  </relation>" NL;  // preset close tag
    break;
    }  // end   depending on output format
  wo__xmlshorttag= true;  // (default)
  }  // end   wo_relation()

static inline void wo_relation_close() {
  // complete writing an OSM relation;
  if(wo__format<0)
    pw_relation_close();
  else if(wo__format==21)
    csv_write();
  }  // end   wo_relation_close()

static void wo_delete(int otype,int64_t id,
    int32_t hisver,int64_t histime,int64_t hiscset,
    uint32_t hisuid,const char* hisuser) {
  // write osm delete request;
  // this is possible for o5m format only;
  // for any other output format, this procedure does nothing;
  // otype: 0: node; 1: way; 2: relation;
  // id: id of this object;
  // hisver: version; 0: no author informaton is to be written
  //                     (necessary if o5m format);
  // histime: time (seconds since 1970)
  // hiscset: changeset
  // hisuid: uid
  // hisuser: user name
  if(otype<0 || otype>2 || wo__format<0)
return;
  if(wo__format==0) {  // o5m (.o5c)
    o5_write();  // write last object - if any
    o5_type(otype);
    o5_byte(0x10+otype);  // data set id
    o5_svar64(id-o5_id); o5_id= id;
    wo__author(hisver,histime,hiscset,hisuid,hisuser);
    }  // end   o5m (.o5c)
  else {  // .osm (.osc)
    wo__CLOSE
    if(wo__logaction)
      wo__action(3);
    if(wo__format>=13) write_str("  <"); else write_str("\t<");
    write_str(ONAME(otype));
    write_str(" id=\""); write_sint64(id);
    if(global_fakelonlat)
      write_str("\" lat=\"0\" lon=\"0");
    wo__author(hisver,histime,hiscset,hisuid,hisuser);
    wo__xmlclosetag= "\"/>" NL;  // preset close tag
    wo__xmlshorttag= false;  // (default)
    wo__CLOSE  // write close tag
    }  // end   .osm (.osc)
  }  // end   wo_delete()

static inline void wo_noderef(int64_t noderef) {
  // write osm object node reference;
  if(wo__format==0) {  // o5m
    o5_svar64(noderef-o5_ref[0]); o5_ref[0]= noderef;
    o5_markref(1);
return;
    }  // end   o5m
  if(wo__format<0) {  // PBF
    pw_way_ref(noderef);
return;
    }
  if(wo__format==21)  // csv
return;
  // here: XML format
  wo__CONTINUE
  switch(wo__format) {  // depending on output format
  case 11:  // native XML
  case 12:  // pbf2osm XML
    write_str("\t\t<nd ref=\""); write_sint64(noderef);
    write_str("\"/>" NL);
    break;
  case 13:  // Osmosis XML
  case 14:  // Osmium XML
    write_str("    <nd ref=\""); write_sint64(noderef);
    write_str("\"/>" NL);
    break;
    }  // end   depending on output format
  }  // end   wo_noderef()

static inline void wo_ref(int64_t refid,int reftype,
    const char* refrole) {
  // write osm object reference;
  if(wo__format==0) {  // o5m
    char o5typerole[4000];

    o5_svar64(refid-o5_ref[reftype]); o5_ref[reftype]= refid;
    o5typerole[0]= reftype+'0';
    strmcpy(o5typerole+1,refrole,sizeof(o5typerole)-1);
    stw_write(o5typerole,NULL);
    o5_markref(1);
return;
    }  // end   o5m
  if(wo__format<0) {  // PBF
    pw_relation_ref(refid,reftype,refrole);
return;
    }
  if(wo__format==21)  // csv
return;
  // here: XML format
  wo__CONTINUE
  switch(wo__format) {  // depending on output format
  case 11:  // native XML
  case 12:  // pbf2osm XML
    if(reftype==0)
      write_str("\t\t<member type=\"node\" ref=\"");
    else if(reftype==1)
      write_str("\t\t<member type=\"way\" ref=\"");
    else
      write_str("\t\t<member type=\"relation\" ref=\"");
    write_sint64(refid);
    write_str("\" role=\""); write_xmlstr(refrole);
    write_str("\"/>" NL);
    break;
  case 13:  // Osmosis XML
  case 14:  // Osmium XML
    if(reftype==0)
      write_str("    <member type=\"node\" ref=\"");
    else if(reftype==1)
      write_str("    <member type=\"way\" ref=\"");
    else
      write_str("    <member type=\"relation\" ref=\"");
    write_sint64(refid);
    write_str("\" role=\""); write_xmlmnstr(refrole);
    write_str("\"/>" NL);
    break;
    }  // end   depending on output format
  }  // end   wo_ref()

static inline void wo_node_keyval(const char* key,const char* val) {
  // write an OSM node object's keyval;
  if(wo__format==0) {  // o5m
    #if 0  // not used because strings would not be transparent anymore
    if(key[1]=='B' && strcmp(key,"bBox")==0 && strchr(val,',')!=0)
        // value is assumed to be dynamic, hence it should not be
        // stored in string list;
      // mark string pair as 'do-not-store';
      key= "\xff""bBox";  // 2012-10-14
      #endif
    stw_write(key,val);
return;
    }  // end   o5m
  if(wo__format<0) {  // PBF
    pw_node_keyval(key,val);
return;
    }
  if(wo__format==21) {  // csv
    csv_add(key,val);
return;
    }
  // here: XML format
  wo__CONTINUE
  switch(wo__format) {  // depending on output format
  case 11:  // native XML
    write_str("\t\t<tag k=\""); write_xmlstr(key);
    write_str("\" v=\"");
write_xmlstr(val);
    write_str("\"/>" NL);
    break;
  case 12:  // pbf2osm XML
    write_str("\t\t<tag k=\""); write_xmlstr(key);
    write_str("\" v=\""); write_xmlstr(val);
    write_str("\" />" NL);
    break;
  case 13:  // Osmosis XML
  case 14:  // Osmium XML
    write_str("    <tag k=\""); write_xmlmnstr(key);
    write_str("\" v=\""); write_xmlmnstr(val);
    write_str("\"/>" NL);
    break;
    }  // end   depending on output format
  }  // end   wo_node_keyval()

static inline void wo_wayrel_keyval(const char* key,const char* val) {
  // write an OSM way or relation object's keyval;
  if(wo__format==0) {  // o5m
    stw_write(key,val);
return;
    }  // end   o5m
  if(wo__format<0) {  // PBF
    pw_wayrel_keyval(key,val);
return;
    }
  if(wo__format==21) {  // csv
    csv_add(key,val);
return;
    }
  // here: XML format
  wo__CONTINUE
  switch(wo__format) {  // depending on output format
  case 11:  // native XML
    write_str("\t\t<tag k=\""); write_xmlstr(key);
    write_str("\" v=\"");
write_xmlstr(val);
    write_str("\"/>" NL);
    break;
  case 12:  // pbf2osm XML
    write_str("\t\t<tag k=\""); write_xmlstr(key);
    write_str("\" v=\""); write_xmlstr(val);
    write_str("\" />" NL);
    break;
  case 13:  // Osmosis XML
  case 14:  // Osmium XML
    write_str("    <tag k=\""); write_xmlmnstr(key);
    write_str("\" v=\""); write_xmlmnstr(val);
    write_str("\"/>" NL);
    break;
    }  // end   depending on output format
  }  // end   wo_wayrel_keyval()

//------------------------------------------------------------
// end   Module wo_   write osm module
//------------------------------------------------------------
