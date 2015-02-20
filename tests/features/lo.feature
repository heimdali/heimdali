Feature: lo

    Scenario: Invoke lo
        When I run the command: lo --help
        Then I see the line in standard output: Compute logarithm of each pixel

    Scenario: Compute logarithm of each pixel
        When I run the command: lo $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 -o lo_out.h5
        Then the HDF5 files lo_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_lo.h5 are almost equal with the relative parameter 1.E-5
