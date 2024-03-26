/*  first_mult_bcorr.cc
    Mark Jenkinson
    Copyright (C) 2009 University of Oxford  */

/*  CCOPYRIGHT */

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <cmath>
#include <algorithm>
#include <vector>
#include "math.h"

#include "utils/options.h"
#include "armawrap/newmat.h"
#include "miscmaths/miscmaths.h"
#include "newimage/newimageall.h"

using namespace std;
using namespace Utilities;
using namespace NEWMAT;
using namespace MISCMATHS;
using namespace NEWIMAGE;

string title="first_mult_bcorr (Version 1.0) University of Oxford (Mark Jenkinson)\nConverts ";
string examples="first_mult_bcorr [options] -i <T1_image> -c <4D_corrected_labels> -u <4D_uncorrected_labels> -o <output_image>";


Option<bool> verbose(string("-v,--verbose"), false,
		     string("output F-stats to standard out"),
		     false, no_argument);
Option<bool> help(string("-h,--help"), false,
		  string("display this message"),
		  false, no_argument);
Option<string> inname(string("-i,--in"), string(""),
		      string("filename of original T1 input image"),
		      true, requires_argument);
Option<string> ucorrname(string("-u,--uncorrected"), string(""),
		       string("filename of 4D image of uncorrected labels (with boundaries)"),
		       true, requires_argument);
Option<string> corrname(string("-c,--corrected"), string(""),
		       string("filename of 4D image of individually corrected labels"),
		       true, requires_argument);
Option<string> outname(string("-o,--out"), string(""),
		       string("output image name (3D label image)"),
		       true, requires_argument);

int nonoptarg;

////////////////////////////////////////////////////////////////////////////

// global variable for holding internal intensities of each structure

vector<volume<float> > internalints;

////////////////////////////////////////////////////////////////////////////


int set_up_globals(const volume<float>& t1im, const volume4D<int>& ucorrim)
{
  // store the set of internal intensities for each structure (for efficiency)
  volume<float> tmpvol;
  internalints.clear();
  for (int s=0; s<ucorrim.tsize(); s++) {
    int nvox=0;
    for (int z=ucorrim.minz(); z<=ucorrim.maxz(); z++) {
      for (int y=ucorrim.miny(); y<=ucorrim.maxy(); y++) {
	for (int x=ucorrim.minx(); x<=ucorrim.maxx(); x++) {
	  if ((ucorrim(x,y,z,s)>0) && (ucorrim(x,y,z,s)<100)) {
	    nvox++;
	  }
	}
      }
    }
    tmpvol.reinitialize(nvox,1,1);
    nvox=0;
    for (int z=ucorrim.minz(); z<=ucorrim.maxz(); z++) {
      for (int y=ucorrim.miny(); y<=ucorrim.maxy(); y++) {
	for (int x=ucorrim.minx(); x<=ucorrim.maxx(); x++) {
	  if ((ucorrim(x,y,z,s)>0) && (ucorrim(x,y,z,s)<100)) {
	    tmpvol(nvox++,0,0)=t1im(x,y,z);
	  }
	}
      }
    }
    internalints.push_back(tmpvol);
  }
  return 0;
}



int labelval(const volume<int>& lvol)
{
  int val=200;
  for (int z=lvol.minz(); z<=lvol.maxz(); z++) {
    for (int y=lvol.miny(); y<=lvol.maxy(); y++) {
      for (int x=lvol.minx(); x<=lvol.maxx(); x++) {
	if ((lvol(x,y,z)>0) && (lvol(x,y,z)<val)) {
	  val=lvol(x,y,z);
	}
      }
    }
  }
  return val;
}

