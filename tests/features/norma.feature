Feature: norma

    Scenario: Invoke norma
        When I run the command: norma --help
        Then I see the line in standard output: Rescale image value between 0 and 1
