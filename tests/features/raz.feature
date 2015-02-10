Feature: The 'raz' command line tool.

    Scenario: Invoke raz
        When I run the command: raz --help
        Then I see the line in standard output: Fill an image with zero values.

    Scenario: Set all pixel of an image to zero
        When I run the command: cp $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 raz_out.h5
        When I run the command: raz raz_out.h5
        Then the HDF5 files raz_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_zero.h5 are equal
