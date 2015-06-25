Feature: exp

    Scenario: Invoke exp
        When I run the command: exp --help
        Then I see the line in standard output: Compute exponential of each pixel

    Scenario: Compute exponential of each pixel
        When I run the command: exp $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_lo.<ext> imtest_z5_y4_x3_c2.<ext>
        Then images imtest_z5_y4_x3_c2.<ext> and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> are almost equal with the relative parameter 1.E-5
    Examples:
      | ext |
      | h5  |
      | inr |
    Scenario: Read from standard input and write to standard output
        When I run the command: cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_lo.h5 | exp > imtest_z5_y4_x3_c2_stdin.h5
        Then images imtest_z5_y4_x3_c2_stdin.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 are almost equal with the relative parameter 1.E-5

    Scenario: Compute exponential of each pixel of a fixed-point image
        When I run the command: exp $HEIMDALI_DATA_DIR/three_values_f1.<ext> | tpr -c
        Then I see the standard output:
        """
        1 1.64549 2.71828 
        ""
        """
    Examples:
      | ext |
      | h5  |
      | inr |
