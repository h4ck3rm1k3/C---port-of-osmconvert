// osmconvert 2012-10-14 18:10
#define VERSION "0.7G"
//
// compile this file:
// gcc osmconvert.c -lz -O3 -o osmconvert
//
// (c) 2011, 2012 Markus Weber, Nuernberg
// Richard Russo contributed the --all-to-nodes-bbox option
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Affero General Public License
// version 3 as published by the Free Software Foundation.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
// You should have received a copy of this license along
// with this program; if not, see http://www.gnu.org/licenses/.
//
// Other licenses are available on request; please ask the author.

// just a note (for tests): -b=8.748,53.052,8.749,53.053
#define MAXLOGLEVEL 2
const char* shorthelptext=
"\nosmconvert " VERSION "  Parameter Overview\n"
"(Please use  --help  to get more information.)\n"
"\n"
"<FILE>                    input file name\n"
"-                         read from standard input\n"
"-b=<x1>,<y1>,<x2>,<y2>    apply a border box\n"
"-B=<border_polygon>       apply a border polygon\n"
"--complete-ways           do not clip ways at the borders\n"
"--complex-ways            do not clip multipolygons at the borders\n"
"--all-to-nodes            convert ways and relations to nodes\n"
"--all-to-nodes-bbox       converts to nodes and adds bbox tags\n"
"--object-type-offset=<id> offset for ways/relations if --all-to-nodes\n"
"--max-objects=<n>         space for --all-to-nodes, 1 obj. = 16 bytes\n"
"--drop-broken-refs        delete references to excluded nodes\n"
"--drop-author             delete changeset and user information\n"
"--drop-version            same as before, but delete version as well\n"
"--drop-nodes              delete all nodes\n"
"--drop-ways               delete all ways\n"
"--drop-relations          delete all relations\n"
"--diff                    calculate differences between two files\n"
"--diff-contents           same as before, but compare whole contents\n"
"--emulate-osmosis         emulate Osmosis XML output format\n"
"--emulate-pbf2osm         emulate pbf2osm output format\n"
"--fake-author             set changeset to 1 and timestamp to 1970\n"
"--fake-version            set version number to 1\n"
"--fake-lonlat             set lon to 0 and lat to 0\n"
"-h                        display this parameter overview\n"
"--help                    display a more detailed help\n"
"--merge-versions          merge versions of each object in a file\n"
"--out-osm                 write output in .osm format (default)\n"
"--out-osc                 write output in .osc format (OSMChangefile)\n"
"--out-osh                 write output in .osh format (visible-tags)\n"
"--out-o5m                 write output in .o5m format (fast binary)\n"
"--out-o5c                 write output in .o5c format (bin. Changef.)\n"
"--out-pbf                 write output in .pbf format (bin. standard)\n"
"--out-csv                 write output in .csv format (plain table)\n"
"--out-none                no standard output (for testing purposes)\n"
"--csv=<column names>      choose columns for csv format\n"
"--csv-headline            start csv output with a headline\n"
"--csv-separator=<sep>     separator character(s) for csv format\n"
"--timestamp=<date_time>   add a timestamp to the data\n"
"--timestamp=NOW-<seconds> add a timestamp in seconds before now\n"
"--out-timestamp           output the file\'s timestamp, nothing else\n"
"--out-statistics          write statistics, nothing else\n"
"--statistics              write statistics to stderr\n"
"-o=<outfile>              reroute standard output to a file\n"
"-t=<tempfile>             define tempfile prefix\n"
"--parameter-file=<file>   param. in file, separated by empty lines\n"
"--verbose                 activate verbose mode\n";
const char* helptext=
"\nosmconvert " VERSION "\n"
"\n"
"This program reads different file formats of the OpenStreetMap\n"
"project and converts the data to the selected output file format.\n"
"These formats can be read:\n"
"  .osm  .osc  .osc.gz  .osh  .o5m  .o5c  .pbf\n"
"These formats can be written:\n"
"  .osm (default)  .osc  .osh  .o5m  .o5c  .pbf\n"
"\n"
"Names of input files must be specified as command line parameters.\n"
"Use - to read from standard input. You do not need to specify the\n"
"input formats, osmconvert will recognize them by itself.\n"
"The output format is .osm by default. If you want a different format,\n"
"please specify it using the appropriate command line parameter.\n"
"\n"
"-b=<x1>,<y1>,<x2>,<y2>\n"
"        If you want to limit the geographical region, you can define\n"
"        a bounding box. To do this, enter the southwestern and the\n"
"        northeastern corners of that area. For example:\n"
"        -b=-0.5,51,0.5,52\n"
"\n"
"-B=<border_polygon>\n"
"        Alternatively to a bounding box you can use a border polygon\n"
"        to limit the geographical region.\n"
"        The format of a border polygon file can be found in the OSM\n"
"        Wiki: http://wiki.openstreetmap.org/wiki/Osmosis/\n"
"              Polygon_Filter_File_Format\n"
"        You do not need to strictly follow the format description,\n"
"        you must ensure that every line of coordinates starts with\n"
"        blanks.\n"
"\n"
"--complete-ways\n"
"        If applying a border box or a border polygon, all nodes\n"
"        the borders are excluded; even then if they belong to a way\n"
"        which is not entirely excluded because it has some nodes\n"
"        inside the borders.\n"
"        This option will ensure that every way stays complete, even\n"
"        it it intersects the borders. This will result in slower\n"
"        processing, and the program will loose its ability to read\n"
"        from standard input. It is recommended to use .o5m format as\n"
"        input format to compensate most of the speed disadvantage.\n"
"\n"
"--complex-ways\n"
"        Same as before, but multipolygons will not be cut at the\n"
"        borders too.\n"
"\n"
"--all-to-nodes\n"
"        Some applications do not have the ability to process ways or\n"
"        relations, they just accept nodes as input. However, more and\n"
"        more complex object are mapped as ways or even relations in\n"
"        order to get all their details into the database.\n"
"        Apply this option if you want to convert ways and relations\n"
"        to nodes and thereby make them available to applications\n"
"        which can only deal with nodes.\n"
"        For each way a node is created. The way's id is increased by\n"
"        10^15 and taken as id for the new node. The node's longitude\n"
"        and latitude are set to the way's geographical center. Same\n"
"        applies to relations, however they get 2*10^15 as id offset.\n"
"\n"
"--all-to-nodes-bbox\n"
"        If applying the --all-to-nodes option as explained above, you\n"
"        may want to get the bounding box for the ways and relations\n"
"        that are converted into nodes.\n"
"        Apply this option if you want to add a tag \"bBox\" to the\n"
"        converted objects, the value will be min Longitude, min\n"
"        Latitude, max Longitude , max Latitude, for example: \n"
"          <tag k=\"bBox\" v=\"-0.5000,51.0000,0.5000,52.0000\"/>\n"
"\n"
"--object-type-offset=<id offset>\n"
"        If applying the --all-to-nodes option as explained above, you\n"
"        may adjust the id offset. For example:\n"
"          --object-type-offset=4000000000\n"
"        By appending \"+1\" to the offset, the program will create\n"
"        ids in a sequence with step 1. This might be useful if the\n"
"        there is a subsequently running application which cannot\n"
"        process large id numbers. Example:\n"
"          --object-type-offset=1900000000+1\n"
"\n"
"--drop-broken-refs\n"
"        Use this option if you need to delete references to nodes\n"
"        which have been excluded because lying outside the borders\n"
"        (mandatory for some applications, e.g. Map Composer, JOSM).\n"
"\n"
"--drop-author\n"
"        For most applications the author tags are not needed. If you\n"
"        specify this option, no author information will be written:\n"
"        no changeset, user or timestamp.\n"
"\n"
"--drop-version\n"
"        If you want to exclude not only the author information but\n"
"        also the version number, specify this option.\n"
"\n"
"--drop-nodes\n"
"--drop-ways\n"
"--drop-relations\n"
"        According to the combination of these parameters, no members\n"
"        of the referred section will be written.\n"
"\n"
"--diff\n"
"        Calculate difference between two files and create a new .osc\n"
"        or .o5c file.\n"
"        There must be TWO input files and borders cannot be applied.\n"
"        Both files must be sorted by object type and id. Created\n"
"        objects will appear in the output file as \"modified\", unless\n"
"        having version number 1.\n"
"\n"
"--diff-contents\n"
"        Similar to --diff, this option calculates differences between\n"
"        two OSM files. Here, to determine the differences complete\n"
"        OSM objects are consulted, not only the version numbers.\n"
"        Unfortunately, this option strictly requires both input files\n"
"        to have .o5m format.\n"
"\n"
"--emulate-osmosis\n"
"--emulate-pbf2osm\n"
"        In case of .osm output format, the program will try to use\n"
"        the same data syntax as Osmosis, resp. pbf2osm.\n"
"\n"
"--fake-author\n"
"        If you have dropped author information (--drop-author) that\n"
"        data will be lost, of course. Some programs however require\n"
"        author information on input although they do not need that\n"
"        data. For this purpose, you can fake the author information.\n"
"        osmconvert will write changeset 1, timestamp 1970.\n"
"\n"
"--fake-version\n"
"        Same as --fake-author, but - if .osm xml is used as output\n"
"        format - only the version number will be written (version 1).\n"
"        This is useful if you want to inspect the data with JOSM.\n"
"\n"
"--fake-lonlat\n"
"        Some programs depend on getting longitude/latitude values,\n"
"        even when the object in question shall be deleted. With this\n"
"        option you can have osmconvert to fake these values:\n"
"           ... lat=\"0\" lon=\"0\" ...\n"
"        Note that this is for XML files only (.osc and .osh).\n"
"\n"
"-h\n"
"        Display a short parameter overview.\n"
"\n"
"--help\n"
"        Display this help.\n"
"\n"
"--merge-versions\n"
"        Some .osc files contain different versions of one object.\n"
"        Use this option to accept such duplicates on input.\n"
"\n"
"--out-osm\n"
"        Data will be written in .osm format. This is the default\n"
"        output format.\n"
"\n"
"--out-osc\n"
"        The OSM Change format will be used for output. Please note\n"
"        that OSM objects which are to be deleted will be represented\n"
"        by their ids only.\n"
"\n"
"--out-osh\n"
"        For every OSM object, the appropriate \'visible\' tag will be\n"
"        added to meet \'full planet history\' specification.\n"
"\n"
"--out-o5m\n"
"        The .o5m format will be used. This format has the same\n"
"        structure as the conventional .osm format, but the data are\n"
"        stored as binary numbers and are therefore much more compact\n"
"        than in .osm format. No packing is used, so you can pack .o5m\n"
"        files using every file packer you want, e.g. lzo, bz2, etc.\n"
"\n"
"--out-o5c\n"
"        This is the change file format of .o5m data format. All\n"
"        <delete> tags will not be performed as delete actions but\n"
"        converted into .o5c data format.\n"
"\n"
"--out-pbf\n"
"        For output, PBF format will be used.\n"
"\n"
"--out-csv\n"
"        A character separated list will be written to output.\n"
"        The default separator is Tab, the default columns are:\n"
"        type, id, name. You can change both by using the options\n"
"        --csv-separator= and --csv=\n"
"\n"
"--csv-headline\n"
"        Choose this option to print a headline to csv output.\n"
"\n"
"--csv-separator=<sep>\n"
"        You may change the default separator (Tab) to a different\n"
"        character or character sequence. For example:\n"
"        --csv-separator=\"; \"\n"
"\n"
"--csv=<columns>\n"
"        If you want to have certain columns in your csv list, please \n"
"        specify their names as shown in this example:\n"
"        --csv=\"@id name ref description\"\n"
"        There are a few special column names for header data:\n"
"        @otype (object type 0..2), @oname (object type name), @id\n"
"        @lon, @lat, @uid, @user\n"
"\n"
"--out-none\n"
"        This will be no standard output. This option is for testing\n"
"        purposes only.\n"
"\n"
"--timestamp=<date_and_time>\n"
"--timestamp=NOW<seconds_relative_to_now>\n"
"        If you want to set the OSM timestamp of your output file,\n"
"        supply it with this option. Date and time must be formatted\n"
"        according OSM date/time specifications. For example:\n"
"        --timestamp=2011-01-31T23:59:30Z\n"
"        You also can supply a relative time in seconds, e.g. 24h ago:\n"
"        --timestamp=NOW-86400\n"
"\n"
"--out-timestamp\n"
"        With this option set, osmconvert prints just the time stamp\n"
"        of the input file, nothing else.\n"
"\n"
"--statistics\n"
"        This option activates a statistics counter. The program will\n"
"        print statistical data to stderr.\n"
"\n"
"--out-statistics\n"
"        Same as --statistics, but the statistical data will be\n"
"        written to standard output.\n"
"\n"
"-o=<outfile>\n"
"        Standard output will be rerouted to the specified file.\n"
"        If no output format has been specified, the program will\n"
"        rely  the file name extension.\n"
"\n"
"-t=<tempfile>\n"
"        If borders are to be applied or broken references to be\n"
"        eliminated, osmconvert creates and uses two temporary files.\n"
"        This parameter defines their name prefix. The default value\n"
"        is \"osmconvert_tempfile\".\n"
"\n"
"--parameter-file=FILE\n"
"        If you want to supply one ore more command line arguments\n"
"        by a parameter file, please use this option and specify the\n"
"        file name. Within the parameter file, parameters must be\n"
"        separated by empty lines. Line feeds inside a parameter will\n"
"        be converted to spaces.\n"
"        Lines starting with \"// \" will be treated as comments.\n"
"\n"
"-v\n"
"--verbose\n"
"        With activated \'verbose\' mode, some statistical data and\n"
"        diagnosis data will be displayed.\n"
"        If -v resp. --verbose is the first parameter in the line,\n"
"        osmconvert will display all input parameters.\n"
"\n"
"Examples\n"
"\n"
"./osmconvert europe.pbf --drop-author >europe.osm\n"
"./osmconvert europe.pbf |gzip >europe.osm.gz\n"
"bzcat europe.osm.bz2 |./osmconvert --out-pbf >europe.pbf\n"
"./osmconvert europe.pbf -B=ch.poly >switzerland.osm\n"
"./osmconvert switzerland.osm --out-o5m >switzerland.o5m\n"
"./osmconvert june_july.osc --out-o5c >june_july.o5c\n"
"./osmconvert june.o5m june_july.o5c.gz --out-o5m >july.o5m\n"
"./osmconvert sep.osm sep_oct.osc oct_nov.osc >nov.osm\n"
"./osmconvert northamerica.osm southamerica.osm >americas.osm\n"
"\n"
"Tuning\n"
"\n"
"To speed-up the process, the program uses some main memory for a\n"
"hash table. By default, it uses 480 MB for storing a flag for every\n"
"possible node, 90 for the way flags, and 30 relation flags.\n"
"Every byte holds the flags for 8 ID numbers, i.e., in 480 MB the\n"
"program can store 3840 million flags. As there are less than 1900\n"
"million IDs for nodes at present (July 2012), 240 MB would suffice.\n"
"So, for example, you can decrease the hash sizes to e.g. 240, 30 and\n"
"2 MB using this option:\n"
"\n"
"  --hash-memory=240-30-2\n"
"\n"
"But keep in mind that the OSM database is continuously expanding. For\n"
"this reason the program-own default value is higher than shown in the\n"
"example, and it may be appropriate to increase it in the future.\n"
"If you do not want to bother with the details, you can enter the\n"
"amount of memory as a sum, and the program will divide it by itself.\n"
"For example:\n"
"\n"
"  --hash-memory=1000\n"
"\n"
"These 1000 MiB will be split in three parts: 800 for nodes, 150 for\n"
"ways, and 50 for relations.\n"
"\n"
"Because we are taking hashes, it is not necessary to provide all the\n"
"suggested memory; the program will operate with less hash memory too.\n"
"But, in this case, the border filter will be less effective, i.e.,\n"
"some ways and some relations will be left in the output file although\n"
"they should have been excluded.\n"
"The maximum value the program accepts for the hash size is 4000 MiB;\n"
"If you exceed the maximum amount of memory available on your system,\n"
"the program will try to reduce this amount and display a warning\n"
"message.\n"
"\n"
"There is another temporary memory space which is used only for the\n"
"conversion of ways and relations to nodes (option --all-to-nodes).\n"
"This space is sufficient for up to 25 Mio. OSM objects, 400 MB of\n"
"main memory are needed for this purpose, 800 MB if extended option\n"
"--all-to-nodes-bbox has been invoked. If this is not sufficient or\n"
"if you want to save memory, you can configure the maximum number of\n"
"OSM objects by yourself. For example:\n"
"\n"
"  --max-objects=35000000\n"
"\n"
"The number of references per object is limited to 100,000. This will\n"
"be sufficient for all OSM files. If you are going to create your own\n"
"OSM files by converting shapefiles or other files to OSM format, this\n"
"might result in way objects with more than 100,000 nodes. For this\n"
"reason you will need to increase the maximum accordingly. Example:\n"
"\n"
"  --max-refs=400000\n"
"\n"
"Limitations\n"
"\n"
"When extracting a geographical region (using -b or -B), the input\n"
"file must contain the objects ordered by their type: first, all\n"
"nodes, next, all ways, followed by all relations. Within each of\n"
"these sections, the objects section must be sorted by their id in\n"
"ascending order.\n"
"\n"
"Usual .osm, .osc, .o5m, o5c and .pbf files adhere to this condition.\n"
"This means that you do not have to worry about this limitation.\n"
"osmconvert will display an error message if this sequence is broken.\n"
"\n"
"If a polygon file for borders is supplied, the maximum number of\n"
"polygon points is about 40,000.\n"
"\n"
"This program is for experimental use. Expect malfunctions and data\n"
"loss. Do not use the program in productive or commercial systems.\n"
"\n"
"There is NO WARRANTY, to the extent permitted by law.\n"
"Please send any bug reports to markus.weber@gmx.com\n\n";

