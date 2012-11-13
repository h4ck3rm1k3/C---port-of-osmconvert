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
