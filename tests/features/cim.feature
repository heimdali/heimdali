Feature: cim

    Scenario: Invoke cim
        When I run the command: cim --help
        Then I see the line in standard output: Inputs ASCII value into image

    Scenario: Create a floating point image
        When I run the interactive command: cim zero_to_one_r4.<ext>

        Then I see in the interactive command standard output: Enter sx, sy, sv, sz
        When I input to the interactive command: 3 4 2 5

        Then I see in the interactive command standard output: Enter pixel type \(0: fixed point, 1: floating point\)
        When I input to the interactive command: 1

        Then I see in the interactive command standard output: Enter byte size \(4: float, 8: double\)
        When I input to the interactive command: 4

        Then I see in the interactive command standard output: Enter pixel values
        When I input the interactive command with the content of the file: $HEIMDALI_DATA_DIR/zero_to_one.txt

        Then I see in the interactive command standard output: zero_to_one_r4.<ext>
        Then images zero_to_one_r4.<ext> and $HEIMDALI_DATA_DIR/zero_to_one_r4.<ext> are equal

    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Create a fixed point image
        When I run the interactive command: cim zero_to_one_f2.<ext>

        Then I see in the interactive command standard output: Enter sx, sy, sv, sz
        When I input to the interactive command: 3 4 2 5

        Then I see in the interactive command standard output: Enter pixel type \(0: fixed point, 1: floating point\)
        When I input to the interactive command: 0

        Then I see in the interactive command standard output: Enter byte size \(1: unsigned char, 2: unsigned short, 4 unsigned int\)
        When I input to the interactive command: 2

        Then I see in the interactive command standard output: Enter pixel values
        When I input the interactive command with the content of the file: $HEIMDALI_DATA_DIR/zero_to_one.txt

        Then I see in the interactive command standard output: zero_to_one_f2.<ext>
        Then images zero_to_one_f2.<ext> and $HEIMDALI_DATA_DIR/zero_to_one_f2.<ext> are equal

    Examples:
      | ext |
      | h5  |
      | inr |
