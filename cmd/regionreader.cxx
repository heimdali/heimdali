#include <iostream>
#include <vector>
#include <string>

#include "heimdali/error.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/version.hxx"
#include "heimdali/regionreader.hxx"

using namespace std;

int main(int argc, char** argv)
{
    try {

    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);

    TCLAP::CmdLine parser("Print subregions to iterate on",' ', HEIMDALI_VERSION);

    // -sz -sy
    TCLAP::ValueArg<unsigned int> szArg("","sz", "Total umber of planes",true,0,"sz", parser);
    TCLAP::ValueArg<unsigned int> syArg("","sy", "Total umber of rows",true,0,"sy", parser);

    // -streaming N
    TCLAP::ValueArg<int> streamingArg("s","streaming", "Number of lines to stream",
        false, 0,"NUMBER_OF_LINES", parser);

    parser.parse(tclap_argv);

    unsigned int sz = szArg.getValue();
    unsigned int sy = syArg.getValue();

    unsigned int nlines_per_loop = streamingArg.getValue();

    Heimdali::RegionReader* regionreader = Heimdali::RegionReader::make_region_reader(sz, sy, nlines_per_loop);

    unsigned int iz, iy, nz, ny;
    unsigned int iregionmax = 1E+06;

    for (unsigned int iregion=0 ; iregion<iregionmax ; iregion++) {
        regionreader->next_iteration();
        if (regionreader->is_complete()) break;
        regionreader->values(iz,iy,nz,ny);
        cout << iz << " " << iy << " " << nz << " " << ny << endl;
    }

    delete regionreader;

    }

    HEIMDALI_CATCH_EXCEPTIONS_WITHOUT_ITK(argv[0]);
}

