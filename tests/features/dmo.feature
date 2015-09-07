Feature: dmo

    Scenario: Invoke dmo
        When I run the command: dmo --help
        Then I see the line in standard output: Direct FFT on complex image
