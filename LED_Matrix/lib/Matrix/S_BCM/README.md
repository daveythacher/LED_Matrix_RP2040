# S_PWM Documentation
This implements the S_PWM Matrix Algorithm for standard (GEN 1) LED Panels. This does not use binary coded modulation (BCM) or bit angle modulation (BAM).

## Status
This is believed to be in working order, however testing did not cover all aspects.

## Overview
This will generate a single on time within the PWM period. This requires more computation and is not supported for all panel sizes and color depth configurations.

This is believed to matter for higher refresh rates as the panels have some low pass filters built into them. These filters will corrupt the duty cycle within the PWM period. The effect is potentially larger with the BCM Matrix Algorithm.

## Interrupts
Follows standard design for Matrix Algorithms.

## Core reservations
Follows standard design for Matrix Algorithms.
