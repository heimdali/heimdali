#include "heimdali/regionreader.hxx"

namespace Heimdali {

/*! \brief Returns appropriately a RegionReaderByLines or a RegionReaderByPlanes */
RegionReader* RegionReader::make_region_reader(unsigned int sz, unsigned int sy, unsigned int nlines_per_loop)
{
    // total number of lines to read is the number of planes times the number
    // of lines per plane.
    unsigned int nlines = sz*sy;

    nlines_per_loop = nlines_per_loop == 0 ? nlines : nlines_per_loop;

    // nzmax is the number of plane we read at each loop,
    // sy is the number of lines per plane.
    unsigned int nzmax = nlines_per_loop / sy;

    if (nzmax > 0) {
        return new RegionReaderByPlanes(sz,sy, nzmax);
    } else {
        unsigned int nymax = nlines_per_loop;
        return new RegionReaderByLines(sz,sy, nymax);
    }
}

RegionReaderByLines::RegionReaderByLines(unsigned int sz, unsigned int sy, unsigned int nymax)
{
    m_sz = sz;
    m_sy = sy;
    m_iy = -nymax;
    m_iz = 0;
    m_nz = 1;
    m_ny = nymax;
    m_is_complete = false;
    m_nymax = nymax;
}

void RegionReaderByLines::next_iteration()
{
    // Prepare to read next lines.
    m_iy += m_ny;
    m_ny = m_nymax;

    // Move to next plane.
    if (m_iy >= m_sy) {
        m_iz += 1;
        if (m_iz >= m_sz) {
            m_is_complete = true;
            return;
        }
        m_iy =  m_iy - m_sy;
    }

    // Don't read lines out of the plane.
    if (m_iy+m_ny-1 >= m_sy) {
        m_ny = m_sy-m_iy;
    }
}

void RegionReaderByLines::values(unsigned int &iz, unsigned int &iy, unsigned int &nz, unsigned int &ny)
{
    iz = m_iz;
    iy = m_iy;
    nz = m_nz;
    ny = m_ny;
}

RegionReaderByPlanes::RegionReaderByPlanes(unsigned int sz, unsigned int sy, unsigned int nzmax)
{
    m_sz = sz;
    m_sy = sy;
    m_iy = 0;
    m_iz = -nzmax;
    m_nz = nzmax;
    m_ny = sy;
    m_is_complete = false;
    m_nzmax = nzmax;
}

void RegionReaderByPlanes::next_iteration()
{
    // Prepare to read next planes.
    m_iz += m_nzmax;
    m_nz = m_nzmax;

    // No more planes to read.
    if (m_iz >= m_sz) {
        m_is_complete = true;
        return;
    }

    // Still planes to read, but less that nzmax.
    else if (m_iz+m_nz-1 >= m_sz) {
        m_nz = m_sz-m_iz;
    }

}

void RegionReaderByPlanes::values(unsigned int &iz, unsigned int &iy, unsigned int &nz, unsigned int &ny)
{
    iz = m_iz;
    iy = m_iy;
    nz = m_nz;
    ny = m_ny;
}

};
