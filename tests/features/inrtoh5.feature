Feature: Convert images between HDF5 and INRimage format

    Scenario: Invoke h5toinr
        When I run the command: h5toinr --help
        Then I see the line in standard output: Convert HDF5 image to INRimage image

    Scenario: Invoke inrtoh5
        When I run the command: inrtoh5 --help
        Then I see the line in standard output: Convert INRimage image to HDF5 image

    Scenario: Convert from HDF5 to INRimage, and back to HDF5
        When I run the command: h5toinr $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 tmp.inr
        When I run the command: inrtoh5 tmp.inr tmp.h5
        Then I run the command: h5diff --exclude-path /HDFVersion --exclude-path /ITKVersion tmp2.h5 $HEIMDALI_DATA_DIR/mtest_z5_y4_x3_c2.h5