float scoreval(float fval, int ulab, int clab, int shapenum)
{
  // score based on reflected cdf around the median
  //  (i.e. closer to the median = better score)
  if (clab==0) { return 0.0; }
  int nint=internalints[shapenum].xsize(), nlower=0;
  for (int x=0; x<nint; x++) {
    if (internalints[shapenum](x,0,0)<fval) { nlower++; }
  }
  float score=0;
  if (nint>0) {
    score=((float) nlower)/((float) nint);
    if (score>0.5) { score=1.0-score; }
    if ((ulab>0) && (ulab<100)) { score += 1; }  // favour interior points
  }
  if (ulab<=0) { cerr << "Inconsistency in labelings passed to score" << endl; }
  return score;
}

int do_work()
{
  volume<float> t1im;
  volume4D<int> ucorrim, corrim;
  read_volume(t1im,inname.value());
  read_volume4D(ucorrim,ucorrname.value());
  read_volume4D(corrim,corrname.value());
  if (!samesize(ucorrim,corrim)) {
    cerr << "ERROR: uncorrected and corrected images must be the same size" << endl;
    exit(EXIT_FAILURE);
  }
  if (!samesize(t1im,corrim[0])) {
    cerr << "ERROR: T1 image and corrected image must be the same size" << endl;
    exit(EXIT_FAILURE);
  }
  if (!samesize(t1im,ucorrim[0])) {
    cerr << "ERROR: T1 image and uncorrected image must be the same size"<<endl;
    exit(EXIT_FAILURE);
  }

  volume<int> outim(corrim[0]);
  outim*=0;

  int nsh=corrim.tsize(), nlab;

  set_up_globals(t1im,ucorrim);

  for (int z=t1im.minz(); z<=t1im.maxz(); z++) {
    for (int y=t1im.miny(); y<=t1im.maxy(); y++) {
      for (int x=t1im.minx(); x<=t1im.maxx(); x++) {
	// work out how many shapes label this voxel
	nlab=0;
	for (int t=0; t<nsh; t++) {
	  if (corrim(x,y,z,t)>0) {
	    nlab++;
	    outim(x,y,z)=corrim(x,y,z,t);  // useful if no conflict
	  }
	}
	if (nlab>1) {  // only do overlapping voxels (conflicts)
	  // calculate the cdf of this voxel's intensity wrt the internal
	  //  intensity dist for each shape
	  if (verbose.value()) { cout << "Found overlapping labels at voxel (" << x << "," << y << "," << z << ")" << endl; }
	  int bestlab=-1;
	  float bestscore=-1.0, score;
	  for (int lab=0; lab<nsh; lab++) {
	    score = scoreval(t1im(x,y,z),ucorrim(x,y,z,lab),corrim(x,y,z,lab),lab);
	    if (score>bestscore) { bestscore=score; bestlab=lab; }
	    //if (verbose.value()) { cout << "  score for label " << lab << " is " << score << endl; }
	  }
	  if (bestlab>=0) {
	    outim(x,y,z)=labelval(ucorrim[bestlab]);
	  } // else it failed, so do nothing
	}
      }
    }
  }
  outim.setAuxFile("MGH-Subcortical");  // set nice colormap
  save_volume(outim,outname.value());
  return 0;
}

////////////////////////////////////////////////////////////////////////////

int main(int argc,char *argv[])
{

  Tracer tr("main");
  OptionParser options(title, examples);

  try {
    // must include all wanted options here (the order determines how
    //  the help message is printed)
    options.add(inname);
    options.add(outname);
    options.add(ucorrname);
    options.add(corrname);
    options.add(verbose);
    options.add(help);
    nonoptarg = options.parse_command_line(argc, argv);

    // line below stops the program if the help was requested or
    //  a compulsory option was not set
    if ( (help.value()) || (!options.check_compulsory_arguments(true)) )
      {
	options.usage();
	exit(EXIT_FAILURE);
      }

    // Call the local functions
    do_work();

  } catch(X_OptionError& e) {
    options.usage();
    cerr << endl << e.what() << endl;
    exit(EXIT_FAILURE);
  } catch(std::exception &e) {
    cerr << e.what() << endl;
  } catch(...) {
    cerr << "Aborted" << endl;
  }

  return 0;// do_work(argc,argv);
}
