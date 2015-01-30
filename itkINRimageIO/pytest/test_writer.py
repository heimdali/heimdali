#!/usr/bin/env python


"""
USAGE

    python test_writer.py [TESTNAMES]

EXAMPLE

    Run all tests with:
        python test_writer.py

    Run tests on only these two files:
        python test_writer.py a-1o.inr a-1o-s.inr

FILES

    Input data and expected returned values are defined in test_input/writer.py

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

from test_input.writer import data as test_input_data

DATAPATH = ('../../../data')

class BaseInrimageTest(numpytestcase.NumpyTestCase):
    """
    Read an image, check its shape and values.

    Subclass must provide in the setUp method the filename, then call
    read_write.
    """

    def read_write(self):

        image_copy_filename = 'image_copy.inr'

        # Create types.
        ImageDimension = 3
        pixelType = itk.F
        ImageType = itk.VectorImage[pixelType, ImageDimension]
        ReaderType = itk.ImageFileReader[ImageType]
        WriterType = itk.ImageFileWriter[ImageType]
        itk_py_converter = PyBuffer[ImageType]

        # Read original image.
        filepath = os.path.join(DATAPATH,self.filename)
        reader = ReaderType.New()
        reader.SetFileName(filepath)
        reader.Update()

        # Save original values in numpy array.
        self.org_values = np.copy( itk_py_converter.GetArrayFromImage(reader.GetOutput()) )

        # Write original image to file.
        writer = WriterType.New()
        writer.SetFileName(image_copy_filename)
        writer.SetInput(reader.GetOutput())
        writer.Update()

        # Now, read the writen image.
        reader = ReaderType.New()
        reader.SetFileName(image_copy_filename)
        reader.Update()

        # Save copy values in numpy array.
        self.cpy_values = np.copy( itk_py_converter.GetArrayFromImage(reader.GetOutput()) )

    def test_values(self):

        self.assertArrayAlmostEqual(self.cpy_values, self.org_values)

    def tearDown(self):
        pass

def generate_testcase(filename):

    """
    Generate a TestCase with given values to be tested.
    """

    class InrimageTest(BaseInrimageTest):

        def setUp(self):

            self.filename = filename
            self.read_write()

    InrimageTest.__name__ = 'Test_' + filename

    return InrimageTest


def get_suite(filenames=[]):
    """
    Generate tests suite with data.
    """

    all_suites = []

    for filename in filenames if filenames else test_input_data:
        test_case = generate_testcase(filename)
        suite = unittest.TestLoader().loadTestsFromTestCase(test_case)
        all_suites.extend(suite)

    suite = unittest.TestSuite(all_suites)

    return suite

if __name__ == '__main__':

    # Add our INRimage reader/writer to the list of readers ITK knows. 
    itk.ObjectFactoryBase.RegisterFactory( itk.INRImageIOFactory.New() )

    # Generate test cases suite.
    suite = get_suite(filenames = sys.argv[1:])

    # Run the suite.
    unittest.TextTestRunner().run(suite)
