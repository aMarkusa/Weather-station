/**
 ****************************************************************************************
 *
 * @file env_task.h
 *
 * @brief Header file - PROXRTASK.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef PROXR_TASK_H_
#define PROXR_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup PROXRTASK Proximity Reporter Task
 * @ingroup PROXR
 * @brief Proximity Reporter Task
 *
 * The PROXRTASK is responsible for handling the APi messages from the Application or internal
 * tasks.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */



#include <stdint.h>
#include "ke_task.h"
#include "env.h"

/*
 * DEFINES
 ****************************************************************************************
 */
 


/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Possible states of the PROXR task
enum
{
    /// Idle state
    PROXR_IDLE,
    /// Proximity reporter in a busy state
    PROXR_BUSY,

    /// Number of defined states.
    PROXR_STATE_MAX
};

/// Proximity reporter feature bit field
enum env_feature
{
    /// immediate alert and TX Power services are not present
    PROXR_IAS_TXPS_NOT_SUP = 0,
    /// immediate alert and TX Power services are present
    PROXR_IAS_TXPS_SUP,
};

/// Messages for Proximity Reporter
enum env_msg_id
{
    /// LLS/IAS Alert Level Indication
    PROXR_ALERT_IND = KE_FIRST_MSG(TASK_ID_PROXR),
};

///Characteristics Code for Write Indications
enum
{
    PROXR_ERR_CHAR,
    PROXR_LLS_CHAR,
    PROXR_IAS_CHAR,
};


/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */
///Parameters of the Proximity service database
struct env_db_cfg
{
    /// Proximity Feature (@see enum proxm_feature)
    uint16_t features;
};


/// Parameters of the @ref PROXR_ALERT_IND message
struct env_alert_ind
{
    /// Connection index
    uint8_t conidx;
    /// Alert level
    uint8_t alert_lvl;
    /// Char Code - Indicate if LLS or IAS
    uint8_t char_code;
};


/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler env_default_handler;


#endif //BLE_PROX_REPORTER

void meas_temp(void);
/// @} PROXRTASK


