Feature: inrtoh5

    Scenario: Invoke h5toinr
        When I run the command: h5toinr --help
        Then I see the line in standard output: Convert HDF5 image to INRimage image

    Scenario: Invoke inrtoh5
        When I run the command: inrtoh5 --help
        Then I see the line in standard output: Convert INRimage image to HDF5 image

    Scenario: Convert from HDF5 to INRimage, and back to HDF5
        When I run the command: h5toinr $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 h5toinr_tmp.inr
        When I run the command: inrtoh5 h5toinr_tmp.inr h5toinr_tmp.h5
        Then the HDF5 files h5toinr_tmp.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 are equal

    Scenario: Convert from HDF5 to INRimage, and back to HDF5, but with origin different that zero.
        When I run the command: h5toinr $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_origin_4_5_6.h5 h5toinr_tmp_origin_4_5_6.inr
        When I run the command: inrtoh5 h5toinr_tmp_origin_4_5_6.inr h5toinr_tmp_origin_4_5_6.h5
        Then the HDF5 files h5toinr_tmp_origin_4_5_6.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_origin_4_5_6.h5 are equal
