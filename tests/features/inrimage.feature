Feature: the InrImage backward compatible class

    Scenario: Run the inrimage_read.cxx example
        Given I build the inrimage_read example
        Then I run the example: ./inrimage_read $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5
