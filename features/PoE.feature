# features/PoE.feature

Feature: PoE Binary(s)

  Rule: Build PoE Binary(s)

    Example: PoE Binary
        Given binary named POE
        When using serial algorithm uart
        When using multiplexer algorithm Decoder
        When using matrix algorithm PWM
        And multiplex ratio of 1 : 8
        And serial clock 17.5 MHz
        And LED contrast ratio of 1 : 8
        And panel refresh rate of 3000 Hz
        And panel with 32 columns
        And anti-ghosting blank time of 10 uS
        And UART baud of 4000000
        And serial protocol uses RGB-24
        Then build it

    Example: PoE Binary Raw
        Given binary named POE_raw
        When using serial algorithm uart
        When using multiplexer algorithm Decoder
        When using matrix algorithm PWM
        And multiplex ratio of 1 : 8
        And serial clock 17.5 MHz
        And LED contrast ratio of 1 : 16
        And panel refresh rate of 3000 Hz
        And panel with 32 columns
        And anti-ghosting blank time of 10 uS
        And UART baud of 4000000
        And serial protocol data is raw
        Then build it

    Example: PoE Binary with TLC5958
        Given binary named POEv2
        When using serial algorithm uart
        When using multiplexer algorithm Decoder
        When using matrix algorithm TLC5958
        And multiplex ratio of 1 : 8
        And serial clock 2 MHz
        And frame rate of 30
        And LED contrast ratio of 1 : 500
        And panel refresh rate of 3000 Hz
        And panel with 128 columns
        And anti-ghosting blank time of 10 uS
        And red gain of 0.5
        And green gain of 0.75
        And blue gain of 1.0
        And grayscale clock of 17.5 MHz
        And UART baud of 7800000
        And serial protocol uses RGB-24
        Then build it
