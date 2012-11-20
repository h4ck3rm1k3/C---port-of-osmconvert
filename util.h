//#include "stdinc.hpp"
int strzcmp(const char* s1,const char* s2);
int strycmp(const char* s1,const char* s2);
bool file_exists(const char* file_name);
char *stpcpy0(char *dest, const char *src);
int strzlcmp(const char* s1,const char* s2);

void PERRv1(const char * format,const char *str);
void WARNv(const char * format,const char *str);
void WARNv(const char * format,int64_t & i,const char *str2);
void WARNv(const char * format,const char *, int64_t & i,const char *str2);
void WARNv(const char * format,const char *, int64_t & i);



#define PERRv(A,B)  PERRv1(A,B);
//#define WARNv(A,..)  WARNv1(A,B);

//void perror(const char * msg);
void PERR1(const char * msg);
void PINFO(const char * msg);
//void PINFOv(const char * fmt, const char * args);
void PINFOv2(const char * fmt, unsigned int arg);

#define PINFOv(A, B)  PINFOv2(A, B);

#define PERR(A)  PERR1(A);

//typedef long long int int64;
//#define strMcpy(d,s) strmcpy((d),(s),sizeof(d))
char* strMcpy(const char* d,const char* s ) ;
time_t oo__strtimetosint64(const char *);
//long long int oo__strtosint64(const char *);
int64 oo__strtosint64(const char *);
char * strmcpy(const char *, const char * ,size_t size);

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
#include <vector>
typedef std::vector<oo__if_t> oo__if_vt;

void oo__reset(oo__if_vt::iterator ifp);

int read_jump();
