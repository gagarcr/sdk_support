/***************************************************************************//**
 * @file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef HARDWARE_ROUTINES_H
#define HARDWARE_ROUTINES_H
#include <stdint.h>

// Note: the functions below are hardware-specific callbacks used by the library to
// perform capacitive sense scanning.  All must be defined
// in the project in order for the library to function correctly.
uint32_t executeConversion(void);
uint32_t scanSensor(uint8_t);
void configureSensorForActiveMode(void);
void nodeInit(uint8_t sensor_index);
uint8_t determine_highest_gain(void);
void initDMModeSensor(uint8_t index);

#endif // HARDWARE_ROUTINES_H
