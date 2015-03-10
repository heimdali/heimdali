Feature: rot

    Scenario: Invoke rot
        When I run the command: rot --help
        Then I see the line in standard output: z-axis rotation and translation

