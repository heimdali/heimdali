Feature: ra

    Scenario: Invoke ra
        When I run the command: ra --help
        Then I see the line in standard output: Compute square root of each pixel

    Scenario: Compute square root of each pixel
        When I run the command: ra $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> imtest_z5_y4_x3_c2_ra.<ext>
        Then images imtest_z5_y4_x3_c2_ra.<ext> and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_ra.<ext> are equal
    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Read from standard input and write to standard output
        When I run the command: cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 | ra > imtest_z5_y4_x3_c2_ra_stdin.h5
        Then images imtest_z5_y4_x3_c2_ra_stdin.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_ra.h5 are equal

    Scenario: Compute square root of each pixel of fixed-point image
        When I run the command: ra $HEIMDALI_DATA_DIR/three_values_f1.<ext> | tpr -c
        Then I see the standard output:
        """
        0 0.705719 1
        ""
        """
    Examples:
      | ext |
      | h5  |
      | inr |
