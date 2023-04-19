/**
 ****************************************************************************************
 *
 * @file env_task.c
 *
 * @brief Proximity Reporter Task implementation.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup PROXRTASK
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_PROX_REPORTER)

#include "gap.h"
#include "gattc_task.h"
#include "attm.h"
#include "atts.h"
#include "env.h"
#include "env_task.h"
#include "attm_cfg.h"
#include "prf_utils.h"
#include "rwip.h"
#include "lld.h"
#include "gapc.h"
#include "reg_ble_em_cs.h"
#if defined (__DA14531__)
#include "rf_531.h"
#endif

uint16_t temp = 0;

void set_temp_val(uint16_t* temp_val){
	temp = *temp_val;
	printf("<ENV_TASK>: Temperature value set to: %d\n", *temp_val);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the read request from peer device
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_read_req_ind_handler(ke_msg_id_t const msgid,
                                      struct gattc_read_req_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    int msg_status = KE_MSG_CONSUMED;

    if(ke_state_get(dest_id) == PROXR_IDLE)
    {
        // Get the address of the environment
        struct env_env_tag *env_env = PRF_ENV_GET(PROXR, env);
        uint8_t conidx = KE_IDX_GET(src_id);

        uint8_t value = PROXR_ALERT_NONE;
        uint8_t status = ATT_ERR_NO_ERROR;

				if(param->handle == (env_env->txp_start_hdl + ENV_IDX_TEMP_VAL))
        {
            // send a request to read TX power level on GAPM
            struct gapm_get_dev_info_cmd* cmd = KE_MSG_ALLOC(GAPM_GET_DEV_INFO_CMD, TASK_GAPM, dest_id,
                    gapm_get_dev_info_cmd);
            cmd->operation = GAPM_GET_DEV_ADV_TX_POWER;
            ke_msg_send(cmd);

            // save the connection index
            env_env->conidx_saved = conidx;

            // need to wait for GAPM response, go into busy state
            ke_state_set(dest_id, PROXR_BUSY);

            status = PRF_APP_ERROR;
        }
        else
        {
            status = ATT_ERR_REQUEST_NOT_SUPPORTED;
        }

        // Don't send the confirmation if gap read request has been initiated.
        if(status != PRF_APP_ERROR)
        {
            // Send data to peer device
            struct gattc_read_cfm* cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id,
                    gattc_read_cfm, sizeof(uint8_t));
            cfm->length = sizeof(uint8_t);
            cfm->value[0] = value;
            cfm->handle = param->handle;
            cfm->status = status;

            // Send value to peer device.
            ke_msg_send(cfm);
        }
    }
    else if(ke_state_get(dest_id) == PROXR_BUSY)
    {
        msg_status = KE_MSG_SAVED;
    }

    return msg_status;
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GAPM_DEV_ADV_TX_POWER_IND message.
 * This generic event is received for different requests, so need to keep track.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_temp_ind_handler(ke_msg_id_t const msgid,
                                            struct gapm_dev_adv_tx_power_ind const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    // Get the address of the environment
    struct env_env_tag *env_env = PRF_ENV_GET(PROXR, env);

    if( ke_state_get(dest_id) == PROXR_BUSY )
    {
        // Send data to peer device
        struct gattc_read_cfm* cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, KE_BUILD_ID(TASK_GATTC, env_env->conidx_saved), dest_id,
                                                        gattc_read_cfm, sizeof(uint16_t));
        cfm->length = sizeof(uint16_t);

            cfm->value[0] = temp & 0xff;  // lsb
						cfm->value[1] = temp >> 8;  // msb
#else
            // DA14585, DA14586
            cfm->value[0] = rwip_rf.txpwr_dbm_get(ble_txpwr_getf(gapc_get_conhdl(env_env->conidx_saved)), MOD_GFSK);
#endif
        cfm->handle = env_env->txp_start_hdl + ENV_IDX_TEMP_VAL;
        cfm->status = GAP_ERR_NO_ERROR;

        // Send value to peer device.
				//printf("Value sent: %d\n", cfm->value[0]);
        ke_msg_send(cfm);

        ke_state_set(dest_id, PROXR_IDLE);
    }
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GAPM_CMP_EVT message.
 * This generic event is received for different requests, so need to keep track.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_cmp_evt_handler(ke_msg_id_t const msgid,
                                struct gapm_cmp_evt const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{
    // Get the address of the environment
    struct env_env_tag *env_env = PRF_ENV_GET(PROXR, env);

    if( (ke_state_get(dest_id) == PROXR_BUSY) && (param->status != GAP_ERR_NO_ERROR))
    {
        // Send data to peer device
        struct gattc_read_cfm* cfm = KE_MSG_ALLOC(GATTC_READ_CFM, KE_BUILD_ID(TASK_GATTC,
                                        env_env->conidx_saved), dest_id, gattc_read_cfm);
        cfm->length = sizeof(uint8_t);
        cfm->handle = env_env->txp_start_hdl + ENV_IDX_TEMP_VAL;
        cfm->status = PRF_APP_ERROR;

        // Send value to peer device.
        ke_msg_send(cfm);

        ke_state_set(dest_id, PROXR_IDLE);
    }
    return (KE_MSG_CONSUMED);
}


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
const struct ke_msg_handler env_default_state[] =
{
    {GATTC_READ_REQ_IND,                  (ke_msg_func_t) gattc_read_req_ind_handler},
    {GAPM_TEMP_IND,           (ke_msg_func_t) gapm_temp_ind_handler},
    {GAPM_CMP_EVT,                        (ke_msg_func_t) gapm_cmp_evt_handler},
};

/// Specifies the message handlers that are common to all states.
const struct ke_state_handler env_default_handler = KE_STATE_HANDLER(env_default_state);



/// @} PROXRTASK
