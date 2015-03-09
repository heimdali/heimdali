Feature: logic

    Scenario: Invoke logic
        When I run the command: logic --help
        Then I see the line in standard output: Boolean operation on image

    Scenario: Binary operation on two images
        When I run the command: logic <flag> $HEIMDALI_DATA_DIR/<input0> $HEIMDALI_DATA_DIR/<input1> <output>
        Then images <output> and $HEIMDALI_DATA_DIR/<output> are equal

    Examples:
    | input0         | input1                 | flag | output             |
    | boolean_f1.h5  | boolean_another_f1.h5  | -et  | boolean_and_f1.h5  |
    | boolean_f1.inr | boolean_another_f1.inr | -et  | boolean_and_f1.inr |
    | boolean_f1.h5  | boolean_another_f1.h5  | -ou  | boolean_or_f1.h5   |
    | boolean_f1.inr | boolean_another_f1.inr | -ou  | boolean_or_f1.inr  |
    | boolean_f1.h5  | boolean_another_f1.h5  | -xou | boolean_xor_f1.h5  |
    | boolean_f1.inr | boolean_another_f1.inr | -xou | boolean_xor_f1.inr |
