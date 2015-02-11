Feature: Change pixel type of image

    Scenario: Invoke cco
        When I run the command: cco --help
        Then I see the line in standard output: Change pixel type of image

    Scenario: Change pixel to from float to int, and back to float
        When I run the command: cco $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 cco_tmp0.h5
        When I run the command: cco cco_tmp0.h5 cco_tmp1.h5
        Then the HDF5 files cco_tmp1.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 are equal
