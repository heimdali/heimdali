Feature: The 'max' command line tool.

    Scenario: Invoke max
        When I run the command: max --help
        Then I see the line in standard output: Maximum of two images

    Scenario: Maximum of two images
        When I run the command: max -o max_out.h5 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi-1000_pow2.h5
        Then the HDF5 files max_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi-1000_pow2_max_with_orig.h5 are equal
