Feature: The 'ad' command line tool.

    Scenario: Invoke ad
        When I run the command: ad --help
        Then I see the line in standard output: Add two images

    Scenario: Add two images
        When I run the command: ad -o ad_out.h5 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_two.h5
        Then the HDF5 files ad_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_ad2.h5 are equal
