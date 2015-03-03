Feature: ical

    Scenario: Invoke ical
        When I run the command: ical --help
        Then I see the line in standard output: Compute min, max and mean of images

    Scenario: Compute min, mean, max of a floating point (float) image
        When I run the command: ical $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5
        Then I see the standard output:
            """
            +1.0000000E+00 +2.1610000E+03 +4.3210000E+03
            ""
            """

    Scenario: Compute min, mean, max of a fixed point (unsigned char) image
        When I run the command: ical $HEIMDALI_DATA_DIR/lena_f1.<ext>
        Then I see the standard output:
            """
            +1.1764706E-01 +4.9966636E-01 +9.3725491E-01
            ""
            """
    Examples:
        | ext |
        | h5  |
        | inr |

    Scenario: Read from stdin
        When I run the command: ad $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> | ical -
        Then I see the standard output:
            """
            +2.0000000E+00 +4.3220000E+03 +8.6420000E+03
            ""
            """
        When I run the command: ad $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> | ical
        Then I see the standard output:
            """
            +2.0000000E+00 +4.3220000E+03 +8.6420000E+03
            ""
            """
    Examples:
        | ext |
        | h5  |
        | inr |
