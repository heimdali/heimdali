Feature: the InrImage backward compatible class

    Scenario: Run the inrimage.cxx example
        Given I build the inrimage example
        When I run the example: ./inrimage $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5
        Then I see the standard output:
        """
        sz: 5
        sy: 4
        sx: 3
        sv: 2
        ""
        """
