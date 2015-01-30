#ifndef REGIONREADER_HXX_INCLUDED
#define REGIONREADER_HXX_INCLUDED

#include <stdexcept>
#include <stdlib.h>
#include <iostream>

#include "hdf5.h"
#include "H5Cpp.h"
#include "H5LTpublic.h"

// Needed by SWIG.
typedef size_t osize_t;

class RegionReader
{
    public:
        static RegionReader* make_region_reader(size_t sz, size_t sy, size_t nlines_per_loop);
        virtual void next_iteration() = 0;
        virtual void values(osize_t &iz, osize_t &iy, 
            osize_t &nz, osize_t &ny) = 0;
        bool is_complete(){return m_is_complete;};
    protected:
        size_t m_sz,m_sy, m_iz,m_iy, m_nz,m_ny;
        bool m_is_complete;
};

class RegionReaderByLines: public RegionReader
{
    public:
        RegionReaderByLines(){};
        RegionReaderByLines(size_t sz, size_t sy, size_t nymax);
        void next_iteration();
        void values(osize_t &iz, osize_t &iy, osize_t &nz, osize_t &ny);
    private:
        size_t m_nymax;
};

class RegionReaderByPlanes: public RegionReader
{
    public:
        RegionReaderByPlanes(){};
        RegionReaderByPlanes(size_t sz, size_t sy, size_t nymax);
        void next_iteration();
        void values(osize_t &iz, osize_t &iy, osize_t &nz, osize_t &ny);
    private:
        size_t m_nzmax;
};

#endif
