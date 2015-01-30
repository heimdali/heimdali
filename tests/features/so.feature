Feature: The 'so' command line tool.

    Scenario: Invoke so
        When I run the command: so --help
        Then I see the line in standard output: Substract two images

    Scenario: Substract two images
        When I run the command: so -o tmp.h5 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5
        Then I run the command: h5diff --exclude-path /HDFVersion --exclude-path /ITKVersion tmp.h5 $HEIMDALI_DATA_DIR/mtest_z5_y4_x3_c2_zero.h5
