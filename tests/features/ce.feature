Feature: ce

    Scenario: Invoke ce
        When I run the command: ce --help
        Then I see the line in standard output: Swap image quadrant regions

