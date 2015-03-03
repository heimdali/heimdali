Feature: min

    Scenario: Invoke min
        When I run the command: min --help
        Then I see the line in standard output: Minimum of two images

    Scenario: Minimum of two images
        When I run the command: min $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi-1000_pow2.<ext> imtest_z5_y4_x3_c2_bi-1000_pow2_min_with_orig.<ext> 
        Then images imtest_z5_y4_x3_c2_bi-1000_pow2_min_with_orig.<ext> and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi-1000_pow2_min_with_orig.<ext> are equal
    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Read from standard input and write to standard output
        When I run the command:  cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 | min $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi-1000_pow2.h5 > imtest_z5_y4_x3_c2_bi-1000_pow2_min_with_orig_stdin.h5 
        Then images imtest_z5_y4_x3_c2_bi-1000_pow2_min_with_orig_stdin.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi-1000_pow2_min_with_orig.h5 are equal