#define __STDC_CONSTANT_MACROS 1
#include <string>
using namespace std;

#include "stdinc.hpp"
#include "obj_relref.hpp"
#include "osm_hash.hpp"
#include "osm_border.h"
#include "oo.h"
#include "read.hpp"
#include "str.hpp"
#include <vector>
#include "process.hpp"
#include "util.h"
#include "process.hpp"
#include "read.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#define _FILE_OFFSET_BITS 64
#include <zlib.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

//typedef enum {false= 0,true= 1} bool;
//int loglevel= 0;  // logging to stderr;
  // 0: no logging; 1: small logging; 2: normal logging;
  // 3: extended logging;
#define DP(f) fprintf(stderr,"Debug: " #f "\n");
#define DPv(f,...) fprintf(stderr,"Debug: " #f "\n",__VA_ARGS__);
#define DPM(f,p,m) { byte* pp; int i,mm; static int msgn= 3; \
  if(--msgn>=0) { fprintf(stderr,"Debug memory: " #f); \
  pp= (byte*)(p); mm= (m); if(pp==NULL) fprintf(stderr,"\n  (null)"); \
  else for(i= 0; i<mm; i++) { \
  if((i%16)==0) fprintf(stderr,"\n "); \
  fprintf(stderr," %02x",*pp++); } \
  fprintf(stderr,"\n"); } }
