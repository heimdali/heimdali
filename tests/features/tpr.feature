Feature: tpr

    Scenario: Invoke tpr
        When I run the command: tpr --help
        Then I see the line in standard output: Print the pixel values of a image subregion

    Scenario: Print the pixel of a image subregion 
        When I run the command: tpr -i 1 -j 1 -k 1 -x 2 -y 2 -z 2  $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5
        Then I see the standard output:
            """
            [+1.1100000E+03, +1.1110000E+03]
            [+1.1200000E+03, +1.1210000E+03]
            [+1.2100000E+03, +1.2110000E+03]
            [+1.2200000E+03, +1.2210000E+03]
            [+2.1100000E+03, +2.1110000E+03]
            [+2.1200000E+03, +2.1210000E+03]
            [+2.2100000E+03, +2.2110000E+03]
            [+2.2200000E+03, +2.2210000E+03]
            ""
            """
