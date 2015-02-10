Feature: The 'bi' command line tool.

    Scenario: Invoke bi
        When I run the command: bi --help
        Then I see the line in standard output: Add a constant to an image


    Scenario: Add 1 to each pixel
        When I run the command: bi -n 1 -i $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 -o out.h5
        Then I run the command: h5diff --exclude-path /HDFVersion --exclude-path /ITKVersion out.h5 $HEIMDALI_DATA_DIR/mtest_z5_y4_x3_c2_bi1.h5


