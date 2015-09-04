Feature: tpr

    Scenario: Invoke tpr
        When I run the command: tpr --help
        Then I see the line in standard output: Print the pixel values of a image subregion

    Scenario: Print a full image
        When I run the command: tpr --from-zero $HEIMDALI_DATA_DIR/<input>
        Then I see as standard output the content of the file $HEIMDALI_DATA_DIR/<stdout>
    Examples:
        | input                     | stdout            |
        | cossin_z5_y5_x5_c2_r4.h5  | tpr_cossin_r4.txt |
        | cossin_z5_y5_x5_c2_r4.inr | tpr_cossin_r4.txt |
        | cossin_z5_y5_x5_c2_f2.h5  | tpr_cossin_f2.txt |
        | cossin_z5_y5_x5_c2_f2.inr | tpr_cossin_f2.txt |

    Scenario: Print a full image, counting from 1
        When I run the command: tpr $HEIMDALI_DATA_DIR/cossin_z5_y5_x5_c2_r4.h5  
        Then I see as standard output the content of the file $HEIMDALI_DATA_DIR/tpr_cossin_r4_from_1.txt 

    Scenario: Print a full image, reading from stdin
        When I run the command: cat $HEIMDALI_DATA_DIR/<input> | tpr --from-zero
        Then I see as standard output the content of the file $HEIMDALI_DATA_DIR/<stdout>
    Examples:
        | input                     | stdout            |
        | cossin_z5_y5_x5_c2_r4.h5  | tpr_cossin_r4.txt |
        | cossin_z5_y5_x5_c2_f2.h5  | tpr_cossin_f2.txt |
        | lena_r4.h5                | tpr_lena_r4.txt   |

    Scenario: Print a image region
       When I run the command: tpr --from-zero -ix 1 -iy 1 -iz 1 -x 2 -y 2 -z 2  $HEIMDALI_DATA_DIR/<input>
       Then I see as standard output the content of the file $HEIMDALI_DATA_DIR/<stdout>
   Examples:
       | input                     | stdout                  |
       | cossin_z5_y5_x5_c2_r4.h5  | tpr_cossin_r4_region.txt |
       | cossin_z5_y5_x5_c2_r4.inr | tpr_cossin_r4_region.txt |
       | cossin_z5_y5_x5_c2_f2.h5  | tpr_cossin_f2_region.txt |
       | cossin_z5_y5_x5_c2_f2.inr | tpr_cossin_f2_region.txt |

    Scenario: Print a image region, counting from 1
       When I run the command: tpr -ix 2 -iy 2 -iz 2 -x 2 -y 2 -z 2  $HEIMDALI_DATA_DIR/cossin_z5_y5_x5_c2_r4.inr 
       Then I see as standard output the content of the file $HEIMDALI_DATA_DIR/tpr_cossin_r4_region_from_1.txt 

    Scenario: Print a 3D image region, reading from stding
       When I run the command: cat $HEIMDALI_DATA_DIR/<input> | tpr --from-zero -ix 1 -iy 1 -iz 1 -x 2 -y 2 -z 2  
       Then I see as standard output the content of the file $HEIMDALI_DATA_DIR/<stdout>
   Examples:
       | input                     | stdout                   |
       | cossin_z5_y5_x5_c2_r4.h5  | tpr_cossin_r4_region.txt |
       | cossin_z5_y5_x5_c2_f2.h5  | tpr_cossin_f2_region.txt |

    Scenario: Print a 2D image region, reading from stding
       When I run the command: cat $HEIMDALI_DATA_DIR/<input> | tpr --from-zero -ix 1 -iy 1 -x 2 -y 2
       Then I see as standard output the content of the file $HEIMDALI_DATA_DIR/<stdout>
    Examples:
       | input                     | stdout                   |
       | lena_r4.h5                | tpr_lena_r4_region.txt   |

    Scenario: Print fixed-point format for unsigned char
        When I run the command: tpr --from-zero <flag> -x 1 -y 1 -c $HEIMDALI_DATA_DIR/<input>
        Then I see the standard output:
            """
            <output>
            ""
            """
    Examples:
        | flag    | input       | output      |
        |         | lena_f1.inr | 165         |
        |         | lena_f1.h5  | 165         |
        | -f "%d" | lena_f1.inr | 165         |
        | -f "%d" | lena_f1.h5  | 165         |
        | -f "%g" | lena_f1.inr | 0.647059    |
        | -f "%g" | lena_f1.h5  | 0.647059    |
        |         | lena_f2.inr | 0.647059    |
        |         | lena_f2.h5  | 0.647059    |
        | -f "%g" | lena_f2.inr | 0.647059    |
        | -f "%g" | lena_f2.h5  | 0.647059    |

    Scenario: Print fixed point image with -f %u
        When I run the command: tpr -f %u -c $HEIMDALI_DATA_DIR/<input>
        Then I see the standard output:
            """
            <output>
            ""
            """
    Examples:
        | input                 | output                  | 
        | zero_half_one_f_o1.h5 | 0 128 255               |
        | zero_half_one_f_o2.h5 | 0 32768 65535           |
        | zero_half_one_f_o4.h5 | 0 2147483648 4294967295 |
