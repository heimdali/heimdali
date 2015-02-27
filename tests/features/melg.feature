Feature: melg

    Scenario: Invoke melg
        When I run the command: melg --help
        Then I see the line in standard output: Mix two images, testing on pixel values

    Scenario: Override a image subregion
        When I run the command: cp $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi1.h5 melg_out.h5
        When I run the command: melg -idz 2 -idy 2 -idx 2 -izi 1 -iyi 1 -ixi 1 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_two.h5 -izo 1 -iyo 1 -ixo 1 melg_out.h5
        Then images melg_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_i1_j1_k1_x2_y2_z2_two.h5 are equal
