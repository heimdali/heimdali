Feature: Convert images between HDF5 and INRimage format

    Scenario: Invoke h5toinr
        When I run the command: h5toinr --help
        Then I see the line in standard output: Convert HDF5 image to INRimage image

    Scenario: Invoke inrtoh5
        When I run the command: inrtoh5 --help
        Then I see the line in standard output: Convert INRimage image to HDF5 image
