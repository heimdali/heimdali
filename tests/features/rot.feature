Feature: rot

    Scenario: Invoke rot
        When I run the command: rot --help
        Then I see the line in standard output: z-axis rotation and translation

    Scenario: Rotate image 45 degree
        When I run the command: rot -a 45 -x 256 -y 256 $HEIMDALI_DATA_DIR/lena_f1.<ext> lena_f1_rot45.<ext>
        Then images lena_f1_rot45.<ext> and $HEIMDALI_DATA_DIR/lena_f1_rot45.<ext> are equal
        #When I run the command: rot -a 45 -x 256 -y 256 lena_f1_rot45.<ext> lena_f1_rot_45_45.<ext>
        #Then images lena_f1_rot_45_45.<ext> and $HEIMDALI_DATA_DIR/lena_f1_rot90.<ext> are equal
    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Rotate image 90 degree
        When I run the command: rot -a 90 -x 256 -y 256 $HEIMDALI_DATA_DIR/lena_f1.<ext> lena_f1_rot90.<ext>
        Then images lena_f1_rot90.<ext> and $HEIMDALI_DATA_DIR/lena_f1_rot90.<ext> are equal
    Examples:
      | ext |
      | h5  |
      | inr |
