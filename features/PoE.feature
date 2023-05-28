# features/PoE.feature

Feature: PoE Binary(s)

  Rule: Build PoE Binary(s)

    Example: PoE Binary
        Given binary named POE
        When using serial algorithm uart
        When using multiplexer algorithm Decoder
        When using matrix algorithm BCM
        And multiplex ratio of 1 : 8
        And serial clock 15 MHz
        And frame rate of 30
        And LED contrast ratio of 1 : 500
        And panel refresh rate of 100 Hz
        And panel with 128 columns
        And anti-ghosting blank time of 10 uS
        Then build it