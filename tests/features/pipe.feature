Feature: pipe

    Scenario: ical reads from stdin
        When I run the command: cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 | ical -
        Then I see the standard output:
            """
            "             1          2161          4321"
            ""
            """

    Scenario: bi reads from stdin
        When I run the command: cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 | bi -n 1 > pipe0_out.h5
        Then images pipe0_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi1.h5 are equal

    Scenario: Read from previous command output
        When I run the command: so $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 | bi -n 1 | sc -n 2 > pipe1_out.h5
        Then images pipe1_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_two.h5 are equal
