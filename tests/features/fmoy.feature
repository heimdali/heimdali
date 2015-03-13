Feature: fmoy

    Scenario: Invoke fmoy
        When I run the command: fmoy --help
        Then I see the line in standard output: Convolution of image by mean filter

