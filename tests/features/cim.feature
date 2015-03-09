Feature: cim

    Scenario: Invoke cim
        When I run the command: cim --help
        Then I see the line in standard output: Inputs ASCII value into image

    Scenario: Read from standard input
        When I run the interactive command: cim imtest_z5_y4_x3_c2.<ext>

        Then I see in the interactive command standard output: Enter sx, sy, sv, sz
        When I input to the interactive command: 3 4 2 5

        Then I see in the interactive command standard output: Enter pixel type \(0: fixed point, 1: floating point\)
        When I input to the interactive command: 1

        Then I see in the interactive command standard output: Enter byte size \(0: float, 1: double\)
        When I input to the interactive command: 0

        Then I see in the interactive command standard output: Enter pixel values
        When I input to the interactive command the text:
        """
        0 1 10 11 20 21 
        100 101 110 111 120 121 
        200 201 210 211 220 221 
        300 301 310 311 320 321 
        1000 1001 1010 1011 1020 1021 
        1100 1101 1110 1111 1120 1121 
        1200 1201 1210 1211 1220 1221 
        1300 1301 1310 1311 1320 1321 
        2000 2001 2010 2011 2020 2021 
        2100 2101 2110 2111 2120 2121 
        2200 2201 2210 2211 2220 2221 
        2300 2301 2310 2311 2320 2321 
        3000 3001 3010 3011 3020 3021 
        3100 3101 3110 3111 3120 3121 
        3200 3201 3210 3211 3220 3221 
        3300 3301 3310 3311 3320 3321 
        4000 4001 4010 4011 4020 4021 
        4100 4101 4110 4111 4120 4121 
        4200 4201 4210 4211 4220 4221 
        4300 4301 4310 4311 4320 4321 
        """

        Then I see in the interactive command standard output: imtest_z5_y4_x3_c2.<ext>
        Then images imtest_z5_y4_x3_c2.<ext> and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext> are equal

    Examples:
      | ext |
      | h5  |
      | inr |
