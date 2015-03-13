Feature: ma

    Scenario: Invoke ma
        When I run the command: ma --help
        Then I see the line in standard output: Convert complex image from (magnitude, phase) to (real, imaginary)

    Scenario: Apply conversion
        When I run the command: ma $HEIMDALI_DATA_DIR/lena_r4_ri_module.inr $HEIMDALI_DATA_DIR/lena_r4_ri_phase.inr lena_r4_ma_re.inr lena_r4_ma_im.inr 
        Then images lena_r4_ma_re.inr and $HEIMDALI_DATA_DIR/lena_r4_ma_re.inr  are equal
        Then images lena_r4_ma_im.inr and $HEIMDALI_DATA_DIR/lena_r4_ma_im.inr  are equal
