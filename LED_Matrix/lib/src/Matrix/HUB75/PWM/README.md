# PWM Documentation
This implements the PWM Matrix Algorithm for standard (GEN 1) LED Panels. This does not use binary coded modulation (BCM) or bit angle modulation (BAM).

## Status
This is believed to be in working order, however testing did not cover all aspects.

## Overview
This will generate a single on time within the PWM period. This requires more computation and is not supported for all panel sizes and color depth configurations.

This is believed to matter for higher refresh rates as the panels have some low pass filters built into them. These filters will corrupt the duty cycle within the PWM period. The effect is potentially larger with the BCM Matrix Algorithm.

## Interrupts
Follows standard design for Matrix Algorithms.

## Core reservations
Follows standard design for Matrix Algorithms.

Internal notes:
PWM_Worker/LUT/PWM_Worker - WIP
PWM_Worker/PWM_Worker - done
PWM_Buffer - done
PWM_Calculator - Needs conversion
PWM_Matrix - Mostly done
PWM_Multiplex - WIP
PWM_Packet - done
PWM_Programs - done

Design is IO pipeline of delegation (FIFO)
Front end is API without flow control.

Matrix is Primary interface (Responsible for Packet and Buffer)
Matrix drives PWM_Worker (Uses PWM_Calculator)
PWM_Worker drives PWM_Multiplex (Responsible for PWM_Programs)
PWM_Multiplex drives Multiplex

Check these:
unique_pointers
factories
exceptions
style
Others?
