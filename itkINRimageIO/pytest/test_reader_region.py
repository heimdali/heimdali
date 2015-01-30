#!/usr/bin/env python

"""
USAGE

    python inrimage_reader_region_test.py [TESTNAMES]

DESCRIPTION

    Test the streaming capability of the INRimage reader. Streaming mean
    reading from a file only a region of an image, instead of reading the
    whole image.

    The tests assumes that reading a whole image from a file with the INRimage reader 
    works without errors. (This is tested in image_reader_test.py).

    The tests consists in:
       - First, reading from a file a whole image, and extract a region.
       - Second, reading from a file only the region (streming).
       - Check that the values of the two regions are equal.

EXAMPLE

    Run all tests with:
        python inrimage_reader_region_test.py

    Run tests on only these two files:
        python inrimage_reader_region_test.py a-1o.inr a-1o-s.inr

FILES

    Input data and expected returned values are defined in test_input/reader_region.py

    Data files are expected to be in ../../../data (a command-line options may
    be add to change this directory).
"""

import sys
import os
import unittest

import numpy as np

import itk
from itk import PyBuffer

ROOT = os.path.dirname( __file__ )
sys.path.append( os.path.join(ROOT,'lib') )
import numpytestcase

from test_input.reader_region import data as test_input_data

DATAPATH = ('../../../data')

class INRimage:
    """
    Read from a file a complete image, and extract and region from it.
    """

    def __init__(self,filepath,dimension):

        self.dimension = dimension

        # Create image type.
        ImageDimension = dimension 
        pixelType = itk.F
        ImageType = itk.VectorImage[pixelType, ImageDimension]

        # Create reader.
        ReaderType = itk.ImageFileReader[ImageType]
        reader = ReaderType.New()
        reader.SetFileName(filepath)

        # Update pipeline.
        reader.Update()

        # Convert to numpy array.
        self.image = reader.GetOutput()
        itk_py_converter = PyBuffer[ImageType]
        self.np_image = itk_py_converter.GetArrayFromImage(self.image)

    def get_region(self,indx,leng):

        if self.dimension==2:
            iy,ix = indx
            ny,nx = leng
            return self.np_image[iy:iy+ny, ix:ix+nx]
        else:
            iz,iy,ix = indx
            nz,ny,nx = leng
            return self.np_image[iz:iz+nz, iy:iy+ny, ix:ix+nx]

class INRimageRegion:
    """
    Read from a file only a region of an image.
    """

    def __init__(self,filepath,dimension,indx,leng):

        self.dimension = dimension

        # Create image type.
        ImageDimension = dimension 
        pixelType = itk.F
        ImageType = itk.VectorImage[pixelType, ImageDimension]

        # Create reader.
        ReaderType = itk.ImageFileReader[ImageType]
        reader = ReaderType.New()
        reader.SetFileName(filepath)

        # Set requested region.
        desiredRegion = itk.ImageRegion[ImageDimension]()
        if self.dimension==2:
            iy,ix = indx
            ny,nx = leng
            desiredRegion.SetIndex( [ix,iy] )
            desiredRegion.SetSize( [nx,ny] )
        else:
            iz,iy,ix = indx
            nz,ny,nx = leng
            desiredRegion.SetIndex( [ix,iy,iz] )
            desiredRegion.SetSize( [nx,ny,nz] )
        reader.GetOutput().SetRequestedRegion(desiredRegion)

        # Update pipeline.
        reader.Update()

        # Convert to numpy array.
        self.image = reader.GetOutput()
        itk_py_converter = PyBuffer[ImageType]
        self.np_image = itk_py_converter.GetArrayFromImage(self.image)

    def get_region(self):

        return self.np_image

class BaseInrimageRegionTest(numpytestcase.NumpyTestCase):
    """
    Read from a file a region of an image in two ways:
       - read the whole image, and extract the region,
       - read only the region.

    Then the two regions are check to be equal.

    Subclass must provide in the setUp method the filepath, dimension, 
    indx and leng, then call read_images.
    """

    def read_images(self):

        self.image_ref = INRimage(self.filepath,self.dimension)
        self.region_ref = self.image_ref.get_region(self.indx,self.leng)

        self.image_region = INRimageRegion(self.filepath,self.dimension,self.indx,self.leng)
        self.region = self.image_region.get_region()

    def test_values(self):

        self.assertArrayAlmostEqual(self.region, self.region_ref)

    def tearDown(self):
        pass

def generate_testcase(name,filepath,dimension,indx,leng):

    """
    Generate a TestCase with given values to be tested.
    """

    class InrimageTest(BaseInrimageRegionTest):

        def setUp(self):

            # Set input data and expected values.
            self.filepath = filepath
            self.dimension = dimension
            self.indx = indx
            self.leng = leng

            self.read_images()

    InrimageTest.__name__ = name

    return InrimageTest


def get_suite(names=[]):
    """
    Generate tests suite with data.
    """

    all_suites = []

    for name in names if names else test_input_data:
        filename,indx,leng = test_input_data[name]
        assert len(indx) == len(leng)
        dimension = len(indx)
        filepath = os.path.join(DATAPATH,filename)
        test_case = generate_testcase(name,filepath,dimension,indx,leng)
        suite = unittest.TestLoader().loadTestsFromTestCase(test_case)
        all_suites.extend(suite)

    suite = unittest.TestSuite(all_suites)

    return suite

if __name__ == '__main__':

    # Add our INRimage reader to the list of readers ITK knows. 
    itk.ObjectFactoryBase.RegisterFactory( itk.INRImageIOFactory.New() )

    # Generate test cases suite.
    suite = get_suite(names = sys.argv[1:])

    # Run the suite.
    unittest.TextTestRunner().run(suite)
