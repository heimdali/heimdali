Feature: par

    Scenario: Invoke par
        When I run the command: par --help
        Then I see the line in standard output: List image formats

    Scenario: Print all parameters
        When I run the command: par $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext>
        Then I see the standard output:
            """
            $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> -x 3	-y 4	-z 5	-v 2	-o 4	-r
            ""
            """
    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Print one parameter
        When I run the command: par $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 -x
        Then I see the standard output:
            """
            " -x 3"
            """

    Scenario: Print 2D image parameters
        When I run the command: par $HEIMDALI_DATA_DIR/lena_f2.inr
        Then I see the standard output:
            """
            $HEIMDALI_DATA_DIR/lena_f2.inr -x 256	-y 256	-o 2	-f
            ""
            """

    Scenario: Print 2D image parameters, force -z to be printed
        When I run the command: par $HEIMDALI_DATA_DIR/lena_f2.inr -z
        Then I see the standard output:
            """
            " -z 1"
            """

    Scenario: Print file type
        When I run the command: par $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> -F
        Then I see the standard output:
            """
            " -F <filetype>"
            """
    Examples:
      | ext | filetype |
      | h5  | HDF5     |
      | inr | Inrimage |

    Scenario: Print parameters of image with origin different than 0
        When I run the command: par $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_origin_4_5_6.<ext>
        Then I see the standard output:
            """
            $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_origin_4_5_6.<ext> -x 3	-y 4	-z 5	-v 2	-x0 4	-y0 5	-z0 6	-o 4	-r
            ""
            """
    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Print parameters of image with one plane and one pixel component
        When I run the command: par $HEIMDALI_DATA_DIR/imtest_z1_y3_x2_c1.<ext>
        Then I see the standard output:
            """
            $HEIMDALI_DATA_DIR/imtest_z1_y3_x2_c1.<ext> -x 2	-y 3	-o 4	-r
            ""
            """
    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Read from stdin
        When I run the command: ad $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> | par -x -
        Then I see the standard output:
            """
            " -x 3"
            """
    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Read from stdin
        When I run the command: ad $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> | par -x
        Then I see the standard output:
            """
            " -x 3"
            """
    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Try read read inexistant files
        When I run the command (with return code 1): par no_such_0 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.inr no_such_1
        Then I see the standard output:
            """
            $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.inr -x 3	-y 4	-z 5	-v 2	-o 4	-r
            ""
            """
        Then I see on standard error:
            """
            par: ERROR: Can't open file(s) for read: <no_such_0>, <no_such_1>.
            ""
            """
