Feature: rdf

    Scenario: Invoke rdf
        When I run the command: rdf --help
        Then I see the line in standard output: Direct FFT on real image

    Scenario: Compute direct FFT on real image
        When I run the command: rdf $HEIMDALI_DATA_DIR/lena_r4.inr lena_r4_rdf_re.inr lena_r4_rdf_im.inr
        #Then images lena_r4_rdf_re.inr and $HEIMDALI_DATA_DIR/lena_r4_rdf_re.inr are equal
        #Then images lena_r4_rdf_im.inr and $HEIMDALI_DATA_DIR/lena_r4_rdf_im.inr are equal
