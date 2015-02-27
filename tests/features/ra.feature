Feature: ra

    Scenario: Invoke ra
        When I run the command: ra --help
        Then I see the line in standard output: Compute square root of each pixel

    Scenario: Compute square root of each pixel
        When I run the command: ra $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 -o ra_out.h5
        Then images ra_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_ra.h5 are equal
