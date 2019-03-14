/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

# include "intertask_interface.h"
# include "create_tasks.h"
# include "common/utils/LOG/log.h"
# include "targets/RT/USER/lte-softmodem.h"

#ifdef OPENAIR2
  #include "sctp_eNB_task.h"
  #include "x2ap_eNB.h"
  #include "s1ap_eNB.h"
  #include "udp_eNB_task.h"
  #include "gtpv1u_eNB_task.h"
  #if ENABLE_RAL
    #include "lteRALue.h"
    #include "lteRALenb.h"
  #endif
  #include "RRC/LTE/rrc_defs.h"
#endif
# include "enb_app.h"


int create_tasks(uint32_t enb_nb) {
  LOG_D(ENB_APP, "%s(enb_nb:%d\n", __FUNCTION__, enb_nb);
  itti_wait_ready(1);

  if (enb_nb > 0) {
    /* Last task to create, others task must be ready before its start */
    if (itti_create_task (TASK_ENB_APP, eNB_app_task, NULL) < 0) {
      LOG_E(ENB_APP, "Create task for eNB APP failed\n");
      return -1;
    }
  }

  if (EPC_MODE_ENABLED) {
    if (enb_nb > 0) {
      if (is_x2ap_enabled()) {
        if (itti_create_task (TASK_X2AP, x2ap_task, NULL) < 0) {
          LOG_E(X2AP, "Create task for X2AP failed\n");
          return -1;
        }
      } else
        LOG_I(X2AP, "X2AP is disabled.\n");

      if (itti_create_task (TASK_SCTP, sctp_eNB_task, NULL) < 0) {
        LOG_E(SCTP, "Create task for SCTP failed\n");
        return -1;
      }

      if (itti_create_task (TASK_S1AP, s1ap_eNB_task, NULL) < 0) {
        LOG_E(S1AP, "Create task for S1AP failed\n");
        return -1;
      }

      if(!(get_softmodem_params()->emulate_rf)) {
        if (itti_create_task (TASK_UDP, udp_eNB_task, NULL) < 0) {
          LOG_E(UDP_, "Create task for UDP failed\n");
          return -1;
        }
      }

      if (itti_create_task (TASK_GTPV1_U, &gtpv1u_eNB_task, NULL) < 0) {
        LOG_E(GTPU, "Create task for GTPV1U failed\n");
        return -1;
      }
    }
  } /* if (EPC_MODE_ENABLED) */

  if (enb_nb > 0) {
    LOG_I(RRC,"Creating RRC eNB Task\n");

    if (itti_create_task (TASK_RRC_ENB, rrc_enb_task, NULL) < 0) {
      LOG_E(RRC, "Create task for RRC eNB failed\n");
      return -1;
    }
  }

  itti_wait_ready(0);
  return 0;
}

