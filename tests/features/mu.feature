Feature: mu

    Scenario: Invoke mu
        When I run the command: mu --help
        Then I see the line in standard output: Multiply two images

    Scenario: Multiply to images
        When I run the command: mu $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_sc1.5.<ext> imtest_z5_y4_x3_c2_pow2_sc1.5.<ext>
        Then images imtest_z5_y4_x3_c2_pow2_sc1.5.<ext> and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_pow2_sc1.5.<ext> are equal
    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Read from standard input and write to standard output
        When I run the command: cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 | mu $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_sc1.5.h5 > imtest_z5_y4_x3_c2_pow2_sc1.5_stdin.h5
        Then images imtest_z5_y4_x3_c2_pow2_sc1.5_stdin.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_pow2_sc1.5.h5 are equal
