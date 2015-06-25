Feature: lo

    Scenario: Invoke lo
        When I run the command: lo --help
        Then I see the line in standard output: Compute logarithm of each pixel

    Scenario: Compute logarithm of each pixel
        When I run the command: lo $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> imtest_z5_y4_x3_c2_lo.<ext>
        Then images imtest_z5_y4_x3_c2_lo.<ext> and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_lo.<ext> are almost equal with the relative parameter 1.E-5
    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Read from standard input and write to standard output
        When I run the command: cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 | lo > imtest_z5_y4_x3_c2_lo_stdin.h5
        Then images imtest_z5_y4_x3_c2_lo_stdin.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_lo.h5 are almost equal with the relative parameter 1.E-5

    Scenario: Compute logarithm of each pixel of fixed-point image
        When I run the command: lo $HEIMDALI_DATA_DIR/three_values_f1.<ext> | tpr -c
        Then I see the standard output:
        """
        -inf -0.697076 0
        ""
        """
    Examples:
      | ext |
      | h5  |
      | inr |
