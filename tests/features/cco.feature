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

    Scenario: Convert fixed point (unsigned char) to floating point (float), -o 4 is implicit
        When I run the command: cco -r $HEIMDALI_DATA_DIR/<input> <output>
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

    Scenario: Convert floating point (float) to fixed point (unsigned char), -f -o 1 are implicit
        When I run the command: cco $HEIMDALI_DATA_DIR/<input> <output>
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
    | lena_r4_sc8.5.h5  |  lena_r4_sc8.5_f1.h5  |

    Scenario: Convert fixed point from unsigned char to unsigned short
        When I run the command: cco -o 2 $HEIMDALI_DATA_DIR/<input> <output>
        Then images <output> and $HEIMDALI_DATA_DIR/<output> are equal
    Examples:
    | input             |  output               |
    | lena_f1.inr       |  lena_f2.inr          |
    | lena_f1.h5        |  lena_f2.h5           |

    Scenario: Convert floating point (float) to fixed point (unsigned short)
        When I run the command: cco -o 2 <input> <output>
        Then images <output> and $HEIMDALI_DATA_DIR/<output> are equal
    Examples:
    | input             |  output               |
    | lena_r4.inr       |  lena_f2.inr          |
    | lena_r4.h5        |  lena_f2.h5           |

    Scenario: Convert fixed point (unsigned short) to binary (unsigned char)
        When I run the command: cco -b 8 <input> <output>
        Then images <output> and $HEIMDALI_DATA_DIR/<output> are equal
    Examples:
    | input             |  output               |
    | lena_f2.inr       |  lena_b1_f1.inr       |
    | lena_f2.h5        |  lena_b1_f1.h5        |

    Scenario: Write fixed point to standard output
        When I run the command: cco -f $HEIMDALI_DATA_DIR/lena_r4.h5 lena_f1.h5
        Then images lena_f1.h5 and $HEIMDALI_DATA_DIR/lena_f1.h5 are equal

    Scenario: Read from standard input
        When I run the command: cat $HEIMDALI_DATA_DIR/lena_r4.h5 | cco -f <input> | par
        Then I see the standard output:
            """
            < -x 256	-y 256	-f	-o 1
            ""
            """
        Examples:
        | input |
        |       |
        | -     |


    Scenario: Write to standard output
        When I run the command: cco -r $HEIMDALI_DATA_DIR/<input> | ical
        Then I see the standard output: 
        """
        "      0.117647      0.499666      0.937255"
        ""
        """
    Examples:
    | input       |  output      |
    | lena_f1.inr |  lena_r4.inr |
    | lena_f1.h5  |  lena_r4.h5  |
