Feature: rmo

    Scenario: Invoke rmo
        When I run the command: rmo --help
        Then I see the line in standard output: Direct FFT on real image
