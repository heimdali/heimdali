#!/usr/bin/env python

"""
USAGE

    python inrimage_reader_test.py [TESTNAMES]

EXAMPLE

    Run all tests with:
        python inrimage_reader_test.py

    Run tests on only these two files:
        python inrimage_reader_test.py a-1o.inr a-1o-s.inr

FILES

    Input data and expected returned values are defined in test_input/reader.py

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

from test_input.reader import data as test_input_data

DATAPATH = ('../../../data')

class BaseInrimageTest(numpytestcase.NumpyTestCase):
    """
    Read an image, check its shape and values.

    Subclass must provide in the setUp method the filename, dimension, 
    expected_shape, indx and expected_value, then call read_image.
    """

    def read_image(self):

        # Create image type.
        ImageDimension = self.dimension 
        pixelType = itk.F
        ImageType = itk.VectorImage[pixelType, ImageDimension]

        # Create reader.
        filepath = os.path.join(DATAPATH,self.filename)
        ReaderType = itk.ImageFileReader[ImageType]
        reader = ReaderType.New()
        reader.SetFileName(filepath)

        # Update pipeline.
        reader.Update()

        # Convert to numpy array.
        # Note that we holds a reference to ITK image (self.image), not only a
        # reference to numpy array. This way, ITK image is not garbage
        # collected immediatelly, and numpy array does not reference
        # to a deleted C++ memory.
        self.image = reader.GetOutput()
        itk_py_converter = PyBuffer[ImageType]
        self.np_image = itk_py_converter.GetArrayFromImage(self.image)

    def test_shape(self):
        self.assertEqual(self.np_image.shape, self.expected_shape)

    def test_values(self):
        value = self.np_image[self.indx]

        if isinstance(value,np.ndarray):
            self.assertArrayAlmostEqual(value,self.expected_value)
        else:
            self.assertAlmostEqual(value,self.expected_value)

    def tearDown(self):
        pass

def generate_testcase(name,filename,dimension,expected_shape,indx,expected_value):

    """
    Generate a TestCase with given values to be tested.
    """

    class InrimageTest(BaseInrimageTest):

        def setUp(self):

            # Set input data and expected values.
            self.filename = filename
            self.dimension = dimension
            self.expected_shape = expected_shape
            self.indx = indx
            self.expected_value = expected_value

            self.read_image()

    InrimageTest.__name__ = name

    return InrimageTest


def get_suite(names=[]):
    """
    Generate tests suite with data.
    """

    all_suites = []

    for name in names if names else test_input_data.keys():
        filename,dimension,expected_shape,indx,expected_value = test_input_data[name]
        test_case = generate_testcase(name,filename,dimension,expected_shape,indx,expected_value)
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
