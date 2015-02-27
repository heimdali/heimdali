Feature: sd

    Scenario: Invoke sd
        When I run the command: sd --help
        Then I see the line in standard output: Divide each pixel by NUMBER

    Scenario: Divide each pixel by 1.5
        When I run the command: sd -n 1.5 -o sd_out.h5 -i $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_sc1.5.h5
        Then images sd_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 are equal
