Feature: flofix

    Scenario: Invoke flofix
        When I run the command: flofix --help
        Then I see the line in standard output: Converts from floating point to fixed point
