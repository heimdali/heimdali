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

    Scenario: Read from stdin and write to stdout
        When I run the command: cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 | extg -ix 1 -iy 1 -iv 1 -iz 1 -x 2 -y 2 -z 2 -v 1 > imtest_z5_y4_x3_c2_extg_v1_stdout.h5
        Then images imtest_z5_y4_x3_c2_extg_v1_stdout.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_extg_v1.h5 are equal

    Scenario: Extract a UNSIGNED CHAR image subregion
        When I run the command: extg -x 100 -y 100 $HEIMDALI_DATA_DIR/lena_f1.<ext> lena_f1_extg.<ext>  
        When I run the command: par lena_f1_extg.<ext>
        Then I see the standard output:
            """
            lena_f1_extg.<ext> -x 100	-y 100	-f	-o 1
            ""
            """
    Examples:
        | ext |
        | h5  |
        | inr |

    Scenario: Extract with sample steps.
        When I run the command: extg -iz 5 -z 3 -zpas 2 -iy 4 -y 2 -ypas 3 -ix 3 -x 3 -xpas 3 -iv 1 -v 2 -vpas 5  $HEIMDALI_DATA_DIR/imtest_z15_y14_x13_c12.h5 | tpr -l 6 --from-zero
        Then I see the standard output:
            """
            plane 0, line 0
            5431 5436 5461 5466 5491 5496
            plane 0, line 1
            5731 5736 5761 5766 5791 5796
            plane 1, line 0
            7431 7436 7461 7466 7491 7496
            plane 1, line 1
            7731 7736 7761 7766 7791 7796
            plane 2, line 0
            9431 9436 9461 9466 9491 9496
            plane 2, line 1
            9731 9736 9761 9766 9791 9796
            ""
            """

    Scenario: Extract one in every two pixel
        When I run the command: extg -xpas 2 $HEIMDALI_DATA_DIR/lena_f1.inr | par -x

    #Scenario: Extract with incorrect number of values
        #When I run the command: extg -ix 1 -iy 1 -iz 1 -x 10 -y 10 -z 10 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 imtest_z5_y4_x3_c2_extg.h5
