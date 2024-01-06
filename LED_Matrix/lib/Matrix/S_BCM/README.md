# S_PWM Documentation
This implements the S_BCM Matrix Algorithm for standard (GEN 1) LED Panels. This does use binary coded modulation (BCM) or bit angle modulation (BAM).

## Status
This is believed to be in working order, however no testing has been done. 

### Flicker
This will implementation is not completely flicker free for all duty cycle values. Currently values below 1/15th of a percent are believed to flicker. This will be rail to rail below the target refresh rate. However the implementation does ensure that this refresh is at least 95Hz. Flicker free is generally believed to be based on the intensive of the light and refresh rate. This implementation reserves 4 bits at target refresh rate which should be somewhere in between 1.5kHz and 3kHz. This means that 93 percent of the intensity is stable.

Removing flicker is possible with a custom gamma table in the application logic, but this will compromise the contrast ratio. There are a few software tricks to choose from however these are not the concern of this implementation.

## Overview
This will generate a single on time within the PWM period. This requires more computation and is not supported for all panel sizes and color depth configurations.

This is believed to matter for higher refresh rates as the panels have some low pass filters built into them. These filters will corrupt the duty cycle within the PWM period. The effect is potentially larger with the BCM Matrix Algorithm.

## Interrupts
Follows standard design for Matrix Algorithms.

## Core reservations
Follows standard design for Matrix Algorithms.
