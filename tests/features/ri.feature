Feature: ri

    Scenario: Invoke ri
        When I run the command: ri --help
        Then I see the line in standard output: Convert complex image from (real, imaginary) to (module, phase)

    Scenario: Compute direct FFT on real image
        When I run the command: ri $HEIMDALI_DATA_DIR/lena_r4_rdf_re.inr $HEIMDALI_DATA_DIR/lena_r4_rdf_im.inr lena_r4_ri_module.inr lena_r4_ri_phase.inr 
        Then images lena_r4_ri_module.inr and $HEIMDALI_DATA_DIR/lena_r4_ri_module.inr are equal
        Then images lena_r4_ri_phase.inr and $HEIMDALI_DATA_DIR/lena_r4_ri_phase.inr are equal
