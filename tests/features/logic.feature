Feature: logic

    Scenario: Invoke logic
        When I run the command: logic --help
        Then I see the line in standard output: Boolean operation on image

    Scenario: Binary AND two images
        When I run the command: logic -et $HEIMDALI_DATA_DIR/boolean_f1.<ext> $HEIMDALI_DATA_DIR/boolean_another_f1.<ext> boolean_and_f1.<ext>
        Then images boolean_and_f1.<ext> and $HEIMDALI_DATA_DIR/boolean_and_f1.<ext> are equal

    Examples:
    | ext |
    | h5  |
    | inr |
