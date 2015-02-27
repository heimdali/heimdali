Feature: heimdali-convert

    Scenario: Invoke heimdali-convert
        When I run the command: heimdali-convert --help
        Then I see the line in standard output: Convert image from one format to another

    Scenario: Convert from HDF5 to INRimage, and back to HDF5
        When I run the command: heimdali-convert $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 imtest_z5_y4_x3_c2.inr
        When I run the command: heimdali-convert imtest_z5_y4_x3_c2.inr imtest_z5_y4_x3_c2.h5
        Then images imtest_z5_y4_x3_c2.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 are equal

    Scenario: Convert from HDF5 to INRimage, and back to HDF5, but with origin different that zero.
        When I run the command: heimdali-convert $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_origin_4_5_6.h5 imtest_z5_y4_x3_c2_origin_4_5_6.inr
        When I run the command: heimdali-convert imtest_z5_y4_x3_c2_origin_4_5_6.inr imtest_z5_y4_x3_c2_origin_4_5_6.h5
        Then images imtest_z5_y4_x3_c2_origin_4_5_6.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_origin_4_5_6.h5 are equal

    Scenario: Convert from HDF5 to INRimage, and back to HDF5, with image of 1 plane and 1 pixel component
        When I run the command: heimdali-convert $HEIMDALI_DATA_DIR/imtest_z1_y3_x2_c1.h5 imtest_z1_y3_x2_c1.inr 
        When I run the command: heimdali-convert imtest_z1_y3_x2_c1.inr imtest_z1_y3_x2_c1.h5 
        Then images imtest_z1_y3_x2_c1.h5 and $HEIMDALI_DATA_DIR/imtest_z1_y3_x2_c1.h5 are equal

    Scenario: Convert from HDF5 to INRimage, and back to HDF5, with UCHAR image
        When I run the command: heimdali-convert $HEIMDALI_DATA_DIR/lena_f1.h5 lena_f1.inr
        When I run the command: heimdali-convert lena_f1.inr lena_f1.h5
        Then images lena_f1.h5 and $HEIMDALI_DATA_DIR/lena_f1.h5 are equal

