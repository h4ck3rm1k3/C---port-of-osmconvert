 int process_outputfile(const char * outputfilename) {
  if(outputfilename[0]!=0 && !outo5m &&
      !global_outo5c && !outosm && !outosc &&
      !global_outosh && !outpbf && !outcsv &&
      !global_outnone && !outstatistics) {
      // have output file name AND  output format not defined
    // try to determine the output format by evaluating
    // the file name extension
    if(strycmp(outputfilename,".o5m")==0) outo5m= true;
    else if(strycmp(outputfilename,".o5c")==0) outo5c= true;
    else if(strycmp(outputfilename,".osm")==0) outosm= true;
    else if(strycmp(outputfilename,".osc")==0) outosc= true;
    else if(strycmp(outputfilename,".osh")==0) outosh= true;
    else if(strycmp(outputfilename,".pbf")==0) outpbf= true;
    else if(strycmp(outputfilename,".csv")==0) outcsv= true;
    }
  if(write_open(outputfilename[0]!=0? outputfilename: NULL)!=0)    {
    return 3;
  }
}
 
