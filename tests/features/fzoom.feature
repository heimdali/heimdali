Feature: fzoom

    Scenario: Invoke fzoom
        When I run the command: fzoom --help
        Then I see the line in standard output: Image enlargement or reduction

    Scenario: Enlarge an image, and reduce it.
        When I run the command: fzoom -sc 2 $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 fzoom_out0.h5
        Then images fzoom_out0.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2_fzoom_2.h5 are equal
        When I run the command: fzoom -sc 0.5 fzoom_out0.h5 fzoom_out1.h5
        #Then images fzoom_out1.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 are equal
