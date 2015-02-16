Feature: the CreateImage and OpenAndWriteImage functions

    Scenario: Run the createInputImage.cxx example
        Given I build the createInputImage example
        When I run the createInputImage example
        Then the HDF5 files ../example/build/image.h5 and $HEIMDALI_DATA_DIR/cossin_z5_y5_x5_c2.h5 are equal
