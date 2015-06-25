Feature: mo

    Scenario: Invoke mo
        When I run the command: mo --help
        Then I see the line in standard output: Compute absolute value of IMAGE-IN

    Scenario: Compute absolute value of an image
        When I run the command: mo $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi-1000.<ext> imtest_z5_y4_x3_c2_bi-1000_mo.<ext>
        Then images imtest_z5_y4_x3_c2_bi-1000_mo.<ext> and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi-1000_mo.<ext> are equal
    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Read from standard input and write to standard output
        When I run the command: cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi-1000.h5 | mo > imtest_z5_y4_x3_c2_bi-1000_mo_stdin.h5
        Then images imtest_z5_y4_x3_c2_bi-1000_mo_stdin.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi-1000_mo.h5 are equal

    Scenario: Compute absolute value of an fixed-point image
        When I run the command: mo $HEIMDALI_DATA_DIR/three_values_f1.<ext> | tpr -c
        Then I see the standard output:
        """
        0 0.498039 1
        ""
        """
    Examples:
      | ext |
      | h5  |
      | inr |
