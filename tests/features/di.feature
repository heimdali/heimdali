Feature: di

    Scenario: Invoke di
        When I run the command: di --help
        Then I see the line in standard output: Divide two images

    Scenario: Divide to images
        When I run the command: di $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_two.h5 di_out.h5 
        Then images di_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_di2.h5 are equal
