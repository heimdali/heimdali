Feature: sc

    Scenario: Invoke sc
        When I run the command: sc --help
        Then I see the line in standard output: Multiply each pixel by NUMBER

    Scenario: Multiply each pixel by <factor>
        When I run the command: sc -n <factor> -i $HEIMDALI_DATA_DIR/<input> -o <output>
        Then images <output> and $HEIMDALI_DATA_DIR/<output> are equal

    Examples:
    | input                 | factor | output                      |
    | imtest_z5_y4_x3_c2.h5 | 1.5    | imtest_z5_y4_x3_c2_sc1.5.h5 |
    | lena_f1.inr           | 8.5    | lena_r4_sc8.5.inr           |
    | lena_f1.h5            | 8.5    | lena_r4_sc8.5.h5            |
