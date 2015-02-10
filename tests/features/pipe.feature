Feature: Chain commands with unix pipes.

    Scenario: Read image from stdin
        When I run the command: cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 | ical -
        Then I see the standard output:
            """
            +1.0000000E+00 +2.1610000E+03 +4.3210000E+03
            ""
            """

# so input0.h5 input1.h5 | bi -n 1 | sc -n 2 > output.inr
