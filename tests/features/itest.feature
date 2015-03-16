Feature: itest

    Scenario: Invoke itest
        When I run the command: itest --help
        Then I see the line in standard output: Perform simple tests on image informations 

    Scenario: Check image dimensions 
        When I run the command: itest -z 5 -y 4 -x 3 -v 2 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.inr
        Then I see the standard output:
            """
            Image informations are the same.
            ""
            """
        When I run the command (with return code 1): itest -z 5 -y 7 -x 3 -v 2 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.inr
        Then I see the standard output:
            """
            Image informations are different.
            Number of rows are different: 4 VS 7 .
            ""
            """

    Scenario: Check number of byte per pixel component
        When I run the command: itest -o 4 $HEIMDALI_DATA_DIR/lena_r4.inr
        Then I see the standard output:
            """
            Image informations are the same.
            ""
            """
        When I run the command: itest -o 2 $HEIMDALI_DATA_DIR/lena_f2.inr
        Then I see the standard output:
            """
            Image informations are the same.
            ""
            """
        When I run the command (with return code 1): itest -o 4 $HEIMDALI_DATA_DIR/lena_f2.inr
        Then I see the standard output:
            """
            Image informations are different.
            Number of bytes of a pixel component are different: 2 VS 4 .
            ""
            """
