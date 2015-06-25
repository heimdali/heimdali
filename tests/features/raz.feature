Feature: raz

    Scenario: Invoke raz
        When I run the command: raz --help
        Then I see the line in standard output: Fill an image with zero values.

    Scenario: Set all pixel of an image to zero
        When I run the command: cp $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 raz_out.h5
        When I run the command: raz raz_out.h5
        Then images raz_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_zero.h5 are equal

    Scenario: Write new image with 2 components per pixel
        When I run the command: raz -x 2 -y 3 -z 4 -v 2 zero.h5
        When I run the command: par zero.h5
        Then I see the standard output:
            """
            "zero.h5 -x 2	-y 3	-z 4	-v 2	-o 4	-r"
            ""
            """

    Scenario: Write new double image
        When I run the command: raz -x 2 -y 3 -z 4 -r -o 8 zero_r8.h5
        When I run the command: par zero_r8.h5
        Then I see the standard output:
            """
            "zero_r8.h5 -x 2	-y 3	-z 4	-o 8	-r"
            ""
            """

    Scenario: Write new image to standard ouptut
        When I run the command: raz -x 2 -y 3 -z 4  | par
        Then I see the standard output:
            """
            "< -x 2	-y 3	-z 4	-o 4	-r"
            ""
            """

    Scenario: Write new fixed point type image to starndard output
        When I run the command: raz -x 2 -y 3 -z 4 -f | par
        Then I see as standard output the line: < -x 2	-y 3	-z 4	-o 1	-f

    Scenario: Write new fixed point (size 2) type image to starndard output
        When I run the command: raz -x 2 -y 3 -z 4 -f -o 2 | par
        Then I see as standard output the line: < -x 2	-y 3	-z 4	-o 2	-f

    Scenario: Write new fixed point (size 2) type image to starndard output
        When I run the command: raz -x 2 -y 3 -z 4 -f -o 4 | par
        Then I see as standard output the line: < -x 2	-y 3	-z 4	-o 4	-f
