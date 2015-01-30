#!/usr/bin/env python

import numpy as np

"""
Provide input data and expected value, to be passed to the tests in
test_reader.py

This is a dictionnary, where keys are filenames, and values are :
[ (exepected shape), (slice object), expected value ]

Expected values can be scalar or NumPy arrays.

For example, if slice object is (2,3) and expected value is 6.5, the
test will consist in checking that np_image[2,3] value is 6.5, where np_image
is the ITK image contained in the file, read and converted to NumPy array.
Note that slice(None) is equivalent to :.

In comments, command-lines used to deterime the reference values are provided.

Note that for vectorial image, with the 'tpr' command, the number of elements
in the vector is counted in x. For exemple, element of index:
    iz=2 iy=50 ix=100 iv=2 (counting from 0)

is obtained with (counting from 1):
    tpr -iz 3 -iy 51 -ix 201 -z 1 -y 1 -x 2 FILENAME

there are nx*nv composant in x, so ix=101 iv=1
is actually 201
"""

data = {

# Codages virgules fixes
# ----------------------

# Codages scalaires

    # par a-1o.inr
    # tpr -iy 101 -ix 91 -y 1 -x 1 -f='%.10g' a-1o.inr
    "a-1o.inr" : ["a-1o.inr",  2, (582,782), (100,90), 0.360784322 ],
    "a-1o.inr-3d" : ["a-1o.inr",  3, (1,582,782), (0,100,90), 0.360784322 ],

    # par a-1o-s.inr
    # tpr -iy 51 -ix 81 -y 1 -x 1 -f='%.10g' a-1o-s.inr
    "a-1o-s.inr" : ["a-1o-s.inr",  2, (256,256), (50,80), 0.4015747905 ],

    # par a-2o.inr
    # tpr -iy 51 -ix 81 -y 1 -x 1 -f='%.10g' a-2o.inr
    "a-2o.inr" : ["a-2o.inr",  2, (582,782), (50,80), 0.002426184481 ],

    # par a-2o-sun.inr
    # tpr -iy 51 -ix 81 -y 1 -x 1 -f='%.10g' a-2o-sun.inr
    "a-2o-sun.inr" : ["a-2o-sun.inr",  2, (582,782), (50,80), 0.00233463035 ],

# Codages vectoriels

    # par a-1o-2v.inr
    # tpr -iy 101 -ix 301 -y 1 -x 2 -f='%.10g' a-1o-2v.inr
    "a-1o-2v.inr" : ["a-1o-2v.inr",  2, (537,537,2), (100,150,slice(None)), 
        np.array((0.4666666687, 1)) ],

    # par a-1o-3v.inr
    # tpr -iy 101 -ix 451 -y 1 -x 3 -f='%.10g' a-1o-3v.inr
    "a-1o-3v.inr" : ["a-1o-3v.inr",  2, (537,537,3), (100,150,slice(None)), 
        np.array((0.760784328, 0.3098039329, 0.3215686381)) ],

    # par a-1o-4v.inr
    # tpr -iy 101 -ix 601 -y 1 -x 4 -f='%.10g' a-1o-4v.inr
    "a-1o-4v.inr" : ["a-1o-4v.inr",  2, (537,537,4), (100,150,slice(None)), 
        np.array((0.760784328, 0.3098039329, 0.3215686381, 1)) ],

    # par a-1o-indexed.inr
    # tpr -iy 101 -ix 101 -y 1 -x 1 -f='%.10g' a-1o-indexed.inr
    "a-1o-indexed.inr" : ["a-1o-indexed.inr",  2, (537,537), (100,100), 0.01960784383 ],

    # par a-1o-256x256.inr
    # tpr -iy 101 -ix 101 -y 1 -x 1 -f='%.10g' a-1o-256x256.inr
    "a-1o-256x256.inr" : ["a-1o-256x256.inr", 2, (256,256), (100,100), 0.4431372583],

# Codages multiplans

    # par a-1o-3z.inr
    # tpr -iz 2 -iy 51 -ix 81 -z 1 -y 1 -x 1 -f='%.10g' a-1o-3z.inr
    "a-1o-3z.inr" : ["a-1o-3z.inr", 3, (3,537,537), (1,50,80), 0.2470588237 ],

    # par a-1o-nz.inr
    # tpr -iz 13 -iy 51 -ix 81 -z 1 -y 1 -x 1 -f='%.10g' a-1o-nz.inr
    "a-1o-nz.inr" : ["a-1o-nz.inr", 3, (37,300,300), (12,50,80), 0.2352941185 ],

    # par a-1o-3v-nz.inr
    # tpr -iz 16 -iy 16 -ix 271 -z 1 -y 1 -x 3 -f='%.10g' a-1o-3v-nz.inr
    "a-1o-3v-nz.inr" : ["a-1o-3v-nz.inr", 3, (37,300,300,3), (15,15,90,slice(None)), 
        np.array((0.2392156869,0,0)) ],

# Codage virgule flottante
# ------------------------

    # par a-4o-r.inr
    # tpr -iy  10 -ix  30 -y 1 -x 1 a-4o-r.inr -f="%.10g
    "a-4o-r.inr" : ["a-4o-r.inr",  2, (537,537), (9,29), 0.6455477476 ],

    # par a-4o-r-3z.inr
    # tpr -iz 2 -iy 51 -ix 101 -z 1 -y 1 -x 1 a-4o-r-3z.inr
    "a-4o-r-3z.inr" : ["a-4o-r-3z.inr",  3, (3,220,279), (1,50,100), 53],

    # par a-8o-r-3v.inr
    # tpr -iz 2 -iy 51 -ix 101 -z 1 -y 1 -x 1 a-8o-r-3v.inr
    "a-8o-r-3v.inr" : ["a-8o-r-3v.inr",  2, (3,220,279), (1,50,100), 53],

    # par a-4o-r-2v.vel
    # tpr -iz 11 -iy 101 -ix 201 -z 1 -y 1 -x 2 a-4o-r-2v.vel
    "a-4o-r-2v.vel" : ["a-4o-r-2v.vel",  3, (37,300,300,2), (10,100,100,slice(None)), np.array((-0.0384942, -0.0153759)) ],

    # par a-8o-r-3v.inr
    # tpr -ix 101 -iy 51 -iz 2 -z 1 -y 1 -x 1 a-8o-r-3v.inr
    "a-8o-r-3v.inr" : ["a-8o-r-3v.inr",  3, (3,220,279), (2,10,200), 572],

# Codage par bits packee (virgule fixe)
# -------------------------------------

    # par bits/a-p4-indexed.inr 
    # tpr -iy 21 -ix 11 -y 1 -x 1 -f='%.10g' bits/a-p4-indexed.inr 
    "bits/a-p4-indexed.inr" : ["bits/a-p4-indexed.inr",  2, (43,73) , (20,10), 0.400000006],

    # par bits/a-b1.inr
    # tpr -iy 21 -ix 11 -y 1 -x 1 -f='%.10g' bits/a-b1.inr
    "bits/a-b1.inr" :  ["bits/a-b1.inr" , 2, (256,256), (20,10), 1],

}

