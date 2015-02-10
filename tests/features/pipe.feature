Feature: Chain commands with unix pipes.

    Scenario: ical reads from stdin
        When I run the command: cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 | ical -
        Then I see the standard output:
            """
            +1.0000000E+00 +2.1610000E+03 +4.3210000E+03
            ""
            """

    Scenario: bi reads from stdin
        When I run the command: cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 | bi -n 1 > pipe0_out.h5
        Then the HDF5 files pipe0_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi1.h5 are equal

    Scenario: Read from previous command output
        When I run the command: so $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 | bi -n 1 | sc -n 2 > pipe1_out.h5
        Then the HDF5 files pipe1_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_two.h5 are equal
