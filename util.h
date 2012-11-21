//#include "stdinc.hpp"
#include <vector>
int strzcmp(const char* s1,const char* s2);
int strycmp(const char* s1,const char* s2);
bool file_exists(const char* file_name);
char *stpcpy0(char *dest, const char *src);
int strzlcmp(const char* s1,const char* s2);

void PERRv1(const char * format,const char *str);
void WARNv(const char * format,const char *str);
typedef uint8_t byte;
typedef unsigned int uint;
void WARNv(const char * format,byte *str);
void WARNv(const char * format,byte &str);
void WARNv(const char * format,int64_t & i,const char *str2);
void WARNv(const char * format,const char *, int64_t & i,const char *str2);
void WARNv(const char * format,const char *, int64_t & i);



#define PERRv(A,B)  PERRv1(A,B);
//#define WARNv(A,..)  WARNv1(A,B);

//void perror(const char * msg);
void PERR1(const char * msg);
void PINFO(const char * msg);
//void PINFOv(const char * fmt, const char * args);
void PINFOv(const char * fmt, unsigned int arg);
void PINFOv(const char * fmt, unsigned int arg,unsigned int arg2);

//#define PINFOv(A, B)  PINFOv2(A, B);

#define PERR(A)  PERR1(A);

//typedef long long int int64;
//#define strMcpy(d,s) strmcpy((d),(s),sizeof(d))
char* strMcpy(const char* d,const char* s ) ;
time_t oo__strtimetosint64(const char *);
//long long int oo__strtosint64(const char *);
int64 oo__strtosint64(const char *);

char * strmcpy(char *dest, const char *src, size_t maxlen);

bool border_box(const char *);
bool border_file(const char *);
bool  oo_open(const char *);
int oo_ifn(); // number of open files
bool o5_ini(); // init
void posi_ini();
void csv_ini(const char *);
int oo_main();
void process_outputfile(const char * outputfile);
void border_active();
int hash_ini(int h_n,int h_w, int h_r);
void read_bufp();
void read_bufe();
void write_flush();
int hash_queryerror();
void write_error();
int64_t oo_sequenceid();
void oo_sequenceid(int64_t v); // set
int oo_sequencetype();
void oo_sequencetype(int i); // set
const char * ONAME(int);


typedef uint8_t byte;
typedef unsigned int uint;
#define isdig(x) isdigit((unsigned char)(x))


void  read_input();
uint32_t pbf_uint32(byte** pp);
uint64_t pbf_uint64(byte** pp);
int64_t  pbf_sint64(byte** pp);
int32_t pbf_sint32(byte** pp);
//void global_mergeversions();

void write_createtimestamp(uint64_t v,char* sp);

void read_switch(read_info_t* ri);
int read_rewind();

void str_switch(str_info_t* ri);
void str_reset();
void pb_input(bool v);

typedef std::vector<oo__if_t> oo__if_vt;

void oo__reset(oo__if_vt::iterator ifp);

int read_jump();
void read_close();

bool read_open(const char * filename);
//read_info_t* read_infop;
bool cwn_ini(const char * fname);
bool cww_ini(const char * fname);

char *stpmcpy(char *dest, const char *src, size_t maxlen);
void write_timestamp(uint64_t v);
void write_str(const char *);
//const char *  NL= "\n";
void cww_processing_set();
void cwn_processing();

void wo_flush();
void wo_reset();
void wo_end();
void wo_format(int wformat);
bool write_newfile(const char * name);
typedef std::vector<int64_t> refid_t;  // ids of referenced object
typedef std::vector<byte> reftype_t;  // ids of referenced object
typedef std::vector<int32_t> coord_t;  // ids of referenced object
typedef std::vector<coord_t> coord2_t;  // ids of referenced object
typedef std::vector<char> charv_t;  // ids of referenced object
typedef std::vector<charv_t> charv2_t;  // ids of referenced object

void posr_processing(int * maxrewind_posr,coord2_t & refxy);

bool write_testmode();

// for testing
char* uint32toa(uint32_t v,char* s);
