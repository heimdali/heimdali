Feature: car

    Scenario: Invoke car
        When I run the command: car --help
        Then I see the line in standard output: Compute square of each pixel

    Scenario: Compute square root of each pixel
        When I run the command: car $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_ra.h5 -o car_out.h5
        Then the HDF5 files car_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 are almost equal with the relative parameter 1.E-5
