/***************************************************************************//**
 * @file
 * @brief OS BSP
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

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _BSP_OS_H_
#define  _BSP_OS_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/toolchains.h>
#include  <common/include/rtos_path.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 *
 * Note(s) : (1) A bug exists in some version of IAR where the compiler can make some optimizations using
 *               a weak definition even though a strong definition exists somewhere else. Using the
 *               volatile type qualifier in the weak definition prevents this optimization.
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_TOOLCHAIN != RTOS_TOOLCHAIN_IAR)
#define  BSP_HW_INFO_EXT(type, name)  PP_WEAK_VAR_DECL(type, name, { 0 })
#else
// See note (1).
#define  BSP_HW_INFO_EXT(type, name)  PP_WEAK_VAR_DECL(volatile type, name, { 0 })
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void BSP_SystemInit(void);

void BSP_OS_Init(void);

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of module include.
