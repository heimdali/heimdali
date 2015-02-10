Feature: The 'extg' command line tool.

    Scenario: Invoke extg
        When I run the command: extg --help
        Then I see the line in standard output: Extract image subregion

    Scenario: Extract a image subregion
        When I run the command: extg -i 1 -j 1 -k 1 -x 2 -y 2 -z 2  $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 extg_out.h5
        Then the HDF5 files extg_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_i1_j1_k1_x2_y2_z2.h5 are equal
