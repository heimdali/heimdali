Feature: The 'sc' command line tool.

    Scenario: Invoke sc
        When I run the command: sc --help
        Then I see the line in standard output: Multiply each pixel by NUMBER

    Scenario: Multiply each pixel by 1.5
        When I run the command: sc -n 1.5 -i $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 -o sc_out.h5
        Then the HDF5 files sc_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_sc1.5.h5 are equal
