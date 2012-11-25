  void read_o5m2_way( bytev_t::iterator & bufp) {
    in.l= pbf.uint32(bufp);
    bytev_t::iterator bp= bufp.limit(in.l); // limit the read to this pointer

    //    int64_t* refidc;  // pointer for object contents comparison

    // read the buffer and iterate over the existing nods in the way
   while(bufp<bp && refidc<refidee) {
     sint64_t offset= pbf.sint64(&bufp);
     oo.ifp->o5rid[0]+= offset;      
     if(*refidc!=oo.ifp->o5rid[0])	{
       diffdifference= true; // the node is different
     }
     refidc++; // next ref to a node in the way
   }    
  }

  void read_o5m2_rel( bytev_t::iterator & bufp) {
    
    int64_t ri;  // temporary, refid
    int rt;  // temporary, reftype
    charv_t::iterator rr;  // temporary, refrole
    
    l= pbf.uint32(&bufp);
    bp= bufp+l;
    if(bp>bufe) bp= bufe;  // (format error)
    while(bufp<bp && refidc<refidee) {
      ri= pbf.sint64(&bufp);
      str.read(&bufp,rr);
      if(*reftypec!=(rt= (*rr++ -'0')%3))
	diffdifference= true;
      if(*refidc!=(oo.ifp->o5rid[rt]+= ri))
	diffdifference= true;
      if(refrolec>=refrolee || strvcmp(*refrolec,rr)!=0)
	diffdifference= true;
      reftypec++;
      refidc++;
      refrolec++;
    }    
  }

  void rundiff() {
          
      // initialize comparison variables
      //refidc= refid.begin();
      //      reftypec= reftype.begin();
      //      refrolec= refrole.begin();
      //keyc= key.begin();
      //      valc= val.begin();
      
      // compare object id
      if(in.id!=(oo.ifp->o5id+= pbf.sint64(bufp)))
	diffdifference= true;
      
      // compare author
      in.hisverc= pbf.uint32(bufp);
      if(in.hisverc!=in.hisver)
	diffdifference= true;
      if(in.hisverc!=0) {  // author information available
	in.histimec= oo.ifp->o5histime+= pbf.sint64(bufp);
	if(in.histimec!=0) {
	  if(in.histimec!=in.histime)
	    diffdifference= true;
	  if(in.hiscset!=(oo.ifp->o5hiscset+= pbf.sint32(bufp)))
	    diffdifference= true;
	  Str::String sp;
	  str.read(bufp,sp,in.hisuserc);

	  if(in.hisuserc != in.hisuser)
	    diffdifference= true;

	  if(in.hisuid!=pbf.uint64(sp))
	    diffdifference= true;
	}
      }  // end   author information available
      
      if(!bufp) { // end of string
	// just the id and author, i.e. this is a delete request
	oo.ifp->deleteobject= 1;
	diffdifference= true;
      }
      else {  // not a delete request
	oo.ifp->deleteobject= 0;
	
	// compare coordinates (for nodes only)
	if(in.otype==0) {  // node
	  // read node body
	  if(in.lon!=(oo.ifp->o5lon+= pbf.sint32(bufp)))
              diffdifference= true;
            if(in.lat!=(oo.ifp->o5lat+= pbf.sint32(bufp)))
              diffdifference= true;
	  }  // end   node

          // compare noderefs (for ways only)
          if(in.otype==1) {  // way
	    read_o5m2_way(bufp);
	  }  // end   way

          // compare refs (for relations only)
          else if(in.otype==2) {  // relation
	    read_o5m2_rel(bufp);
	  }  // end   relation

          // compare node key/val pairs
          while(bufp<bufe && keyc<key.end()) {
            char * k;
	    const char * v="";
            str.read(&bufp,&k,&v);
            if(keyc>=key.end() || strvcmp(k,*keyc)!=0 || strvcmp(v,*valc)!=0)
              diffdifference= true;
            keyc++; valc++;
	  }
	}  // end   not a delete request

        // compare indexes
        if(keyc!=key.end() || (in.otype>0 && refidc!=refide))
          diffdifference= true;

  }
