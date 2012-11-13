int strzcmp(const char* s1,const char* s2);
int strMcpy(const char* s1,const char* s2 );
int strycmp(const char* s1,const char* s2);
bool file_exists(const char* file_name);
char *stpcpy0(char *dest, const char *src);
int strzlcmp(const char* s1,const char* s2);

void PERRv1(const char * format,const char *str);

#define PERRv(A,B)  PERRv1(A,B);

void perror(const char * msg);
void PERR1(const char * msg);
void PINFO(const char * msg);
void PINFOv(const char * fmt, const char * args);
void PINFOv2(const char * fmt, unsigned int arg);

#define PINFOv(A, B)  PINFOv2(A, B);

#define PERR(A)  PERR1(A);


time_t oo__strtimetosint64(const char *);
long long int oo__strtosint64(const char *);
void strmcpy(const char *, const char * ,size_t size);

bool border_box(const char *);
bool border_file(const char *);
bool  oo_open(const char *);
void oo_ifn();
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
int oo_sequencetype();
const char * ONAME(int);
