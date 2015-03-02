Feature: cco

    Scenario: Invoke cco
        When I run the command: cco --help
        Then I see the line in standard output: Change pixel type of image

    Scenario: Convert fixed point (unsigned char) to floating point (float)
        When I run the command: cco -o 4 -r $HEIMDALI_DATA_DIR/<input> <output>
        Then images <output> and $HEIMDALI_DATA_DIR/<output> are equal
    Examples:
    | input       |  output      |
    | lena_f1.inr |  lena_r4.inr |
    | lena_f1.h5  |  lena_r4.h5  |

    Scenario: Convert floating point (float) to fixed point (unsigned char)
        When I run the command: cco -o 1 -f $HEIMDALI_DATA_DIR/<input> <output>
        Then images <output> and $HEIMDALI_DATA_DIR/<output> are equal
    Examples:
    | input       |  output      |
    | lena_r4.inr |  lena_f1.inr |
    | lena_r4.h5  |  lena_f1.h5  |

    Scenario: Convert floating point (float) to fixed point (unsigned char), values greater than 1 are casted to 1
        When I run the command: cco -o 1 -f $HEIMDALI_DATA_DIR/<input> <output>
        Then images <output> and $HEIMDALI_DATA_DIR/<output> are equal
    Examples:
    | input             |  output               |
    | lena_r4_sc8.5.inr |  lena_r4_sc8.5_f1.inr |
    #| lena_r4_sc8.5.h5 |  lena_r4_sc8.5_f1.h5  |

