Feature: so

    Scenario: Invoke so
        When I run the command: so --help
        Then I see the line in standard output: Subtract two images

    Scenario: Subtract two images
        When I run the command: so $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 imtest_z5_y4_x3_c2_zero.h5 
        Then images imtest_z5_y4_x3_c2_zero.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_zero.h5 are equal

    Scenario: Read from standard input and write to standard output, first case
        When I run the command: cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 | so $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_sc3.h5 > imtest_z5_y4_x2_c2_sc2_stdin0.h5
        Then images imtest_z5_y4_x2_c2_sc2_stdin0.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_sc2.h5 are equal

    Scenario: Read from standard input and write to standard output, second case
        When I run the command: cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_sc3.h5 | so - $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 > imtest_z5_y4_x2_c2_sc2_stdin1.h5
        Then images imtest_z5_y4_x2_c2_sc2_stdin1.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_sc2.h5 are equal

    Scenario: Read from standard input and write to standard output, third case
        When I run the command: cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 | so $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_sc3.h5 - > imtest_z5_y4_x2_c2_sc2_stdin2.h5
        Then images imtest_z5_y4_x2_c2_sc2_stdin2.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_sc2.h5 are equal
