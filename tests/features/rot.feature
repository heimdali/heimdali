Feature: rot

    Scenario: Invoke rot
        When I run the command: rot --help
        Then I see the line in standard output: z-axis rotation and translation

    Scenario: Rotate image
        When I run the command: rot -a 45 -x 256 -y 256 $HEIMDALI_DATA_DIR/lena_f1.inr lena_f1_rot45.inr
        Then images lena_f1_rot45.inr and $HEIMDALI_DATA_DIR/lena_f1_rot45.inr are equal