#if __WIN32__
  #define NL "\r\n"  // use CR/LF as new-line sequence
  #define off_t off64_t
  #define lseek lseek64
  z_off64_t gzseek64(gzFile,z_off64_t,int);
  #define gzseek gzseek64
#else
  #define NL "\n"  // use LF as new-line sequence
  #define O_BINARY 0
#endif


static void assistant_end();
#include "util.h"
static bool assistant(int* argcp,char*** argvp) {
  // interactively guide the user through basic functions;
  // argcp==NULL AND argvp==NULL: to confirm that the calculation
  //                              has been terminated correctly;
  // argcp==NULL AND argvp!=NULL:
  // display 'bye message', do nothing else (third call);
  // usually, this procedure must be called twice: first, before
  // parsing the command line arguments, and second, after
  // the regular processing has been done without any error;
  // the third call will be done by atexit();
  // return: user wants to terminate the program;
  #define langM 2
  static int lang= 0;
  static const char* talk_lang1[langM]= { "", "de_" };
  static const char* talk_lang2[langM]= { "", "German_" };
  static const char* talk_section[langM]= {
    "-----------------------------------------------------------------\n"
    };
  static const char* talk_intro[langM]= {
    "\n"
    "osmconvert " VERSION "\n"
    "\n"
    "Converts .osm, .o5m, .pbf, .osc, .osh files, applies changes\n"
    "of .osc, .o5c, .osh files and sets limiting borders.\n"
    "Use command line option -h to get a parameter overview,\n"
    "or --help to get detailed help.\n"
    "\n"
    "If you are familiar with the command line, press <Return>.\n"
    "\n"
    "If you do not know how to operate the command line, please\n"
    "enter \"a\" (press key E and hit <Return>).\n"
    ,
    "\n"
    "osmconvert " VERSION "\n"
    "\n"
    "Konvertiert .osm-, .o5m-, .pbf-, .osc- und .osh-Dateien,\n"
    "spielt Updates von .osc-, .o5c- und .osh-Dateien ein und\n"
    "setzt geografische Grenzen.\n"
    "Die Kommandozeilenoption -h zeigt eine Parameteruebersicht,\n"
    "--help bringt eine detaillierte Hilfe (in Englisch).\n"
    "\n"
    "Wenn Sie mit der Kommandozeile vertraut sind, druecken Sie\n"
    "bitte <Return>.\n"
    "\n"
    "Falls Sie sich mit der Kommandozeile nicht auskennen, druecken\n"
    "Sie bitte \"a\" (Taste A und dann die Eingabetaste).\n"
    };
  static const char* talk_hello[langM]= {
    "Hi, I am osmconBert - just call me Bert.\n"
    "I will guide you through the basic functions of osmconvert.\n"
    "\n"
    "At first, please ensure to have the \"osmconvert\" file\n"
    "(resp. \"osmconvert.exe\" file if Windows) located in the\n"
    "same directory in which all your OSM data is stored.\n"
    "\n"
    "You may exit this program whenever you like. Just hold\n"
    "the <Ctrl> key and press the key C.\n"
    "\n"
    ,
    "Hallo, ich bin osmconBert - nennen Sie mich einfach Bert.\n"
    "Ich werde Sie durch die Standardfunktionen von osmconvert leiten.\n"
    "\n"
    "Bitte stellen Sie zuerst sicher, dass sich die Programmdatei\n"
    "\"osmconvert\" (bzw. \"osmconvert.exe\" im Fall von Windows) im\n"
    "gleichen Verzeichnis befindet wie Ihre OSM-Dateien.\n"
    "\n"
    "Sie koennen das Programm jederzeit beenden. Halten Sie dazu die\n"
    "<Strg>-Taste gedrueckt und druecken die Taste C.\n"
    "\n"
    };
  static const char* talk_input_file[langM]= {
    "Please please tell me the name of the file you want to process:\n"
    ,
    "Bitte nennen Sie mir den Namen der Datei, die verarbeitet werden soll:\n"
    };
  static const char* talk_not_found[langM]= {
    "Sorry, I cannot find a file with this name in the current directory.\n"
    "\n"
    ,
    "Sorry, ich kann diese Datei im aktuellen Verzeichnis nicht finden.\n"
    "\n"
    };
  static const char* talk_input_file_suffix[langM]= {
    "Sorry, the file must have \".osm\", \".o5m\" or \".pbf\" as suffix.\n"
    "\n"
    ,
    "Sorry, die Datei muss \".osm\", \".o5m\" oder \".pbf\" als Endung haben.\n"
    "\n"
    };
  static const char* talk_thanks[langM]= {
    "Thanks!\n"
    ,
    "Danke!\n"
    };
  static const char* talk_function[langM]= {
    "What may I do with this file?\n"
    "\n"
    "1  convert it to a different file format\n"
    "2  use an OSM Changefile to update this file\n"
    "3  use a border box to limit the geographical region\n"
    "4  use a border polygon file to limit the geographical region\n"
    "5  minimize file size by deleting author information\n"
    "6  display statistics of the file\n"
    "\n"
    "Please enter the number of one or more functions you choose:\n"
    ,
    "Was soll ich mit dieser Datei tun?\n"
    "\n"
    "1  in ein anderes Format umwandeln\n"
    "2  sie per OSM-Change-Datei aktualisieren\n"
    "3  per Laengen- und Breitengrad einen Bereich ausschneiden\n"
    "4  mit einer Polygon-Datei einen Bereich ausschneiden\n"
    "5  Autorinformationen loeschen und damit Dateigroesse minimieren\n"
    "6  statistische Daten zu dieser Datei anzeigen\n"
    "\n"
    "Bitte waehlen Sie die Nummer(n) von einer oder mehreren Funktionen:\n"
    };
  static const char* talk_all_right[langM]= {
    "All right.\n"
    ,
    "Geht in Ordnung.\n"
    };
  static const char* talk_cannot_understand[langM]= {
    "Sorry, I could not understand.\n"
    "\n"
    ,
    "Sorry, das habe ich nicht verstanden.\n"
    "\n"
    };
  static const char* talk_two_borders[langM]= {
    "Please do not choose both, border box and border polygon.\n"
    "\n"
    ,
    "Bitte nicht beide Arten des Ausschneidens gleichzeitig waehlen.\n"
    "\n"
    };
  static const char* talk_changefile[langM]= {
    "Please tell me the name of the OSM Changefile:\n"
    ,
    "Bitte nennen Sie mir den Namen der OSM-Change-Datei:\n"
    };
  static const char* talk_changefile_suffix[langM]= {
    "Sorry, the Changefile must have \".osc\" or \".o5c\" as suffix.\n"
    "\n"
    ,
    "Sorry, die Change-Datei muss \".osc\" oder \".o5c\" als Endung haben.\n"
    "\n"
    };
  static const char* talk_polygon_file[langM]= {
    "Please tell me the name of the polygon file:\n"
    ,
    "Bitte nennen Sie mir den Namen der Polygon-Datei:\n"
    };
  static const char* talk_polygon_file_suffix[langM]= {
    "Sorry, the polygon file must have \".poly\" as suffix.\n"
    "\n"
    ,
    "Sorry, die Polygon-Datei muss \".poly\" als Endung haben.\n"
    "\n"
    };
  static const char* talk_coordinates[langM]= {
    "We need the coordinates of the border box.\n"
    "The unit is degree, just enter each number, e.g.: -35.75\n"
    ,
    "Wir brauchen die Bereichs-Koordinaten in Grad,\n"
    "aber jeweils ohne Einheitenbezeichnung, also z.B.: 7,75\n"
    };
  static const char* talk_minlon[langM]= {
    "Please tell me the minimum longitude:\n"
    ,
    "Bitte nennen Sie mir den Minimum-Laengengrad:\n"
    };
  static const char* talk_maxlon[langM]= {
    "Please tell me the maximum longitude:\n"
    ,
    "Bitte nennen Sie mir den Maximum-Laengengrad:\n"
    };
  static const char* talk_minlat[langM]= {
    "Please tell me the minimum latitude:\n"
    ,
    "Bitte nennen Sie mir den Minimum-Breitengrad:\n"
    };
  static const char* talk_maxlat[langM]= {
    "Please tell me the maximum latitude:\n"
    ,
    "Bitte nennen Sie mir den Maximum-Breitengrad:\n"
    };
  static const char* talk_output_format[langM]= {
    "Please choose the output file format:\n"
    "\n"
    "1 .osm (standard XML format - results in very large files)\n"
    "2 .o5m (binary format - allows fast)\n"
    "3 .pbf (standard binary format - results in small files)\n"
    "\n"
    "Enter 1, 2 or 3:\n"
    ,
    "Bitte waehlen Sie das Format der Ausgabe-Datei:\n"
    "\n"
    "1 .osm (Standard-XML-Format - ergibt sehr grosse Dateien)\n"
    "2 .o5m (binaeres Format - recht schnell)\n"
    "3 .pbf (binaeres Standard-Format - ergibt kleine Dateien)\n"
    "\n"
    "1, 2 oder 3 eingeben:\n"
    };
  static const char* talk_working[langM]= {
    "Now, please hang on - I am working for you.\n"
    "If the input file is very large, this will take several minutes.\n"
    "\n"
    "If you want to get acquainted with the much more powerful\n"
    "command line, this would have been your command:\n"
    "\n"
    ,
    "Einen Moment bitte - ich arbeite fuer Sie.\n"
    "Falls die Eingabe-Datei sehr gross ist, dauert das einige Minuten.\n"
    "\n"
    "Fall Sie sich mit der viel leistungsfaehigeren Kommandozeilen-\n"
    "eingabe vertraut machen wollen, das waere Ihr Kommando gewesen:\n"
    "\n"
    };
  static const char* talk_finished[langM]= {
    "Finished! Calculation time: "
    ,
    "Fertig! Berechnungsdauer: "
    };
  static const char* talk_finished_file[langM]= {
    "I just completed your new file with this name:\n"
    ,
    "Soeben habe ich Ihre neue Datei mit diesem Namen fertiggestellt:\n"
    };
  static const char* talk_error[langM]= {
    "I am sorry, an error has occurred (see above).\n"
    ,
    "Es tut mir Leid, es ist ein Fehler aufgetreten (siehe oben).\n"
    };
  static const char* talk_bye[langM]= {
    "\n"
    "Thanks for visiting me. Bye!\n"
    "Yours, Bert\n"
    "(To close this window, please press <Return>.)\n"
    ,
    "\n"
    "Danke fuer Ihren Besuch. Tschues!\n"
    "Schoene Gruesse - Bert\n"
    "(Zum Schließen dieses Fensters bitte die Eingabetaste druecken.)\n"
    };
  #define DD(s) fprintf(stderr,"%s",(s[lang]));  // display text
  #define DI(s) fgets(s,sizeof(s),stdin); \
    if(strchr(s,'\r')!=NULL) *strchr(s,'\r')= 0; \
    if(strchr(s,'\n')!=NULL) *strchr(s,'\n')= 0;  // get user's response
  bool
    function_convert= false,
    function_update= false,
    function_border_box= false,
    function_border_polygon= false,
    function_drop_author= false,
    function_statistics= false;
  static bool function_only_statistics= false;
  static time_t start_time;
  bool verbose;
  char s[500];  // temporary string for several purposes
  char* sp;
  static char input_file[500];
  bool file_type_osm,file_type_osc,file_type_o5m,file_type_o5c,
    file_type_pbf;
  static char changefile[500];
  char polygon_file[500];
  char minlon[30],maxlon[30],minlat[30],maxlat[30];
  static char output_file[550]= "";  // the first three characters
    // are reserved for the commandline option "-o="
  int i;

  // display 'bye message' - if requested
  if(argcp==NULL) {
    static bool no_error= false;

    if(argvp==NULL)
      no_error= true;
    else {
      if(output_file[0]!=0) {
        DD(talk_section)
        if(no_error) {
          DD(talk_finished)
          fprintf(stderr,"%lds.\n",
            (int64_t)(time(NULL)-start_time));
          DD(talk_finished_file)
          fprintf(stderr,"  %s",output_file+3);
          }
        else
          DD(talk_error)
        DD(talk_bye)
        DI(s)
        }
      else if(function_only_statistics) {
        DD(talk_section)
        if(no_error) {
          DD(talk_finished)
          fprintf(stderr,"%lds.\n",
            (int64_t)(time(NULL)-start_time));
          }
        else
          DD(talk_error)
        DD(talk_bye)
        DI(s)
        }
      }
return false;
    }

  // initialization
  atexit(assistant_end);
  for(i= 1; i<langM; i++) {
    talk_section[i]= talk_section[0];
    // (this dialog text is the same for all languages)
    }
  verbose= false;

  /* get system language */ {
    const char* syslang;

    syslang= setlocale(LC_ALL,"");
    lang= langM;
    while(--lang>0)
      if(syslang!=NULL &&
          (strzcmp(syslang,talk_lang1[lang])==0 ||
          strzcmp(syslang,talk_lang2[lang])==0)) break;
    setlocale(LC_ALL,"C");  // switch back to C standard
    }

  // introduction
  DD(talk_intro)
  DI(s)
  sp= s;
  while(*sp==' ') sp++;  // dispose of leading spaces
  if((*sp!='a' && *sp!='A') || sp[1]!=0)
return true;
  verbose= isupper(*(unsigned char*)sp);

  // choose input file
  DD(talk_section)
  DD(talk_hello)
  for(;;) {
    DD(talk_input_file)
    DI(input_file)
    file_type_osm= strycmp(input_file,".osm")==0;
    file_type_osc= strycmp(input_file,".osc")==0;
    file_type_o5m= strycmp(input_file,".o5m")==0;
    file_type_o5c= strycmp(input_file,".o5c")==0;
    file_type_pbf= strycmp(input_file,".pbf")==0;
    if(!file_type_osm && !file_type_osc && !file_type_o5m &&
        !file_type_o5c && !file_type_pbf) {
      DD(talk_input_file_suffix)
  continue;
      }
    if(input_file[strcspn(input_file,"\"\', :;|&\\")]!=0 ||
        !file_exists(input_file)) {
      DD(talk_not_found)
  continue;
      }
    break;
    }
  DD(talk_thanks)

  // choose function
  DD(talk_section)
  for(;;) {
    function_convert= function_update= function_border_polygon=
      function_border_box= function_statistics= false;
    DD(talk_function)
    DI(s)
    i= 0;  // here: number of selected functions
    sp= s;
    while(*sp!=0) {
      if(*sp=='1')
        function_convert= true;
      else if(*sp=='2')
        function_update= true;
      else if(*sp=='3')
        function_border_box= true;
      else if(*sp=='4')
        function_border_polygon= true;
      else if(*sp=='5')
        function_drop_author= true;
      else if(*sp=='6')
        function_statistics= true;
      else if(*sp==' ' || *sp==',' || *sp==';') {
        sp++;
    continue;
        }
      else {  // syntax error
        i= 0;  // ignore previous input
    break;
        }
      i++; sp++;
      }
    if(function_border_box && function_border_polygon) {
      DD(talk_two_borders)
  continue;
      }
    if(i==0) {  // no function has been chosen OR syntax error
      DD(talk_cannot_understand)
  continue;
      }
    break;
    }
  function_only_statistics= function_statistics &&
    !function_convert && !function_update &&
    !function_border_polygon && !function_border_box;
  DD(talk_all_right)

  // choose OSM Changefile
  if(function_update) {
    DD(talk_section)
    for(;;) {
      DD(talk_changefile)
      DI(changefile)
      if(strycmp(changefile,".osc")!=0 &&
          strycmp(changefile,".o5c")!=0) {
        DD(talk_changefile_suffix)
    continue;
        }
      if(changefile[strcspn(changefile,"\"\' ,:;|&\\")]!=0 ||
          !file_exists(changefile)) {
        DD(talk_not_found)
    continue;
        }
      break;
      }
    DD(talk_thanks)
    }

  // choose polygon file
  if(function_border_polygon) {
    DD(talk_section)
    for(;;) {
      DD(talk_polygon_file)
      DI(polygon_file)
      if(strycmp(polygon_file,".poly")!=0) {
        DD(talk_polygon_file_suffix)
    continue;
        }
      if(polygon_file[strcspn(polygon_file,"\"\' ,:;|&\\")]!=0 ||
          !file_exists(polygon_file)) {
        DD(talk_not_found)
    continue;
        }
      break;
      }
    DD(talk_thanks)
    }

  // choose coordinates
  if(function_border_box) {
    DD(talk_section)
    for(;;) {
      #define D(s) DI(s) \
        while(strchr(s,',')!=NULL) *strchr(s,',')= '.'; \
        if(s[0]==0 || s[strspn(s,"0123456789.-")]!=0) { \
          DD(talk_cannot_understand) continue; }
      DD(talk_coordinates)
      DD(talk_minlon)
      D(minlon)
      DD(talk_minlat)
      D(minlat)
      DD(talk_maxlon)
      D(maxlon)
      DD(talk_maxlat)
      D(maxlat)
      #undef D
      break;
      }
    DD(talk_thanks)
    }

  // choose file type
  if(function_convert) {
    file_type_osm= file_type_osc= file_type_o5m=
    file_type_o5c= file_type_pbf= false;
    DD(talk_section)
    for(;;) {
      DD(talk_output_format)
      DI(s)
      sp= s; while(*sp==' ') sp++;  // ignore spaces
      if(*sp=='1')
        file_type_osm= true;
      else if(*sp=='2')
        file_type_o5m= true;
      else if(*sp=='3')
        file_type_pbf= true;
      else {
        DD(talk_cannot_understand)
    continue;
        }
      break;
      }
    DD(talk_thanks)
    }

  // assemble output file name
  DD(talk_section)
  if(!function_only_statistics) {
    if(file_type_osm) strcpy(s,".osm");
    if(file_type_osc) strcpy(s,".osc");
    if(file_type_o5m) strcpy(s,".o5m");
    if(file_type_o5c) strcpy(s,".o5c");
    if(file_type_pbf) strcpy(s,".pbf");
    sp= stpcpy(output_file,"-o=");
    strcpy(sp,input_file);
    sp= strrchr(sp,'.');
    if(sp==NULL) sp= strchr(output_file,0);
    i= 1;
    do
      sprintf(sp,"_%02i%s",i,s);
      while(++i<9999 && file_exists(output_file+3));
    }

  /* create new commandline arguments */ {
    int argc;
    static char* argv[10];
    static char border[550];

    argc= 0;
    argv[argc++]= (*argvp)[0];  // save program name
    if(verbose)
      argv[argc++]= (char*)"-v";  // activate verbose mode
    argv[argc++]= input_file;
    if(function_update)
      argv[argc++]= changefile;
    if(function_border_polygon) {
      sp= stpcpy(border,"-B=");
      strcpy(sp,polygon_file);
      argv[argc++]= border;
      }
    else if(function_border_box) {
      sprintf(border,"-b=%s,%s,%s,%s",minlon,minlat,maxlon,maxlat);
      argv[argc++]= border;
      }
    if(function_drop_author)
      argv[argc++]= (char*)"--drop-author";
    if(function_only_statistics)
      argv[argc++]= (char*)"--out-statistics";
    else if(function_statistics)
        argv[argc++]= (char*)"--statistics";
    if(output_file[0]!=0) {
      if(file_type_osm) argv[argc++]= (char*)"--out-osm";
      else if(file_type_osc) argv[argc++]= (char*)"--out-osc";
      else if(file_type_o5m) argv[argc++]= (char*)"--out-o5m";
      else if(file_type_o5c) argv[argc++]= (char*)"--out-o5c";
      else if(file_type_pbf) argv[argc++]= (char*)"--out-pbf";
      argv[argc++]= output_file;
      }
    // return commandline variables
    *argcp= argc;
    *argvp= argv;

    // display the virtual command line
    DD(talk_working)
    fprintf(stderr,"osmconvert");
    i= 0;
    while(++i<argc)
      fprintf(stderr," %s",argv[i]);
    fprintf(stderr,"\n");
    DD(talk_section)
    }

  start_time= time(NULL);
  #undef langM
  #undef DP
  #undef DI
  return false;
  }  // assistant()

