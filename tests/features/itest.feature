Feature: itest

    Scenario: Invoke itest
        When I run the command: itest --help
        Then I see the line in standard output: Perform simple tests on image informations 

    Scenario: Check image dimensions are as expected
        When I run the command: itest -z 5 -y 4 -x 3 -v 2 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.inr
        Then I see the standard output:
            """
            Image informations are the same.
            ""
            """

    Scenario: Check image dimensions are not as expected
        When I run the command (with return code 1): itest -z 5 -y 7 -x 3 -v 2 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.inr
        Then I see the standard output:
            """
            Image informations are different.
            Number of rows are different: 4 VS 7 .
            ""
            """
