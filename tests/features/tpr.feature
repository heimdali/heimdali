Feature: tpr

    Scenario: Invoke tpr
        When I run the command: tpr --help
        Then I see the line in standard output: Print the pixel values of a image subregion

    Scenario: Print a full image
        When I run the command: tpr $HEIMDALI_DATA_DIR/<input>
        Then I see as standard output the content of the file $HEIMDALI_DATA_DIR/<stdout>
    Examples:
        | input                     | stdout            |
        | cossin_z5_y5_x5_c2_r4.h5  | tpr_cossin_r4.txt |
        | cossin_z5_y5_x5_c2_r4.inr | tpr_cossin_r4.txt |
        | cossin_z5_y5_x5_c2_f2.h5  | tpr_cossin_f2.txt |
        | cossin_z5_y5_x5_c2_f2.inr | tpr_cossin_f2.txt |

    Scenario: Print a full image, reading from stdin
        When I run the command: cat $HEIMDALI_DATA_DIR/<input> | tpr
        Then I see as standard output the content of the file $HEIMDALI_DATA_DIR/<stdout>
    Examples:
        | input                     | stdout            |
        | cossin_z5_y5_x5_c2_r4.h5  | tpr_cossin_r4.txt |
        | cossin_z5_y5_x5_c2_f2.h5  | tpr_cossin_f2.txt |
        | lena_r4.h5                | tpr_lena_r4.txt   |

    Scenario: Print a image region
       When I run the command: tpr -ix 1 -iy 1 -iz 1 -x 2 -y 2 -z 2  $HEIMDALI_DATA_DIR/<input>
       Then I see as standard output the content of the file $HEIMDALI_DATA_DIR/<stdout>
   Examples:
       | input                     | stdout                  |
       | cossin_z5_y5_x5_c2_r4.h5  | tpr_cossin_r4_region.txt |
       | cossin_z5_y5_x5_c2_r4.inr | tpr_cossin_r4_region.txt |
       | cossin_z5_y5_x5_c2_f2.h5  | tpr_cossin_f2_region.txt |
       | cossin_z5_y5_x5_c2_f2.inr | tpr_cossin_f2_region.txt |

    Scenario: Print a 3D image region, reading from stding
       When I run the command: cat $HEIMDALI_DATA_DIR/<input> | tpr -ix 1 -iy 1 -iz 1 -x 2 -y 2 -z 2  
       Then I see as standard output the content of the file $HEIMDALI_DATA_DIR/<stdout>
   Examples:
       | input                     | stdout                   |
       | cossin_z5_y5_x5_c2_r4.h5  | tpr_cossin_r4_region.txt |
       | cossin_z5_y5_x5_c2_f2.h5  | tpr_cossin_f2_region.txt |

    Scenario: Print a 2D image region, reading from stding
       When I run the command: cat $HEIMDALI_DATA_DIR/<input> | tpr -ix 1 -iy 1 -x 2 -y 2
       Then I see as standard output the content of the file $HEIMDALI_DATA_DIR/<stdout>
   Examples:
       | input                     | stdout                   |
       | lena_r4.h5                | tpr_lena_r4_region.txt   |
