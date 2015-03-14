Feature: muls

    Scenario: Invoke muls
        When I run the command: muls --help
        Then I see the line in standard output: Threshold image with multiple thresholds

    #Scenario: Threshold image
        #When I run the command: muls -vs 0.2,0.4,0.6,0.8 $HEIMDALI_DATA_DIR/zero_to_one_f1.inr zero_to_one_f1_muls.inr
        #Then images zero_to_one_f1_muls.inr and $HEIMDALI_DATA_DIR/zero_to_one_f1_muls.inr are equal
