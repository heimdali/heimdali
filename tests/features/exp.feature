Feature: exp

    Scenario: Invoke exp
        When I run the command: exp --help
        Then I see the line in standard output: Compute exponential of each pixel

    Scenario: Compute square root of each pixel
        When I run the command: exp $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_lo.h5 -o exp_out.h5
        Then images exp_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 are almost equal with the relative parameter 1.E-5
