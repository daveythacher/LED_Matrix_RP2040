# BCM Documentation
This implements the BCM Matrix Algorithm for standard (GEN 1) LED Panels. This does not use standard PWM.

BCM stands for binary coded modulation, which uses the binary magnitude to express the duty cycle.

## Status
This is believed to be in working order, however testing did not cover all aspects.

## Overview
This will generate multiple on times within the PWM period. This requires less computation and is not supported for high refresh rates.

This is believed to have issues with higher refresh rates as the panels have some low pass filters built into them. These filters will corrupt the duty cycle within the PWM period. 

## Interrupts
Follows standard design for Matrix Algorithms.

## Core reservations
Follows standard design for Matrix Algorithms.
