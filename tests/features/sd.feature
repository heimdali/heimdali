Feature: sd

    Scenario: Invoke sd
        When I run the command: sd --help
        Then I see the line in standard output: Divide each pixel by NUMBER

    Scenario: Divide each pixel by 1.5
        When I run the command: sd -n 1.5 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_sc1.5.<ext> imtest_z5_y4_x3_c2.<ext> 
        Then images imtest_z5_y4_x3_c2.<ext> and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> are equal
    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Read from standard input and write to standard output
        When I run the command: cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_sc1.5.h5 | sd -n 1.5 > imtest_z5_y4_x3_c2_stdin.h5 
        Then images imtest_z5_y4_x3_c2_stdin.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 are equal
