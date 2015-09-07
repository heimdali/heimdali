Feature: imo

    Scenario: Invoke imo
        When I run the command: imo --help
        Then I see the line in standard output: Inverse FFT on complex image
