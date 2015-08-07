Feature: ddf

    Scenario: Invoke ddf
        When I run the command: ddf --help
        Then I see the line in standard output: Direct FFT on complex image

    Scenario: Compute direct FFT on complex image
        When I run the command: ddf $HEIMDALI_DATA_DIR/lena_r4_rdf_re.h5 $HEIMDALI_DATA_DIR/lena_r4_rdf_im.h5 lena_r4_ddf_re.h5 lena_r4_ddf_im.h5
        Then images lena_r4_ddf_re.h5 and $HEIMDALI_DATA_DIR/lena_r4_ddf_re.h5 are equal
        Then images lena_r4_ddf_im.h5 and $HEIMDALI_DATA_DIR/lena_r4_ddf_im.h5 are equal
