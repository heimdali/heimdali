/* Glossary.
 *
 * h5file
 *    Classical HDF5 file stored on an hard disk.
 *
 * fileimage
 *    Like an h5image, but instead of being stored on an hard disk, is stored
 *    in allocated memory of our C++ program. It suppot the same write/read
 *    operator than a h5image, all of them being performed in the memory of
 *    our running C++ program, never on the hard disk.
 *
 * fileimagebuffer
 *    The full content of the fileimage, ie all of the data and metadata. It can be
 *    binary written to a file to produce a valid h5image. Symetrically, it
 *    can be binary read from a h5image to a buffer to procude a valid
 *    fileiamgebuffer.
 */

#include "regionreader.hxx"

/*! \brief Returns appropriately a RegionReaderByLines or a RegionReaderByPlanes */
RegionReader* RegionReader::make_region_reader(size_t sz, size_t sy, size_t nlines_per_loop)
{
    // total number of lines to read is the number of planes times the number
    // of lines per plane.
    size_t nlines = sz*sy;

    nlines_per_loop = nlines_per_loop == 0 ? nlines : nlines_per_loop;

    // nzmax is the number of plane we read at each loop,
    // sy is the number of lines per plane.
    size_t nzmax = nlines_per_loop / sy;

    if (nzmax > 0) {
        return new RegionReaderByPlanes(sz,sy, nzmax);
    } else {
        size_t nymax = nlines_per_loop;
        return new RegionReaderByLines(sz,sy, nymax);
    }
}

RegionReaderByLines::RegionReaderByLines(size_t sz, size_t sy, size_t nymax)
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

void RegionReaderByLines::values(osize_t &iz, osize_t &iy, osize_t &nz, osize_t &ny)
{
    iz = m_iz;
    iy = m_iy;
    nz = m_nz;
    ny = m_ny;
}

RegionReaderByPlanes::RegionReaderByPlanes(size_t sz, size_t sy, size_t nzmax)
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

void RegionReaderByPlanes::values(osize_t &iz, osize_t &iy, osize_t &nz, osize_t &ny)
{
    iz = m_iz;
    iy = m_iy;
    nz = m_nz;
    ny = m_ny;
}
