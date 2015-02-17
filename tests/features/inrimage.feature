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
        "image[2,0,0] = 2000 2001 "
        "image[2,0,1] = 2010 2011 "
        "image[2,0,2] = 2020 2021 "
        "image[2,1,0] = 2100 2101 "
        "image[2,1,1] = 2110 2111 "
        "image[2,1,2] = 2120 2121 "
        "image[2,2,0] = 2200 2201 "
        "image[2,2,1] = 2210 2211 "
        "image[2,2,2] = 2220 2221 "
        "image[2,3,0] = 2300 2301 "
        "image[2,3,1] = 2310 2311 "
        "image[2,3,2] = 2320 2321 "
        ""
        """
