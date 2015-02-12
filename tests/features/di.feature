Feature: The 'di' command line tool.

    Scenario: Invoke di
        When I run the command: di --help
        Then I see the line in standard output: Divide two images

    #Scenario: Divide to images
        #When I run the command: di -o di_out.h5 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 $HEIMDATA_DIR_DIR/imtest_z5_y4_x3_c2_two.h5
        #Then the HDF5 files di_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_di2.h5 are equal
