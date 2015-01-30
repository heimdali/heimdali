#!/usr/bin/env python

import unittest
import numpy as np

def assertArrayAlmostEqual(array1,array2,eps=1.E-07):

    s1 = np.shape(array1)
    s2 = np.shape(array2)

    if s1 != s2:
        raise AssertionError,'arrays have different shapes: %r != %r' % (s1,s2,)

    are_different = np.abs(array1-array2) > eps

    if np.any( are_different ):
        diff1 = array1[are_different]
        diff2 = array2[are_different]
        ndiff = len(diff1)

        if array1.size < 20:
            msg = 'arrays %s and %s have %i different value(s) with eps=%r, first are:: %r != %r' \
                % (array1,array2,ndiff,eps,diff1[0],diff2[0])
        else:
            msg = 'arrays have %i different value(s) with eps=%r, first are:: %r != %r' \
                % (ndiff,eps,diff1[0],diff2[0])

        raise AssertionError, msg

class NumpyTestCase(unittest.TestCase):

    """
    Provide a method to compare 2 numpy arrays.
    """

    def assertArrayAlmostEqual(self,array1,array2,eps=1.E-07):

        assertArrayAlmostEqual(array1,array2,eps)

