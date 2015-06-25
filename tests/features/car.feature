Feature: car

    Scenario: Invoke car
        When I run the command: car --help
        Then I see the line in standard output: Compute square of each pixel

    Scenario: Compute square of each pixel
        When I run the command: car $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_ra.<ext> imtest_z5_y4_x3_c2.<ext>
        Then images imtest_z5_y4_x3_c2.<ext> and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> are almost equal with the relative parameter 1.E-5
    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Read from standard input and write to standard output
        When I run the command: cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_ra.h5 | car > imtest_z5_y4_x3_c2_stdin.h5
        Then images imtest_z5_y4_x3_c2_stdin.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 are almost equal with the relative parameter 1.E-5

    Scenario: Compute square of each pixel of a fixed-point image
        When I run the command: car $HEIMDALI_DATA_DIR/three_values_f1.<ext> | tpr -c
        Then I see the standard output:
        """
        0 0.248043 1 
        ""
        """
    Examples:
      | ext |
      | h5  |
      | inr |
