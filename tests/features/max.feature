Feature: max

    Scenario: Invoke max
        When I run the command: max --help
        Then I see the line in standard output: Maximum of two images

    Scenario: Maximum of two images
        When I run the command: max $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi-1000_pow2.<ext> imtest_z5_y4_x3_c2_bi-1000_pow2_max_with_orig.<ext> 
        Then images imtest_z5_y4_x3_c2_bi-1000_pow2_max_with_orig.<ext> and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi-1000_pow2_max_with_orig.<ext> are equal
    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Read from standard input and write to standard output
        When I run the command:  cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 | max $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi-1000_pow2.h5 > imtest_z5_y4_x3_c2_bi-1000_pow2_max_with_orig_stdin.h5 
        Then images imtest_z5_y4_x3_c2_bi-1000_pow2_max_with_orig_stdin.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi-1000_pow2_max_with_orig.h5 are equal

    Scenario: Maximum of two fixed-point images
        When I run the command: max $HEIMDALI_DATA_DIR/three_values_f1.<ext> $HEIMDALI_DATA_DIR/three_values_f1.<ext> | tpr -c
        Then I see the standard output:
        """
        0 0.498039 1
        ""
        """
    Examples:
      | ext |
      | h5  |
      | inr |

