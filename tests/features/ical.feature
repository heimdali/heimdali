Feature: The 'ical' command line tool.

    Scenario: Invoke ical
        When I run the command: ical --help
        Then I see the line in standard output: Compute min, max and mean of images

    Scenario: Compute min, mean, max of an image
        When I run the command: ical $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5
        Then I see the standard output:
            """
            +1.0000000E+00 +2.1610000E+03 +4.3210000E+03
            ""
            """

