Feature: med

    Scenario: Invoke med
        When I run the command: med --help
        Then I see the line in standard output: Median filter

