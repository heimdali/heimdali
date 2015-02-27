Feature: min

    Scenario: Invoke min
        When I run the command: min --help
        Then I see the line in standard output: Minimum of two images

    Scenario: Minimum of two images
        When I run the command: min -o min_out.h5 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi-1000_pow2.h5
        Then images min_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi-1000_pow2_min_with_orig.h5 are equal
