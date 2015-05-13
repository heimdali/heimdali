Feature: raz

    Scenario: Invoke raz
        When I run the command: raz --help
        Then I see the line in standard output: Fill an image with zero values.

    Scenario: Set all pixel of an image to zero
        When I run the command: cp $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 raz_out.h5
        When I run the command: raz raz_out.h5
        Then images raz_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_zero.h5 are equal

    Scenario: Write to standard ouptut
        When I run the command: raz -x 2 -y 3 -z 4  | par
        Then I see the standard output:
            """
            " -x 2	-y 3	-z 4	-o 4	-r"
            ""
            """
