Feature: itest

    Scenario: Invoke itest
        When I run the command: itest A --help
        Then I see the line in standard output: Perform simple tests on image informations 

    Scenario: Check one image dimensions 
        When I run the command: itest -z 5 -y 4 -x 3 -v 2 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.inr
        Then I see the standard output:
            """
            Image informations are the same.
            ""
            """
        When I run the command (with return code 1): itest -z 5 -y 7 -x 3 -v 2 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.inr
        Then I see the standard output:
            """
            Image informations are different: nrows
            ""
            """

    Scenario: Check one image number of byte per pixel component
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
            Image informations are different: componentsize
            ""
            """

    Scenario: Check one image image type (fixed point or floating point)
        When I run the command: itest -f $HEIMDALI_DATA_DIR/lena_f2.inr
        Then I see the standard output:
            """
            Image informations are the same.
            ""
            """
        When I run the command (with return code 1): itest -f $HEIMDALI_DATA_DIR/lena_r4.inr
        Then I see the standard output:
            """
            Image informations are different: fixed-point
            ""
            """
        When I run the command: itest -r $HEIMDALI_DATA_DIR/lena_r4.inr
        Then I see the standard output:
            """
            Image informations are the same.
            ""
            """
        When I run the command (with return code 1): itest -r $HEIMDALI_DATA_DIR/lena_f2.inr
        Then I see the standard output:
            """
            Image informations are different: floating-point
            ""
            """

    Scenario: Compare two images dimensions
        When I run the command: itest -z -y -x -v $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.inr $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi1.inr
        Then I see the standard output:
            """
            Image informations are the same.
            ""
            """
        When I run the command (with return code 1): itest -z -y -x -v $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.inr $HEIMDALI_DATA_DIR/lena_r4.inr
        Then I see the standard output:
            """
            Image informations are different: ncolumns ncomponents nplanes nrows
            ""
            """

    Scenario: Read from stdin (check one image dimensions)
        When I run the command: cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 | itest -z 5 -y 4 -x 3 -v 2
        Then I see the standard output:
            """
            Image informations are the same.
            ""
            """
        When I run the command (with return code 1):  cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 | itest -z 5 -y 7 -x 3 -v 2
        Then I see the standard output:
            """
            Image informations are different: nrows
            ""
            """
