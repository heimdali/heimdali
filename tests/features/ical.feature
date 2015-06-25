Feature: ical

    Scenario: Invoke ical
        When I run the command: ical --help
        Then I see the line in standard output: Compute min, max and mean of images

    Scenario: Compute min, mean, max of a floating point (float) image
        When I run the command: ical $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5
        Then I see the standard output:
            """
            "             1          2161          4321"
            ""
            """

    Scenario: Compute min, mean, max of a fixed point (unsigned char) image
        When I run the command: ical $HEIMDALI_DATA_DIR/lena_f1.<ext>
        Then I see the standard output:
            """
            "      0.117647      0.499666      0.937255"
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
            "             2          4322          8642"
            ""
            """
        When I run the command: ad $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> | ical
        Then I see the standard output:
            """
            "             2          4322          8642"
            ""
            """
    Examples:
        | ext |
        | h5  |
        | inr |
