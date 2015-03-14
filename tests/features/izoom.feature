Feature: izoom

    Scenario: Invoke izoom
        When I run the command: izoom --help
        Then I see the line in standard output: Image enlargement or reduction

    Scenario: Enlarge an image, and reduce it.
        When I run the command: izoom -sc 2 $HEIMDALI_DATA_DIR/lena_f1.inr lena_f1_sc_2.inr
        Then images lena_f1_sc_2.inr and $HEIMDALI_DATA_DIR/lena_f1_sc_2.inr are equal
