#ifndef REGIONREADER_HXX_INCLUDED
#define REGIONREADER_HXX_INCLUDED

#include <stdexcept>
#include <stdlib.h>
#include <iostream>

#include "hdf5.h"
#include "H5Cpp.h"
#include "H5LTpublic.h"

namespace Heimdali {

class RegionReader
{
    public:
        static RegionReader* make_region_reader(unsigned int sz, unsigned int sy, unsigned int nlines_per_loop);
        virtual void next_iteration() = 0;
        virtual void values(unsigned int &iz, unsigned int &iy, 
            unsigned int &nz, unsigned int &ny) = 0;
        bool is_complete(){return m_is_complete;};
    protected:
        unsigned int m_sz,m_sy, m_iz,m_iy, m_nz,m_ny;
        bool m_is_complete;
};

class RegionReaderByLines: public RegionReader
{
    public:
        RegionReaderByLines(){};
        RegionReaderByLines(unsigned int sz, unsigned int sy, unsigned int nymax);
        void next_iteration();
        void values(unsigned int &iz, unsigned int &iy, unsigned int &nz, unsigned int &ny);
    private:
        unsigned int m_nymax;
};

class RegionReaderByPlanes: public RegionReader
{
    public:
        RegionReaderByPlanes(){};
        RegionReaderByPlanes(unsigned int sz, unsigned int sy, unsigned int nymax);
        void next_iteration();
        void values(unsigned int &iz, unsigned int &iy, unsigned int &nz, unsigned int &ny);
    private:
        unsigned int m_nzmax;
};

};

#endif
