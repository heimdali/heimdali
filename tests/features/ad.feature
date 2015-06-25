Feature: ad

    Scenario: Invoke ad
        When I run the command: ad --help
        Then I see the line in standard output: Add two images

    Scenario: Add two images
        When I run the command: ad $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_two.<ext> imtest_z5_y4_x2_c2_ad2.<ext>
        Then images imtest_z5_y4_x2_c2_ad2.<ext> and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_ad2.<ext> are equal
    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Read from standard input and write to standard output
        When I run the command: cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 | ad $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_two.h5 > imtest_z5_y4_x2_c2_sc2_stdin.h5
        Then images imtest_z5_y4_x2_c2_sc2_stdin.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_ad2.h5 are equal

    Scenario: Add two fixed-point images
        When I run the command: ad $HEIMDALI_DATA_DIR/three_values_f1.<ext> $HEIMDALI_DATA_DIR/three_values_f1.<ext> | tpr -c
        Then I see the standard output:
        """
        0 0.996078 2
        ""
        """
    Examples:
      | ext |
      | h5  |
      | inr |

