
//------------------------------------------------------------
// Module csv_   csv write module
//------------------------------------------------------------

// this module provides procedures for generating csv output;
// as usual, all identifiers of a module have the same prefix,
// in this case 'csv'; an underline will follow in case of a
// global accessible object, two underlines in case of objects
// which are not meant to be accessed from outside this module;
// the sections of private and public definitions are separated
// by a horizontal line: ----

#define csv__keyM 200  // max number of keys and vals
#define csv__keyMM 256  // max number of characters +1 in key or val
static char* csv__key= NULL;  // [csv__keyM][csv__keyMM]
static int csv__keyn= 0;  // number of keys
static char* csv__val= NULL;  // [csv__keyM][csv__keyMM]
static int csv__valn= 0;  // number of vals
// some booleans which tell us if certain keys are in column list;
// this is for program acceleration
static bool csv_key_otype= false, csv_key_oname= false,
  csv_key_id= false, csv_key_lon= false, csv_key_lat= false,
  csv_key_uid= false, csv_key_user= false;
static char csv__sep0= '\t';  // first character of global_csvseparator;
static char csv__rep0= ' ';  // replacement character for separator char

static void csv__end() {
  // clean-up csv processing;

  if(csv__key!=NULL)
    { free(csv__key); csv__key= NULL; }
  if(csv__val!=NULL)
    { free(csv__val); csv__val= NULL; }
  }  // end   csv__end()

//------------------------------------------------------------

static int csv_ini(const char* columns) {
  // initialize this module;
  // must be called before any other procedure is called;
  // may be called more than once; only the first call will
  // initialize this module, every other call will solely
  // overwrite the columns information  if !=NULL;
  // columns[]: space-separated list of keys who are to be
  //            used as column identifiers;
  //            ==NULL: if list has already been given, do not
  //                    change it; if not, set list to default;
  // return: 0: everything went ok;
  //         !=0: an error occurred;
  static bool firstrun= true;

  if(firstrun) {
    firstrun= false;

    csv__key= (char*)malloc(csv__keyM*csv__keyMM);
    csv__val= (char*)malloc(csv__keyM*csv__keyMM);
    if(csv__key==NULL || csv__val==NULL)
return 1;
    atexit(csv__end);
    }
  if(columns==NULL) {  // default columns shall be set
    if(csv__keyn==0) {  // until now no column has been set
      // set default columns
      strcpy(&csv__key[0*csv__keyMM],"@oname");
      csv_key_oname= true;
      strcpy(&csv__key[1*csv__keyMM],"@id");
      csv_key_id= true;
      strcpy(&csv__key[2*csv__keyMM],"name");
      csv__keyn= 3;
      }  // until now no column has been set
    }  // default columns shall be set
  else {  // new columns shall be set
    for(;;) {  // for each column name
      int len;
      char* tp;

      len= strcspn(columns," ");
      if(len==0)
    break;
      if(csv__keyn>=csv__keyM) {
        WARN("too many csv columns")
    break;
        }
      len++;
      if(len>csv__keyMM) len= csv__keyMM;  // limit key length
      tp= &csv__key[csv__keyn*csv__keyMM];
      strmcpy(tp,columns,len);
      csv__keyn++;
      if(strcmp(tp,"@otype")==0) csv_key_otype= true;
      else if(strcmp(tp,"@oname")==0) csv_key_oname= true;
      else if(strcmp(tp,"@id")==0) csv_key_id= true;
      else if(strcmp(tp,"@lon")==0) csv_key_lon= true;
      else if(strcmp(tp,"@lat")==0) csv_key_lat= true;
      else if(strcmp(tp,"@uid")==0) csv_key_uid= true;
      else if(strcmp(tp,"@user")==0) csv_key_user= true;
      columns+= len-1;
      if(columns[0]==' ') columns++;
      }  // for each column name
    }  // new columns shall be set
  // care about separator chars
  if(global_csvseparator[0]==0 || global_csvseparator[1]!=0) {
    csv__sep0= 0;
    csv__rep0= 0;
    }
  else {
    csv__sep0= global_csvseparator[0];
    if(csv__sep0==' ')
      csv__rep0= '_';
    else
      csv__rep0= ' ';
    }
  return 0;
  }  // end   csv_ini()

static void csv_add(const char* key,const char* val) {
  // test if the key's value shall be printed and do so if yes;
  int keyn;
  const char* kp;

  keyn= csv__keyn;
  kp= csv__key;
  while(keyn>0) {  // for all keys in column list
    if(strcmp(key,kp)==0) {  // key is in column list
      strmcpy(csv__val+(kp-csv__key),val,csv__keyMM);
        // store value
      csv__valn++;
  break;
      }  // key is in column list
    kp+= csv__keyMM;  // take next key in list
    keyn--;
    }  // for all keys in column list
  }  // end   csv_add()

static void csv_write() {
  // write a csv line - if csv data had been stored
  char* vp,*tp;
  int keyn;

  if(csv__valn==0)
return;
  vp= csv__val;
  keyn= csv__keyn;
  while(keyn>0) {  // for all keys in column list
    if(*vp!=0) {  // there is a value for this key
      tp= vp;
      do {
        if(*tp==csv__sep0 || *tp==NL[0] || *tp==NL[1])
              // character identical with separator or line feed
          write_char(csv__rep0);  // replace it by replacement char
        else
          write_char(*tp);
        tp++;
        } while(*tp!=0);
      *vp= 0;  // delete list entry
      }
    vp+= csv__keyMM;  // take next val in list
    keyn--;
    if(keyn>0)  // at least one column will follow
      write_str(global_csvseparator);
    }  // for all keys in column list
  write_str(NL);
  csv__valn= 0;
  }  // end   csv_write()

static void csv_headline() {
  // write a headline to csv output file
  char* kp;
  int keyn;

  if(!global_csvheadline)  // headline shall not be written
return;
  kp= csv__key;
  keyn= csv__keyn;
  while(keyn>0) {  // for all keys in column list
    csv_add(kp,kp);
    kp+= csv__keyMM;  // take next key in list
    keyn--;
    }  // for all keys in column list
  csv_write();
  }  // end   csv_headline()

//------------------------------------------------------------
// end   Module csv_   csv write module
//------------------------------------------------------------
