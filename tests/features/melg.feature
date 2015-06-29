Feature: melg

    Scenario: Invoke melg
        When I run the command: melg --help
        Then I see the line in standard output: Mix two images, testing on pixel values

    Scenario: Override a image subregion
        When I run the command: cp $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi1.h5 melg_out.h5
        When I run the command: melg -idz 2 -idy 2 -idx 2 -izi 1 -iyi 1 -ixi 1 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_two.h5 -izo 1 -iyo 1 -ixo 1 melg_out.h5
        Then images melg_out.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_i1_j1_k1_x2_y2_z2_two.h5 are equal

    Scenario: Read from stdin
        When I run the command: cp $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_bi1.h5 melg_out_stdin.h5
        When I run the command: cat $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_two.h5 | melg -idz 2 -idy 2 -idx 2 -izi 1 -iyi 1 -ixi 1 -izo 1 -iyo 1 -ixo 1 - melg_out_stdin.h5
        Then images melg_out_stdin.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_i1_j1_k1_x2_y2_z2_two.h5 are equal

    Scenario: Paste top-left region of lena in blank image
        When I run the command: raz -x 256 -y 256 | cco -f > large_blank.h5
        When I run the command: extg -x 100 -y 100 $HEIMDALI_DATA_DIR/lena_f1.h5 small_lena.h5
        When I run the command: melg small_lena.h5 large_blank.h5
        Then images large_blank.h5 and $HEIMDALI_DATA_DIR/melg_lena_f1.h5 are equal
