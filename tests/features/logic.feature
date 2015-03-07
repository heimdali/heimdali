Feature: logic

    Scenario: Invoke logic
        When I run the command: logic --help
        Then I see the line in standard output: Boolean operation on image
