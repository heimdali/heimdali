Feature: bi

    Scenario: Invoke bi
        When I run the command: bi --help
        Then I see the line in standard output: Add a constant to an image

    Scenario: Add 1 to each pixel
        When I run the command: bi -n 1 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> imtest_z5_y4_x3_c2_bi1.<ext>
        Then images imtest_z5_y4_x3_c2_bi1.<ext> and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi1.<ext> are equal
    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Read from standard input and write to standard output
        When I run the command: cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 | bi -n 1 > imtest_z5_y4_x3_c2_bi1_stdin.h5
        Then images imtest_z5_y4_x3_c2_bi1_stdin.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi1.h5 are equal
