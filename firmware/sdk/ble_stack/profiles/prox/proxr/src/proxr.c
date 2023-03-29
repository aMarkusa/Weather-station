/**
 ****************************************************************************************
 *
 * @file env.c
 *
 * @brief Proximity Reporter Implementation.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup PROXR
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_PROX_REPORTER)

#include <string.h>

#include "proxr.h"
#include "proxr_task.h"
#include "attm_db.h"
#include "gap.h"

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static uint8_t env_init(struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task,
                            uint8_t sec_lvl, struct env_db_cfg* params)
{
    // Service content flag
    uint32_t cfg_flag = PROXR_LLS_MANDATORY_MASK;
    // DB Creation Status
    uint8_t status;

    //-------------------- allocate memory required for the profile  ---------------------
    struct env_env_tag* env_env =
            (struct env_env_tag* ) ke_malloc(sizeof(struct env_env_tag), KE_MEM_ATT_DB);
    memset(env_env, 0, sizeof(struct env_env_tag));

    // allocate PROXR required environment variable
    env->env = (prf_env_t*) env_env;


    if( (status == ATT_ERR_NO_ERROR) && (params->features == PROXR_IAS_TXPS_SUP) )
    {
       
       if( status == ATT_ERR_NO_ERROR )
       {
           cfg_flag = PROXR_TXP_MANDATORY_MASK;
           status = attm_svc_create_db(start_hdl, ATT_SVC_ENVIRONMENTAL_SENSING, (uint8_t *)&cfg_flag,
                      ENV_IDX_NB, NULL, env->task, &env_txps_att_db[0],
                     (sec_lvl & PERM_MASK_SVC_AUTH) | (sec_lvl & PERM_MASK_SVC_EKS) | PERM(SVC_PRIMARY, ENABLE)
                     | PERM(SVC_MI, DISABLE) );

           env_env->txp_start_hdl     = *start_hdl;

           if (status == ATT_ERR_NO_ERROR)
           {
               // set start handle to first allocated service value
               *start_hdl = env_env->ias_start_hdl;

               env_env->features          = params->features;
               env_env->prf_env.app_task  = app_task
                       | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
               env_env->prf_env.prf_task  = env->task | PERM(PRF_MI, DISABLE);

               // initialize environment variable
               env->id                     = TASK_ID_PROXR;
               env->desc.idx_max           = PROXR_IDX_MAX;
               env->desc.state             = env_env->state;
               env->desc.default_handler   = &env_default_handler;

               // service is ready, go into an Idle state
               ke_state_set(env->task, PROXR_IDLE);
           }
       }
    }
    return status;
}

static void env_destroy(struct prf_task_env* env)
{
    struct env_env_tag* env_env = (struct env_env_tag*) env->env;

    // free profile environment variables
    env->env = NULL;
    ke_free(env_env);
}

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 ****************************************************************************************
 */
static void env_create(struct prf_task_env* env, uint8_t conidx)
{
    struct env_env_tag* env_env = (struct env_env_tag*) env->env;

    env_env->lls_alert_lvl[conidx] = PROXR_ALERT_NONE;
}

/**
 ****************************************************************************************
 * @brief Handles Disconnection
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 * @param[in]        reason     Detach reason
 ****************************************************************************************
 */
static void env_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    struct env_env_tag* env_env = (struct env_env_tag*) env->env;

    if((env_env->features == PROXR_IAS_TXPS_SUP) && (reason != LL_ERR_REMOTE_USER_TERM_CON))
    {
        if(env_env->lls_alert_lvl[conidx] > PROXR_ALERT_NONE)
        {

            // Allocate the alert value change indication
            struct env_alert_ind *ind = KE_MSG_ALLOC(PROXR_ALERT_IND,
                    prf_dst_task_get(&(env_env->prf_env), conidx),
                    prf_src_task_get(&(env_env->prf_env), conidx), env_alert_ind);

            // Fill in the parameter structure
            ind->alert_lvl = env_env->lls_alert_lvl[conidx];
            ind->char_code = PROXR_LLS_CHAR;
            ind->conidx = conidx;

            // Send the message
            ke_msg_send(ind);
        }
    }

}

/*
 * PROXIMITY PROFILE ATTRIBUTES DEFINITION
 ****************************************************************************************
 */


/// Full TXPS Database Description - Used to add attributes into the database
const struct attm_desc env_txps_att_db[ENV_IDX_NB] =
{
    // TX Power Service Declaration
    [ENV_IDX_SVC]                  =   {ATT_DECL_PRIMARY_SERVICE, PERM(RD, ENABLE), 0},
    // TX Power Level Characteristic Declaration
    [ENV_IDX_TEMP_CHAR]       =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), ENV_IDX_TEMP_VAL},
    // TX Power Level Characteristic Value
    [ENV_IDX_TEMP_VAL]     =   {ATT_CHAR_TEMPERATURE_MEAS, PERM(RD, ENABLE), PERM(RI, ENABLE) | sizeof(uint8_t)},
};

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */


/// PROXR Task interface required by profile manager
const struct prf_task_cbs env_itf =
{
    (prf_init_fnct) env_init,
    env_destroy,
    env_create,
    env_cleanup,
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs* env_prf_itf_get(void)
{
   return &env_itf;
}


#endif //BLE_PROX_REPORTER

/// @} PROXR
