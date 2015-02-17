Feature: the InrImage backward compatible class

    Scenario: Run the inrimage.cxx example
        Given I build the inrimage example
        Then I run the example: ./inrimage $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5
