Feature: The 'mu' command line tool.

    Scenario: Invoke mu
        When I run the command: mu --help
        Then I see the line in standard output: Multiply two images

    Scenario: Multiply to images
        When I run the command: mu $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_sc1.5.h5 -o mu_out.h5
        Then the HDF5 files mu_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_pow2_sc1.5.h5 are equal
