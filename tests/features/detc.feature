Feature: detc

    Scenario: Invoke detc
        When I run the command: detc --help
        Then I see the line in standard output: Detect image contour