static void assistant_end() {
  // will be called via atexit()
  assistant(NULL,(char***)assistant_end);
  }  // assistant_end()



#if !__WIN32__
void sigcatcher(int sig) {
  fprintf(stderr,"osmconvert: Output has been terminated.\n");
  exit(1);
  }  // end   sigchatcher()
#endif


int main(int argc,char** argv) {
  Process aProcess;
  Process * process=&aProcess; // pointer to the current process, we can push and pop them

  Read aRead;
  Read * read=&aRead; // pointer to the current process, we can push and pop them

  // main program;
  // for the meaning of the calling line parameters please look at the
  // contents of helptext[]; 
 bool usesstdin;
  static char outputfilename[400]= "";  // standard output file name
    // =="": standard output 'stdout'
  int h_n,h_w,h_r;  // user-suggested hash size in MiB, for
    // hash tables of nodes, ways, and relations;
  int r,l;
  const char* a;  // command line argument
  static FILE* parafile= NULL;
  static char* aa= NULL;  // buffer for parameter file line
  char* ap;  // pointer in aa[]
  int aamax;  // maximum length of string to read
  #define main__aaM 1000000

  #if !__WIN32__
  /* care about signal handler */ {
    static struct sigaction siga;

    siga.sa_handler= sigcatcher;
    sigemptyset(&siga.sa_mask);
    siga.sa_flags= 0;
    sigaction(SIGPIPE,&siga,NULL);
    }
  #endif

  // initializations
  usesstdin= false;
  h_n= h_w= h_r= 0;
  #if __WIN32__
    setmode(fileno(stdout),O_BINARY);
    setmode(fileno(stdin),O_BINARY);
  #endif

  // read command line parameters
  if(argc<=1) {  // no command line parameters given
    if(assistant(&argc,&argv))  // call interactive program guide
return 0;
    }
  while(parafile!=NULL || argc>0) {
      // for every parameter in command line
    if(parafile!=NULL) do {
        // there are parameters waiting in a parameter file
      ap= aa;
      for(;;) {
        aamax= main__aaM-1-(ap-aa);
        if(fgets(ap,aamax,parafile)==NULL) {
          if(ap>aa) {
            if(ap>aa && ap[-1]==' ')
              *--ap= 0;  // cut one trailing space
      break;
            }
          goto parafileend;
          }
        if(strzcmp(ap,"// ")==0)
      continue;
        if(ap>aa && (*ap=='\r' || *ap=='\n' || *ap==0)) {
            // end of this parameter
          while(ap>aa && (ap[-1]=='\r' || ap[-1]=='\n')) *--ap= 0;
            // eliminate trailing NL
          if(ap>aa && ap[-1]==' ')
            *--ap= 0;  // cut one trailing space
      break;
          }
        ap= strchr(ap,0);  // find end of string
        while(ap>aa && ap[-1]=='\n')
          *--ap= 0;  // cut newline chars
        *ap++= ' '; *ap= 0;  // add a space
        }
      a= aa;
      while(*a!=0 && strchr(" \t\r\n",*a)!=NULL) a++;
      if(*a!=0)
    break;
    parafileend:
      fclose(parafile); parafile= NULL;
      free(aa); aa= NULL;
      } while(false);
    if(parafile==NULL) {
      if(--argc<=0)
  break;
      argv++;  // switch to next parameter; as the first one is just
        // the program name, we must do this previous reading the
        // first 'real' parameter;
      a= argv[0];
      }
    if((l= strzlcmp(a,"--parameter-file="))>0 && a[l]!=0) {
        // parameter file
      parafile= fopen(a+l,"r");
      if(parafile==NULL) {
        PERRv("Cannot open parameter file: %.80s",a+l)
        perror("osmconvert");
return 1;
        }
      aa= (char*)malloc(main__aaM);
      if(aa==NULL) {
        PERR("Cannot get memory for parameter file.")
        fclose(parafile); parafile= NULL;
return 1;
        }
      aa[0]= 0;
  continue;  // take next parameter
      }
    if(loglevel>0)  // verbose mode
      fprintf(stderr,"osmconvert Parameter: %.2000s\n",a);
    if(strcmp(a,"-h")==0) {  // user wants parameter overview
      fprintf(stdout,"%s",shorthelptext);  // print brief help text
        // (took "%s", to prevent oversensitive compiler reactions)
return 0;
      }
    if(strcmp(a,"-help")==0 || strcmp(a,"--help")==0) {
        // user wants help text
      fprintf(stdout,"%s",helptext);  // print help text
        // (took "%s", to prevent oversensitive compiler reactions)
return 0;
      }
    if(strzcmp(a,"--diff-c")==0) {
        // user wants a diff file to be calculated
      process->diffcontents=true;
      process->diff=true;
  continue;  // take next parameter
      }
    if(strcmp(a,"--diff")==0) {
        // user wants a diff file to be calculated
      process->diff= true;
  continue;  // take next parameter
      }
    if(strzcmp(a,"--drop-his")==0) {
        // (deprecated)
      PINFO("Option --drop-history is deprecated. Using --drop-author.");
      process->dropauthor= true;
  continue;  // take next parameter
      }
    if(strzcmp(a,"--drop-aut")==0) {
        // user does not want author information in standard output
      process->dropauthor= true;
  continue;  // take next parameter
      }
    if(strzcmp(a,"--drop-ver")==0) {
        // user does not want version number in standard output
      process->dropauthor= true;
      process->dropversion= true;
  continue;  // take next parameter
      }
    if(strzcmp(a,"--fake-his")==0) {
        // (deprecated)
      PINFO("Option --fake-history is deprecated. Using --fake-author.");
      process->fakeauthor= true;
  continue;  // take next parameter
      }
    if(strzcmp(a,"--fake-aut")==0) {
        // user wants faked author information
      process->fakeauthor= true;
  continue;  // take next parameter
      }
    if(strzcmp(a,"--fake-ver")==0) {
        // user wants just a faked version number as meta data
      process->fakeversion= true;
  continue;  // take next parameter
      }
    if(strzcmp(a,"--fake-lonlat")==0) {
        // user wants just faked longitude and latitude
        // in case of delete actions (.osc files);
      process->fakelonlat= true;
  continue;  // take next parameter
      }
    if(strzcmp(a,"--drop-bro")==0) {
        // user does not want broken references in standard output
      process->dropbrokenrefs= true;
  continue;  // take next parameter
      }
    if(strzcmp(a,"--drop-nod")==0) {
        // user does not want nodes section in standard output
      process->dropnodes= true;
  continue;  // take next parameter
      }
    if(strzcmp(a,"--drop-way")==0) {
        // user does not want ways section in standard output
      process->dropways= true;
  continue;  // take next parameter
      }
    if(strzcmp(a,"--drop-rel")==0) {
        // user does not want relations section in standard output
      process->droprelations= true;
  continue;  // take next parameter
      }
    if(strzcmp(a,"--merge-ver")==0) {
        // user wants duplicate versions in input files to be merged
      process->mergeversions= true;
  continue;  // take next parameter
      }
    if((l= strzlcmp(a,"--csv="))>0 && a[l]!=0) {
        // user-defined columns for csv format
      process->csv_ini(a+l);
      process->outcsv= true;
  continue;  // take next parameter
      }
    if(strcmp(a,"--csv-headline")==0) {
        // write headline to csv output
      process->csvheadline= true;
      process->outcsv= true;
  continue;  // take next parameter
      }
    if((l= strzlcmp(a,"--csv-separator="))>0 && a[l]!=0) {
        // user-defined separator for csv format
      strMcpy(process->csvseparator,a+l);
      process->outcsv= true;
  continue;  // take next parameter
      }
    if(strcmp(a,"--in-josm")==0) {
      // deprecated;
      // this option is still accepted for compatibility reasons;
  continue;  // take next parameter
      }
    if(strcmp(a,"--out-o5m")==0 ||
        strcmp(a,"-5")==0) {
        // user wants output in o5m format
      process->outo5m= true;
  continue;  // take next parameter
      }
    if(strcmp(a,"--out-o5c")==0 ||
        strcmp(a,"-5c")==0) {
        // user wants output in o5m format
      process->outo5m= process->outo5c= true;
  continue;  // take next parameter
      }
    if(strcmp(a,"--out-osm")==0) {
        // user wants output in osm format
      process->outosm= true;
  continue;  // take next parameter
      }
    if(strcmp(a,"--out-osc")==0) {
        // user wants output in osc format
      process->outosc= true;
  continue;  // take next parameter
      }
    if(strcmp(a,"--out-osh")==0) {
        // user wants output in osc format
      process->outosh= true;
  continue;  // take next parameter
      }
    if(strcmp(a,"--out-none")==0) {
        // user does not want any standard output
      process->outnone= true;
  continue;  // take next parameter
      }
    if(strcmp(a,"--out-pbf")==0) {
        // user wants output in PBF format
      process->outpbf= true;
  continue;  // take next parameter
      }
    if(strcmp(a,"--out-csv")==0) {
        // user wants output in CSV format
      process->outcsv= true;
  continue;  // take next parameter
      }
    if(strzcmp(a,"--emulate-pbf2")==0) {
        // emulate pbf2osm compatible output
      process->emulatepbf2osm= true;
  continue;  // take next parameter
      }
    if(strzcmp(a,"--emulate-osmo")==0) {
        // emulate Osmosis compatible output
      process->emulateosmosis= true;
  continue;  // take next parameter
      }
    if(strzcmp(a,"--emulate-osmi")==0) {
        // emulate Osmium compatible output
      process->emulateosmium= true;
  continue;  // take next parameter
      }
    if((l= strzlcmp(a,"--timestamp="))>0 && a[l]!=0) {
        // user-defined file timestamp
      process->timestamp= oo__strtimetosint64(a+l);
  continue;  // take next parameter
      }
    if(strcmp(a,"--out-timestamp")==0) {
        // user wants output in osc format
      process->outtimestamp= true;
  continue;  // take next parameter
      }
    if(strcmp(a,"--statistics")==0) {
        // print statistics (usually to stderr)
      process->statistics= true;
  continue;  // take next parameter
      }
    if(strcmp(a,"--out-statistics")==0) {  // print statistics to stdout
      process->outstatistics= true;
      process->statistics= true;
      process->outnone= true;
  continue;  // take next parameter
      }
    if(strcmp(a,"--complete-ways")==0) {
        // do not clip ways when applying borders
      process->completeways= true;
  continue;  // take next parameter
      }
    if(strcmp(a,"--complex-ways")==0) {
        // do not clip multipolygons when applying borders
      process->complexways= true;
  continue;  // take next parameter
      }
    if(strcmp(a,"--all-to-nodes")==0) {
        // convert ways and relations to nodes
      process->alltonodes= true;
  continue;  // take next parameter
      }
    if(strcmp(a,"--all-to-nodes-bbox")==0) {
        // convert ways and relations to nodes,
        // and compute a bounding box
      process->alltonodes= true;
      process->alltonodes_bbox= true;
  continue;  // take next parameter
      }
    if((l= strzlcmp(a,"--max-objects="))>0 && a[l]!=0) {
        // define maximum number of objects for --all-to-nodes
      process->maxobjects= oo__strtosint64(a+l);
      if(process->maxobjects<4) process->maxobjects= 4;
  continue;  // take next parameter
      }
    if((l= strzlcmp(a,"--max-refs="))>0 && a[l]!=0) {
        // define maximum number of references
      process->maxrefs= oo__strtosint64(a+l);
      if(process->maxrefs<1) process->maxrefs= 1;
  continue;  // take next parameter
      }
    if((l= strzlcmp(a,"--object-type-offset="))>0 && a[l]!=0) {
        // define id offset for ways and relations for --all-to-nodes
      process->otypeoffset10= oo__strtosint64(a+l);
      if(process->otypeoffset10<10) process->otypeoffset10= 10;
      if(strstr(a+l,"+1")!=NULL)
        process->otypeoffsetstep= true;
  continue;  // take next parameter
      }
    if(strzcmp(a,"-t=")==0 && a[3]!=0) {
        // user-defined prefix for names of temorary files
      strncpy(process->tempfilename,a+3,sizeof(process->tempfilename)-30);
  continue;  // take next parameter
      }
    if(strzcmp(a,"-o=")==0 && a[3]!=0) {
        // reroute standard output to a file
      strMcpy(outputfilename,a+3);
  continue;  // take next parameter
      }
    if((strcmp(a,"-v")==0 || strcmp(a,"--verbose")==0 ||
        strzcmp(a,"-v=")==0 || strzcmp(a,"--verbose=")==0) &&
        loglevel==0) {  // test mode - if not given already
      const char* sp;
      sp= strchr(a,(int)'=');
      if(sp!=NULL) loglevel= sp[1]-'0'; else loglevel= 1;
      if(loglevel<1) loglevel= 1;
      if(loglevel>MAXLOGLEVEL) loglevel= MAXLOGLEVEL;
      if(a[1]=='-') {  // must be "--verbose" and not "-v"
        if(loglevel==1)
          fprintf(stderr,"osmconvert: Verbose mode.\n");
        else
          fprintf(stderr,"osmconvert: Verbose mode %i.\n",loglevel);
        }
  continue;  // take next parameter
      }
    if(strcmp(a,"-t")==0) {
        // test mode
      process->write_testmode= true;
      fprintf(stderr,"osmconvert: Entering test mode.\n");
  continue;  // take next parameter
      }
    if(((l= strzlcmp(a,"--hash-memory="))>0 ||
        (l= strzlcmp(a,"-h="))>0) && isdig(a[l])) {
        // "-h=...": user wants a specific hash size;
      const char* p;

      p= a+l;  // jump over "-h="
      h_n= h_w= h_r= 0;
      // read the up to three values for hash tables' size;
      // format examples: "-h=200-20-10", "-h=1200"
      while(isdig(*p)) { h_n= h_n*10+*p-'0'; p++; }
      if(*p!=0) { p++; while(isdig(*p)) { h_w= h_w*10+*p-'0'; p++; } }
      if(*p!=0) { p++; while(isdig(*p)) { h_r= h_r*10+*p-'0'; p++; } }
  continue;  // take next parameter
      }
    if(strzcmp(a,"-b=")==0) {
        // border consideration by a bounding box
      if(!border_box(a+3)) {
        fprintf(stderr,"osmconvert Error: use border format: "
          " -b=\"x1,y1,x2,y2\"\n");
return 3;
        }  // end   border consideration by a bounding box
      continue;  // take next parameter
      }
    if(strzcmp(a,"-B=")==0) {
        // border consideration by polygon file
      if(!border_file(a+3)) {
        fprintf(stderr,
          "osmconvert Error: no polygon file or too large: %s\n",a);
return 4;
        }  // end   border consideration by polygon file
  continue;  // take next parameter
      }
    if(strcmp(a,"-")==0) {  // use standard input
      usesstdin= true;
      if(oo_open(NULL))  // file cannot be read
return 2;
  continue;  // take next parameter
      }
    if(a[0]=='-') {
      PERRv("unrecognized option: %.80s",a)
return 1;
      }
    // here: parameter must be a file name
    if(strcmp(a,"/dev/stdin")==0)
      usesstdin= true;
    if(oo_open(a))  // file cannot be read
return 2;
    }  // end   for every parameter in command line

  // process parameters
  if(usesstdin && process->completeways) {
    PERR("cannot apply --complete-ways when reading standard input.")
return 2;
    }
  if(usesstdin && process->complexways) {
    PERR("cannot apply --complex-ways when reading standard input.")
return 2;
    }
  if(process->completeways || process->complexways) {
    uint32_t zlibflags;
    zlibflags= zlibCompileFlags();
    if(loglevel>=2) {
      PINFOv("zlib " ZLIB_VERSION " flags: %08x",zlibflags);
      }
    //if((zlibflags&0xc0) <= 0x40)
      //WARN("you are using the 32 bit zlib. Hence file size max. 2 GB.")
    }
  if(oo_ifn==0) {  // no input files given
    PERR("use \"-\" to read from standard input or try:  osmconvert -h")
return 0;  // end the program, because without having input files
      // we do not know what to do;
    }
  process_outputfile(outputfilename);
  if(border_active || process->dropbrokenrefs) {  // user wants borders
    int r;

    if(process->diff) {
      PERR(
        "-b=, -B=, --drop-brokenrefs must not be combined with --diff");
return 6;
      }
    if(h_n==0) h_n= 600;  // use standard value if not set otherwise
    if(h_w==0 && h_r==0) {
        // user chose simple form for hash memory value
      // take the one given value as reference and determine the 
      // three values using these factors: 80%, 15%, 5%
      h_w= h_n/5; h_r= h_n/20;
      h_n-= h_w; h_w-= h_r; }
    r= hash_ini(h_n,h_w,h_r);  // initialize hash table
    if(r==1)
      fprintf(stderr,"osmconvert: Hash size had to be reduced.\n");
    else if(r==2)
      fprintf(stderr,"osmconvert: Not enough memory for hash.\n");
    }  // end   user wants borders
  if(process->outo5m || border_active || process->dropbrokenrefs ||
      process->alltonodes) {
      // .o5m format is needed as output
    if(o5_ini()!=0) {
      fprintf(stderr,"osmconvert: Not enough memory for .o5m buffer.\n");
return 5;
      }
    }  // end   user wants borders
  if(process->diff) {
    if(process->oo_ifn!=2) {
      PERR("Option --diff requires exactly two input files.");
return 7;
      }
    if(!process->outosc && !process->outosh && !process->outo5c)
      process->outosc= true;
    }  // end   diff
  sprintf(strchr(process->tempfilename,0),".%ld",(int64_t)getpid());
  if(loglevel>=2)
    fprintf(stderr,"Tempfiles: %s.*\n",process->tempfilename);
  if(process->alltonodes)
    posi_ini();
  if(process->outcsv)
    csv_ini(NULL);

  // do the work
  r= oo_main();
  if(loglevel>=2) {  // verbose
    if(read->bufp() !=NULL && read->bufp() <read->bufe()
)
      fprintf(stderr,"osmconvert: Next bytes to parse:\n"
        "  %.02X %.02X %.02X %.02X %.02X %.02X %.02X %.02X\n",
	      read->bufc(0),
	      read->bufc(1),
	      read->bufc(2),
	      read->bufc(3),
	      read->bufc(4),
	      read->bufc(5),
	      read->bufc(6),
	      read->bufc(7));
    }  // verbose
  write_flush();
  if(hash_queryerror()!=0)
    r= 91;
  if(write_error) {
    r= 92;
    PERR("write error.")
    }
  if(loglevel>0) {  // verbose mode
    if(oo_sequenceid()!=INT64_C(-0x7fffffffffffffff))
      {
	const char * oname=ONAME(oo_sequencetype());
	fprintf(stderr,"osmconvert: Last processed: %s %lu.\n",
		oname,
		oo_sequenceid());
      }
    if(r!=0)
      fprintf(stderr,"osmconvert Exit: %i\n",r);
    }  // verbose mode
  assistant(NULL,NULL);
  return r;
  }  // end   main()

