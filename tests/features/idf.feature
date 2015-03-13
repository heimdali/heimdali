Feature: idf

    Scenario: Invoke idf
        When I run the command: idf --help
        Then I see the line in standard output: Inverse FFT on complex image

    Scenario: Compute direct FFT on complex image
        When I run the command: idf $HEIMDALI_DATA_DIR/lena_r4_ddf_re.h5 $HEIMDALI_DATA_DIR/lena_r4_ddf_im.h5 lena_r4_idf_re.h5 lena_r4_idf_im.h5
