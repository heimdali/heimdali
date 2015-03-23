Feature: extg

    Scenario: Invoke extg
        When I run the command: extg --help
        Then I see the line in standard output: Extract image subregion

    Scenario: Extract a image subregion
        When I run the command: extg -ix 1 -iy 1 -iz 1 -x 2 -y 2 -z 2 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> imtest_z5_y4_x3_c2_extg.<ext>  
        Then images imtest_z5_y4_x3_c2_extg.<ext> and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_extg.<ext> are equal
    Examples:
        | ext |
        | h5  |
        | inr |

    Scenario: Extract a image subregion and component range
        When I run the command: extg -ix 1 -iy 1 -iv 1 -iz 1 -x 2 -y 2 -z 2 -v 1 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> imtest_z5_y4_x3_c2_extg_v1.<ext>  
        Then images imtest_z5_y4_x3_c2_extg_v1.<ext> and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_extg_v1.<ext> are equal
    Examples:
        | ext |
        | h5  |
        | inr |

    Scenario: Read from stdin
        When I run the command: cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 | extg -ix 1 -iy 1 -iv 1 -iz 1 -x 2 -y 2 -z 2 -v 1 - imtest_z5_y4_x3_c2_extg_v1_stdin.h5
        Then images imtest_z5_y4_x3_c2_extg_v1_stdin.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_extg_v1.h5 are equal
