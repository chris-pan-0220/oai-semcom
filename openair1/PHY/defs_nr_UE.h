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

/*! \file PHY/defs_nr_UE.h
 \brief Top-level defines and structure definitions for nr ue
 \author Guy De Souza, H. WANG, A. Mico Pereperez
 \date 2018
 \version 0.1
 \company Eurecom
 \email: desouza@eurecom.fr
 \note
 \warning
*/
#ifndef __PHY_DEFS_NR_UE__H__
#define __PHY_DEFS_NR_UE__H__


#include "defs_nr_common.h"
#include "CODING/nrPolar_tools/nr_polar_pbch_defs.h"


#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include "common_lib.h"
#include "msc.h"

//#include <complex.h>
#include "assertions.h"
#ifdef MEX
# define msg mexPrintf
#else
# ifdef OPENAIR2
#   if ENABLE_RAL
#     include "collection/hashtable/hashtable.h"
#     include "COMMON/ral_messages_types.h"
#     include "UTIL/queue.h"
#   endif
#   include "log.h"
#   define msg(aRGS...) LOG_D(PHY, ##aRGS)
# else
#   define msg printf
# endif
#endif
//use msg in the real-time thread context
#define msg_nrt printf
//use msg_nrt in the non real-time context (for initialization, ...)
#ifndef malloc16
#  ifdef __AVX2__
#    define malloc16(x) memalign(32,x)
#  else
#    define malloc16(x) memalign(16,x)
#  endif
#endif
#define free16(y,x) free(y)
#define bigmalloc malloc
#define bigmalloc16 malloc16
#define openair_free(y,x) free((y))
#define PAGE_SIZE 4096

//#define RX_NB_TH_MAX 3
//#define RX_NB_TH 3

#ifdef NR_UNIT_TEST
  #define FILE_NAME                " "
  #define LINE_FILE                (0)
  #define NR_TST_PHY_PRINTF(...)   printf(__VA_ARGS__)
#else
  #define FILE_NAME                (__FILE__)
  #define LINE_FILE                (__LINE__)
  #define NR_TST_PHY_PRINTF(...)
#endif

//#ifdef SHRLIBDEV
//extern int rxrescale;
//#define RX_IQRESCALELEN rxrescale
//#else
//#define RX_IQRESCALELEN 15
//#endif

//! \brief Allocate \c size bytes of memory on the heap with alignment 16 and zero it afterwards.
//! If no more memory is available, this function will terminate the program with an assertion error.
/*static inline void* malloc16_clear( size_t size )
{
#ifdef __AVX2__
  void* ptr = memalign(32, size);
#else
  void* ptr = memalign(16, size);
#endif
  DevAssert(ptr);
  memset( ptr, 0, size );
  return ptr;
}*/



#define PAGE_MASK 0xfffff000
#define virt_to_phys(x) (x)

#define openair_sched_exit() exit(-1)


//#define max(a,b)  ((a)>(b) ? (a) : (b))
//#define min(a,b)  ((a)<(b) ? (a) : (b))


#define bzero(s,n) (memset((s),0,(n)))

#define cmax(a,b)  ((a>b) ? (a) : (b))
#define cmin(a,b)  ((a<b) ? (a) : (b))

#define cmax3(a,b,c) ((cmax(a,b)>c) ? (cmax(a,b)) : (c))

/// suppress compiler warning for unused arguments
#define UNUSED(x) (void)x;


#include "impl_defs_top.h"
#include "impl_defs_nr.h"

#include "PHY/TOOLS/time_meas.h"
#include "PHY/CODING/coding_defs.h"
#include "PHY/TOOLS/tools_defs.h"
#include "platform_types.h"
#include "NR_UE_TRANSPORT/nr_transport_ue.h"

#if defined(UPGRADE_RAT_NR)

#include "PHY/NR_REFSIG/ss_pbch_nr.h"

#endif

#include "PHY/NR_UE_TRANSPORT/dci_nr.h"
//#include "PHY/LTE_TRANSPORT/defs.h"
//#include "PHY/NR_UE_TRANSPORT/defs_nr.h"
#include <pthread.h>

#include "targets/ARCH/COMMON/common_lib.h"

#include "NR_IF_Module.h"

//#if defined(UPGRADE_RAT_NR)
#if 1
                        /* see 38.321  Table 7.1-2  RNTI usage */
typedef enum {          /* Type for Radio Network Temporary Identifier */
  C_RNTI_NR = 0,        /* Cell RNTI */
  Temporary_C_RNTI_NR,  /* Temporary C-RNTI */
  CS_RNTI_NR,           /* Configured Scheduling RNTI */
  P_RNTI_NR,            /* Paging RNTI */
  SI_RNTI_NR,           /* System information RNTI */
  RA_RNTI_NR,           /* Random Access RNTI */
  TPC_CS_RNTI_NR,       /* configured scheduling uplink power control */
  TPC_PUCCH_RNTI_NR,    /* PUCCH power control */
  TPC_PUSCH_RNTI_NR,    /* PUSCH power control */
  TPC_SRS_RNTI_NR,      /* SRS trigger and power control */
  INT_RNTI_NR,          /* Indication pre-emption in DL */
  SFI_RNTI_NR,          /* Slot Format Indication on the given cell */
  SP_CSI_RNTI_NR        /* Semipersistent CSI reporting on PUSCH */
} nr_rnti_type_t;

#endif

/// Context data structure for RX/TX portion of subframe processing
typedef struct {
  /// index of the current UE RX/TX proc
  int                  proc_id;
  /// Component Carrier index
  uint8_t              CC_id;
  /// timestamp transmitted to HW
  openair0_timestamp timestamp_tx;
//#ifdef UE_NR_PHY_DEMO
  /// NR TTI index within subframe_tx [0 .. ttis_per_subframe - 1] to act upon for transmission
  int nr_tti_tx;
  /// NR TTI index within subframe_rx [0 .. ttis_per_subframe - 1] to act upon for reception
  int nr_tti_rx;
//#endif
  /// subframe to act upon for transmission
  int subframe_tx;
  /// subframe to act upon for reception
  int subframe_rx;
  /// frame to act upon for transmission
  int frame_tx;
  /// frame to act upon for reception
  int frame_rx;
  /// \brief Instance count for RXn-TXnp4 processing thread.
  /// \internal This variable is protected by \ref mutex_rxtx.
  int instance_cnt_rxtx;
  /// pthread structure for RXn-TXnp4 processing thread
  pthread_t pthread_rxtx;
  /// pthread attributes for RXn-TXnp4 processing thread
  pthread_attr_t attr_rxtx;
  /// condition variable for tx processing thread
  pthread_cond_t cond_rxtx;
  /// mutex for RXn-TXnp4 processing thread
  pthread_mutex_t mutex_rxtx;
  /// scheduling parameters for RXn-TXnp4 thread
  struct sched_param sched_param_rxtx;

  /// internal This variable is protected by ref mutex_fep_slot1.
  //int instance_cnt_slot0_dl_processing;
  int instance_cnt_slot1_dl_processing;
  /// pthread descriptor fep_slot1 thread
  //pthread_t pthread_slot0_dl_processing;
  pthread_t pthread_slot1_dl_processing;
  /// pthread attributes for fep_slot1 processing thread
 // pthread_attr_t attr_slot0_dl_processing;
  pthread_attr_t attr_slot1_dl_processing;
  /// condition variable for UE fep_slot1 thread;
  //pthread_cond_t cond_slot0_dl_processing;
  pthread_cond_t cond_slot1_dl_processing;
  /// mutex for UE synch thread
  //pthread_mutex_t mutex_slot0_dl_processing;
  pthread_mutex_t mutex_slot1_dl_processing;
  //int instance_cnt_slot0_dl_processing;
  int instance_cnt_dlsch_td;
  /// pthread descriptor fep_slot1 thread
  //pthread_t pthread_slot0_dl_processing;
  pthread_t pthread_dlsch_td;
  /// pthread attributes for fep_slot1 processing thread
 // pthread_attr_t attr_slot0_dl_processing;
  pthread_attr_t attr_dlsch_td;
  /// condition variable for UE fep_slot1 thread;
  //pthread_cond_t cond_slot0_dl_processing;
  pthread_cond_t cond_dlsch_td;
  /// mutex for UE synch thread
  //pthread_mutex_t mutex_slot0_dl_processing;
  pthread_mutex_t mutex_dlsch_td;
  //
  uint8_t chan_est_pilot0_slot1_available;
  uint8_t chan_est_slot1_available;
  uint8_t llr_slot1_available;
  uint8_t dci_slot0_available;
  uint8_t first_symbol_available;
  uint8_t decoder_thread_available;
  uint8_t decoder_main_available;
  uint8_t decoder_switch;
  int counter_decoder;
  uint8_t channel_level;
  int eNB_id;
  int harq_pid;
  int llr8_flag;
  /// scheduling parameters for fep_slot1 thread
  struct sched_param sched_param_fep_slot1;

  int sub_frame_start;
  int sub_frame_step;
  unsigned long long gotIQs;
  uint8_t decoder_thread_available1;
  int instance_cnt_dlsch_td1;
  /// pthread descriptor fep_slot1 thread
  //pthread_t pthread_slot0_dl_processing;
  pthread_t pthread_dlsch_td1;
  /// pthread attributes for fep_slot1 processing thread
 // pthread_attr_t attr_slot0_dl_processing;
  pthread_attr_t attr_dlsch_td1;
  /// condition variable for UE fep_slot1 thread;
  //pthread_cond_t cond_slot0_dl_processing;
  pthread_cond_t cond_dlsch_td1;
  /// mutex for UE synch thread
  //pthread_mutex_t mutex_slot0_dl_processing;
  pthread_mutex_t mutex_dlsch_td1;
  int dci_err_cnt;
} UE_nr_rxtx_proc_t;

/// Context data structure for eNB subframe processing
typedef struct {
  /// Component Carrier index
  uint8_t              CC_id;
  /// Last RX timestamp
  openair0_timestamp timestamp_rx;
  /// pthread attributes for main UE thread
  pthread_attr_t attr_ue;
  /// scheduling parameters for main UE thread
  struct sched_param sched_param_ue;
  /// pthread descriptor main UE thread
  pthread_t pthread_ue;
  /// \brief Instance count for synch thread.
  /// \internal This variable is protected by \ref mutex_synch.
  int instance_cnt_synch;
  /// pthread attributes for synch processing thread
  pthread_attr_t attr_synch;
  /// scheduling parameters for synch thread
  struct sched_param sched_param_synch;
  /// pthread descriptor synch thread
  pthread_t pthread_synch;
  /// condition variable for UE synch thread;
  pthread_cond_t cond_synch;
  /// mutex for UE synch thread
  pthread_mutex_t mutex_synch;
  /// set of scheduling variables RXn-TXnp4 threads
  UE_nr_rxtx_proc_t proc_rxtx[RX_NB_TH];
} UE_nr_proc_t;

typedef enum {
  NR_PBCH_EST=0,
  NR_PDCCH_EST,
  NR_PDSCH_EST,
  NR_SSS_EST,
} NR_CHANNEL_EST_t;

#define debug_msg if (((mac_xface->frame%100) == 0) || (mac_xface->frame < 50)) msg

typedef struct {
  //unsigned int   rx_power[NUMBER_OF_CONNECTED_eNB_MAX][NB_ANTENNAS_RX];     //! estimated received signal power (linear)
  //unsigned short rx_power_dB[NUMBER_OF_CONNECTED_eNB_MAX][NB_ANTENNAS_RX];  //! estimated received signal power (dB)
  //unsigned short rx_avg_power_dB[NUMBER_OF_CONNECTED_eNB_MAX];              //! estimated avg received signal power (dB)

  // RRC measurements
  uint32_t rssi;
  int n_adj_cells;
  unsigned int adj_cell_id[6];
  uint32_t rsrq[7];
  uint32_t rsrp[7];
  float rsrp_filtered[7]; // after layer 3 filtering
  float rsrq_filtered[7];
  // common measurements
  //! estimated noise power (linear)
  unsigned int   n0_power[NB_ANTENNAS_RX];
  //! estimated noise power (dB)
  unsigned short n0_power_dB[NB_ANTENNAS_RX];
  //! total estimated noise power (linear)
  unsigned int   n0_power_tot;
  //! total estimated noise power (dB)
  unsigned short n0_power_tot_dB;
  //! average estimated noise power (linear)
  unsigned int   n0_power_avg;
  //! average estimated noise power (dB)
  unsigned short n0_power_avg_dB;
  //! total estimated noise power (dBm)
  short n0_power_tot_dBm;

  // UE measurements
  //! estimated received spatial signal power (linear)
  int            rx_spatial_power[NUMBER_OF_CONNECTED_eNB_MAX][2][2];
  //! estimated received spatial signal power (dB)
  unsigned short rx_spatial_power_dB[NUMBER_OF_CONNECTED_eNB_MAX][2][2];

  /// estimated received signal power (sum over all TX antennas)
  //int            wideband_cqi[NUMBER_OF_CONNECTED_eNB_MAX][NB_ANTENNAS_RX];
  int            rx_power[NUMBER_OF_CONNECTED_eNB_MAX][NB_ANTENNAS_RX];
  /// estimated received signal power (sum over all TX antennas)
  //int            wideband_cqi_dB[NUMBER_OF_CONNECTED_eNB_MAX][NB_ANTENNAS_RX];
  unsigned short rx_power_dB[NUMBER_OF_CONNECTED_eNB_MAX][NB_ANTENNAS_RX];

  /// estimated received signal power (sum over all TX/RX antennas)
  int            rx_power_tot[NUMBER_OF_CONNECTED_eNB_MAX]; //NEW
  /// estimated received signal power (sum over all TX/RX antennas)
  unsigned short rx_power_tot_dB[NUMBER_OF_CONNECTED_eNB_MAX]; //NEW

  //! estimated received signal power (sum of all TX/RX antennas, time average)
  int            rx_power_avg[NUMBER_OF_CONNECTED_eNB_MAX];
  //! estimated received signal power (sum of all TX/RX antennas, time average, in dB)
  unsigned short rx_power_avg_dB[NUMBER_OF_CONNECTED_eNB_MAX];

  /// SINR (sum of all TX/RX antennas, in dB)
  int            wideband_cqi_tot[NUMBER_OF_CONNECTED_eNB_MAX];
  /// SINR (sum of all TX/RX antennas, time average, in dB)
  int            wideband_cqi_avg[NUMBER_OF_CONNECTED_eNB_MAX];

  //! estimated rssi (dBm)
  short          rx_rssi_dBm[NUMBER_OF_CONNECTED_eNB_MAX];
  //! estimated correlation (wideband linear) between spatial channels (computed in dlsch_demodulation)
  int            rx_correlation[NUMBER_OF_CONNECTED_eNB_MAX][2];
  //! estimated correlation (wideband dB) between spatial channels (computed in dlsch_demodulation)
  int            rx_correlation_dB[NUMBER_OF_CONNECTED_eNB_MAX][2];

  /// Wideband CQI (sum of all RX antennas, in dB, for precoded transmission modes (3,4,5,6), up to 4 spatial streams)
  int            precoded_cqi_dB[NUMBER_OF_CONNECTED_eNB_MAX+1][4];
  /// Subband CQI per RX antenna (= SINR)
  int            subband_cqi[NUMBER_OF_CONNECTED_eNB_MAX][NB_ANTENNAS_RX][NUMBER_OF_SUBBANDS_MAX];
  /// Total Subband CQI  (= SINR)
  int            subband_cqi_tot[NUMBER_OF_CONNECTED_eNB_MAX][NUMBER_OF_SUBBANDS_MAX];
  /// Subband CQI in dB (= SINR dB)
  int            subband_cqi_dB[NUMBER_OF_CONNECTED_eNB_MAX][NB_ANTENNAS_RX][NUMBER_OF_SUBBANDS_MAX];
  /// Total Subband CQI
  int            subband_cqi_tot_dB[NUMBER_OF_CONNECTED_eNB_MAX][NUMBER_OF_SUBBANDS_MAX];
  /// Wideband PMI for each RX antenna
  int            wideband_pmi_re[NUMBER_OF_CONNECTED_eNB_MAX][NB_ANTENNAS_RX];
  /// Wideband PMI for each RX antenna
  int            wideband_pmi_im[NUMBER_OF_CONNECTED_eNB_MAX][NB_ANTENNAS_RX];
  ///Subband PMI for each RX antenna
  int            subband_pmi_re[NUMBER_OF_CONNECTED_eNB_MAX][NUMBER_OF_SUBBANDS_MAX][NB_ANTENNAS_RX];
  ///Subband PMI for each RX antenna
  int            subband_pmi_im[NUMBER_OF_CONNECTED_eNB_MAX][NUMBER_OF_SUBBANDS_MAX][NB_ANTENNAS_RX];
  /// chosen RX antennas (1=Rx antenna 1, 2=Rx antenna 2, 3=both Rx antennas)
  unsigned char           selected_rx_antennas[NUMBER_OF_CONNECTED_eNB_MAX][NUMBER_OF_SUBBANDS_MAX];
  /// Wideband Rank indication
  unsigned char  rank[NUMBER_OF_CONNECTED_eNB_MAX];
  /// Number of RX Antennas
  unsigned char  nb_antennas_rx;
  /// DLSCH error counter
  // short          dlsch_errors;

} PHY_NR_MEASUREMENTS;

typedef struct {

	  /// \brief Holds the received data in the frequency domain.
	  /// - first index: rx antenna [0..nb_antennas_rx[
	  /// - second index: symbol [0..28*ofdm_symbol_size[
	  int32_t **rxdataF;

	  /// \brief Hold the channel estimates in frequency domain.
	  /// - first index: eNB id [0..6] (hard coded)
	  /// - second index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
	  /// - third index: samples? [0..symbols_per_tti*(ofdm_symbol_size+LTE_CE_FILTER_LENGTH)[
	  int32_t **dl_ch_estimates[7];

	  /// \brief Hold the channel estimates in time domain (used for tracking).
	  /// - first index: eNB id [0..6] (hard coded)
	  /// - second index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
	  /// - third index: samples? [0..2*ofdm_symbol_size[
	  int32_t **dl_ch_estimates_time[7];
}NR_UE_COMMON_PER_THREAD;

typedef struct {
  /// \brief Holds the transmit data in time domain.
  /// For IFFT_FPGA this points to the same memory as PHY_vars->tx_vars[a].TX_DMA_BUFFER.
  /// - first index: tx antenna [0..nb_antennas_tx[
  /// - second index: sample [0..FRAME_LENGTH_COMPLEX_SAMPLES[
  int32_t **txdata;
  /// \brief Holds the transmit data in the frequency domain.
  /// For IFFT_FPGA this points to the same memory as PHY_vars->rx_vars[a].RX_DMA_BUFFER.
  /// - first index: tx antenna [0..nb_antennas_tx[
  /// - second index: sample [0..FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX[
  int32_t **txdataF;

  /// \brief Holds the received data in time domain.
  /// Should point to the same memory as PHY_vars->rx_vars[a].RX_DMA_BUFFER.
  /// - first index: rx antenna [0..nb_antennas_rx[
  /// - second index: sample [0..FRAME_LENGTH_COMPLEX_SAMPLES+2048[
  int32_t **rxdata;

  NR_UE_COMMON_PER_THREAD common_vars_rx_data_per_thread[RX_NB_TH_MAX];

  /// holds output of the sync correlator
  int32_t *sync_corr;
  /// estimated frequency offset (in radians) for all subcarriers
  int32_t freq_offset;
  /// eNb_id user is synched to
  int32_t eNb_id;
} NR_UE_COMMON;

typedef struct {
  /// \brief Received frequency-domain signal after extraction.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **rxdataF_ext;
  /// \brief Received frequency-domain ue specific pilots.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..12*N_RB_DL[
  int32_t **rxdataF_uespec_pilots;
  /// \brief Received frequency-domain signal after extraction and channel compensation.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **rxdataF_comp0;
  /// \brief Received frequency-domain signal after extraction and channel compensation for the second stream. For the SIC receiver we need to store the history of this for each harq process and round
  /// - first index: ? [0..7] (hard coded) accessed via \c harq_pid
  /// - second index: ? [0..7] (hard coded) accessed via \c round
  /// - third index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - fourth index: ? [0..168*N_RB_DL[
  int32_t **rxdataF_comp1[8][8];
  /// \brief Downlink channel estimates extracted in PRBS.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **dl_ch_estimates_ext;
  /// \brief Downlink cross-correlation of MIMO channel estimates (unquantized PMI) extracted in PRBS. For the SIC receiver we need to store the history of this for each harq process and round
  /// - first index: ? [0..7] (hard coded) accessed via \c harq_pid
  /// - second index: ? [0..7] (hard coded) accessed via \c round
  /// - third index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - fourth index: ? [0..168*N_RB_DL[
  int32_t **dl_ch_rho_ext[8][8];
  /// \brief Downlink beamforming channel estimates in frequency domain.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: samples? [0..symbols_per_tti*(ofdm_symbol_size+LTE_CE_FILTER_LENGTH)[
  int32_t **dl_bf_ch_estimates;
  /// \brief Downlink beamforming channel estimates.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **dl_bf_ch_estimates_ext;
  /// \brief Downlink cross-correlation of MIMO channel estimates (unquantized PMI) extracted in PRBS.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **dl_ch_rho2_ext;
  /// \brief Downlink PMIs extracted in PRBS and grouped in subbands.
  /// - first index: ressource block [0..N_RB_DL[
  uint8_t *pmi_ext;
  /// \brief Magnitude of Downlink Channel first layer (16QAM level/First 64QAM level).
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **dl_ch_mag0;
  /// \brief Magnitude of Downlink Channel second layer (16QAM level/First 64QAM level).
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **dl_ch_mag1[8][8];
  /// \brief Magnitude of Downlink Channel, first layer (2nd 64QAM level).
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **dl_ch_magb0;
  /// \brief Magnitude of Downlink Channel second layer (2nd 64QAM level).
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **dl_ch_magb1[8][8];
  /// \brief Cross-correlation of two eNB signals.
  /// - first index: rx antenna [0..nb_antennas_rx[
  /// - second index: symbol [0..]
  int32_t **rho;
  /// never used... always send dl_ch_rho_ext instead...
  int32_t **rho_i;
  /// \brief Pointers to llr vectors (2 TBs).
  /// - first index: ? [0..1] (hard coded)
  /// - second index: ? [0..1179743] (hard coded)
  int16_t *llr[2];
  /// \f$\log_2(\max|H_i|^2)\f$
  int16_t log2_maxh;
    /// \f$\log_2(\max|H_i|^2)\f$ //this is for TM3-4 layer1 channel compensation
  int16_t log2_maxh0;
    /// \f$\log_2(\max|H_i|^2)\f$ //this is for TM3-4 layer2 channel commpensation
  int16_t log2_maxh1;
  /// \brief LLR shifts for subband scaling.
  /// - first index: ? [0..168*N_RB_DL[
  uint8_t *llr_shifts;
  /// \brief Pointer to LLR shifts.
  /// - first index: ? [0..168*N_RB_DL[
  uint8_t *llr_shifts_p;
  /// \brief Pointers to llr vectors (128-bit alignment).
  /// - first index: ? [0..0] (hard coded)
  /// - second index: ? [0..]
  int16_t **llr128;
  /// \brief Pointers to llr vectors (128-bit alignment).
  /// - first index: ? [0..0] (hard coded)
  /// - second index: ? [0..]
  int16_t **llr128_2ndstream;
  //uint32_t *rb_alloc;
  //uint8_t Qm[2];
  //MIMO_mode_t mimo_mode;
  // llr offset per ofdm symbol
  uint32_t llr_offset[14];
  // llr length per ofdm symbol
  uint32_t llr_length[14];
} NR_UE_PDSCH;

typedef struct {
  /// \brief Received frequency-domain signal after extraction.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..]
  int32_t **rxdataF_ext;
  /// \brief Received frequency-domain signal after extraction and channel compensation.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..]
  double **rxdataF_comp;
  /// \brief Downlink channel estimates extracted in PRBS.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..]
  int32_t **dl_ch_estimates_ext;
  ///  \brief Downlink cross-correlation of MIMO channel estimates (unquantized PMI) extracted in PRBS.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..]
  double **dl_ch_rho_ext;
  /// \brief Downlink PMIs extracted in PRBS and grouped in subbands.
  /// - first index: ressource block [0..N_RB_DL[
  uint8_t *pmi_ext;
  /// \brief Magnitude of Downlink Channel (16QAM level/First 64QAM level).
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..]
  double **dl_ch_mag;
  /// \brief Magnitude of Downlink Channel (2nd 64QAM level).
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..]
  double **dl_ch_magb;
  /// \brief Cross-correlation of two eNB signals.
  /// - first index: rx antenna [0..nb_antennas_rx[
  /// - second index: ? [0..]
  double **rho;
  /// never used... always send dl_ch_rho_ext instead...
  double **rho_i;
  /// \brief Pointers to llr vectors (2 TBs).
  /// - first index: ? [0..1] (hard coded)
  /// - second index: ? [0..1179743] (hard coded)
  int16_t *llr[2];
  /// \f$\log_2(\max|H_i|^2)\f$
  uint8_t log2_maxh;
  /// \brief Pointers to llr vectors (128-bit alignment).
  /// - first index: ? [0..0] (hard coded)
  /// - second index: ? [0..]
  int16_t **llr128;
  //uint32_t *rb_alloc;
  //uint8_t Qm[2];
  //MIMO_mode_t mimo_mode;
} NR_UE_PDSCH_FLP;

#define NR_PDCCH_DEFS_NR_UE
#define NR_NBR_CORESET_ACT_BWP 3      // The number of CoreSets per BWP is limited to 3 (including initial CORESET: ControlResourceId 0)
#define NR_NBR_SEARCHSPACE_ACT_BWP 10 // The number of SearchSpaces per BWP is limited to 10 (including initial SEARCHSPACE: SearchSpaceId 0)
#ifdef NR_PDCCH_DEFS_NR_UE

#define MAX_NR_DCI_DECODED_SLOT 10
#define NBR_NR_FORMATS         8
#define NBR_NR_DCI_FIELDS     56

#define IDENTIFIER_DCI_FORMATS           0
#define CARRIER_IND                      1
#define SUL_IND_0_1                      2
#define SLOT_FORMAT_IND                  3
#define PRE_EMPTION_IND                  4
#define TPC_CMD_NUMBER                   5
#define BLOCK_NUMBER                     6
#define BANDWIDTH_PART_IND               7
#define SHORT_MESSAGE_IND                8
#define SHORT_MESSAGES                   9
#define FREQ_DOM_RESOURCE_ASSIGNMENT_UL 10
#define FREQ_DOM_RESOURCE_ASSIGNMENT_DL 11
#define TIME_DOM_RESOURCE_ASSIGNMENT    12
#define VRB_TO_PRB_MAPPING              13
#define PRB_BUNDLING_SIZE_IND           14
#define RATE_MATCHING_IND               15
#define ZP_CSI_RS_TRIGGER               16
#define FREQ_HOPPING_FLAG               17
#define TB1_MCS                         18
#define TB1_NDI                         19
#define TB1_RV                          20
#define TB2_MCS                         21
#define TB2_NDI                         22
#define TB2_RV                          23
#define MCS                             24
#define NDI                             25
#define RV                              26
#define HARQ_PROCESS_NUMBER             27
#define DAI_                            28
#define FIRST_DAI                       29
#define SECOND_DAI                      30
#define TB_SCALING                      31
#define TPC_PUSCH                       32
#define TPC_PUCCH                       33
#define PUCCH_RESOURCE_IND              34
#define PDSCH_TO_HARQ_FEEDBACK_TIME_IND 35
//#define SHORT_MESSAGE_IND             33
#define SRS_RESOURCE_IND                36
#define PRECOD_NBR_LAYERS               37
#define ANTENNA_PORTS                   38
#define TCI                             39
#define SRS_REQUEST                     40
#define TPC_CMD_NUMBER_FORMAT2_3        41
#define CSI_REQUEST                     42
#define CBGTI                           43
#define CBGFI                           44
#define PTRS_DMRS                       45
#define BETA_OFFSET_IND                 46
#define DMRS_SEQ_INI                    47
#define UL_SCH_IND                      48
#define PADDING_NR_DCI                  49
#define SUL_IND_0_0                     50
#define RA_PREAMBLE_INDEX               51
#define SUL_IND_1_0                     52
#define SS_PBCH_INDEX                   53
#define PRACH_MASK_INDEX                54
#define RESERVED_NR_DCI                 55


typedef enum {
  _format_0_0_found=0,
  _format_0_1_found=1,
  _format_1_0_found=2,
  _format_1_1_found=3,
  _format_2_0_found=4,
  _format_2_1_found=5,
  _format_2_2_found=6,
  _format_2_3_found=7} format_found_t;
#define TOTAL_NBR_SCRAMBLED_VALUES 13
#define _C_RNTI_           0
#define _CS_RNTI_          1
#define _NEW_RNTI_         2
#define _TC_RNTI_          3
#define _P_RNTI_           4
#define _SI_RNTI_          5
#define _RA_RNTI_          6
#define _SP_CSI_RNTI_      7
#define _SFI_RNTI_         8
#define _INT_RNTI_         9
#define _TPC_PUSCH_RNTI_  10
#define _TPC_PUCCH_RNTI_  11
#define _TPC_SRS_RNTI_    12
  typedef enum {
      _c_rnti         = _C_RNTI_,
      _cs_rnti        = _CS_RNTI_,
      _new_rnti       = _NEW_RNTI_,
      _tc_rnti        = _TC_RNTI_,
      _p_rnti         = _P_RNTI_,
      _si_rnti        = _SI_RNTI_,
      _ra_rnti        = _RA_RNTI_,
      _sp_csi_rnti    = _SP_CSI_RNTI_,
      _sfi_rnti       = _SFI_RNTI_,
      _int_rnti       = _INT_RNTI_,
      _tpc_pusch_rnti = _TPC_PUSCH_RNTI_,
      _tpc_pucch_rnti = _TPC_PUCCH_RNTI_,
      _tpc_srs_rnti   = _TPC_SRS_RNTI_} crc_scrambled_t;




typedef enum {bundle_n2=2,bundle_n3=3,bundle_n6=6} NR_UE_CORESET_REG_bundlesize_t;

typedef enum {interleave_n2=2,interleave_n3=3,interleave_n6=6} NR_UE_CORESET_interleaversize_t;

typedef struct {
  //Corresponds to L1 parameter 'CORESET-REG-bundle-size' (see 38.211, section FFS_Section)
  NR_UE_CORESET_REG_bundlesize_t reg_bundlesize;
  //Corresponds to L1 parameter 'CORESET-interleaver-size' (see 38.211, 38.213, section FFS_Section)
  NR_UE_CORESET_interleaversize_t interleaversize;
  //Corresponds to L1 parameter 'CORESET-shift-index' (see 38.211, section 7.3.2.2)
  int shiftIndex;
} NR_UE_CORESET_CCE_REG_MAPPING_t;

typedef enum {allContiguousRBs=0,sameAsREGbundle=1} NR_UE_CORESET_precoder_granularity_t;

typedef struct {
  /*
   * define CORESET structure according to 38.331
   *
   * controlResourceSetId: 		Corresponds to L1 parameter 'CORESET-ID'
   * 							Value 0 identifies the common CORESET configured in MIB and in ServingCellConfigCommon
   *                       		Values 1..maxNrofControlResourceSets-1 identify CORESETs configured by dedicated signalling
   * frequencyDomainResources: 	BIT STRING (SIZE (45))
   * 							Corresponds to L1 parameter 'CORESET-freq-dom'(see 38.211, section 7.3.2.2)
   * 							Frequency domain resources for the CORESET. Each bit corresponds a group of 6 RBs, with grouping starting from PRB 0,
   * 							which is fully contained in the bandwidth part within which the CORESET is configured.
   * duration:					INTEGER (1..maxCoReSetDuration)
   * 							Corresponds to L1 parameter 'CORESET-time-duration' (see 38.211, section 7.3.2.2FFS_Section)
   * 							Contiguous time duration of the CORESET in number of symbols
   * cce-REG-MappingType:		interleaved
   *								reg-BundleSize: ENUMERATED {n2, n3, n6}
   *								interleaverSize: ENUMERATED {n2, n3, n6}
   *								shiftIndex: INTEGER
   *							nonInterleaved NULL
   * precoderGranularity:		ENUMERATED {sameAsREG-bundle, allContiguousRBs}
   * 							Corresponds to L1 parameter 'CORESET-precoder-granuality' (see 38.211, sections 7.3.2.2 and 7.4.1.3.2)
   * tci-StatesPDCCH:			SEQUENCE(SIZE (1..maxNrofTCI-StatesPDCCH)) OF TCI-StateId OPTIONAL
   * 							A subset of the TCI states defined in TCI-States used for providing QCL relationships between the DL RS(s)
   * 							in one RS Set (TCI-State) and the PDCCH DMRS ports.
   * 							Corresponds to L1 parameter 'TCI-StatesPDCCH' (see 38.214, section FFS_Section)
   * tci-PresentInDCI:			ENUMERATED {enabled} OPTIONAL
   * 							Corresponds to L1 parameter 'TCI-PresentInDCI' (see 38,213, section 5.1.5)
   * pdcch-DMRS-ScramblingID:	BIT STRING (SIZE (16)) OPTIONAL
   * 							PDCCH DMRS scrambling initalization.
   * 							Corresponds to L1 parameter 'PDCCH-DMRS-Scrambling-ID' (see 38.214, section 5.1)
   * 							When the field is absent the UE applies the value '0'.
   */
  int controlResourceSetId;
  uint64_t frequencyDomainResources;
  int duration;
  NR_UE_CORESET_CCE_REG_MAPPING_t cce_reg_mappingType;
  NR_UE_CORESET_precoder_granularity_t precoderGranularity;
  int tciStatesPDCCH;
  int tciPresentInDCI;
  uint16_t pdcchDMRSScramblingID;
  uint16_t rb_offset;
} NR_UE_PDCCH_CORESET;

// Slots for PDCCH Monitoring configured as periodicity and offset
typedef enum {nr_sl1=1,nr_sl2=2,nr_sl4=4,nr_sl5=5,nr_sl8=8,nr_sl10=10,nr_sl16=16,nr_sl20=20} NR_UE_SLOT_PERIOD_OFFSET_t;
typedef enum {nc0=0,nc1=1,nc2=2,nc3=3,nc4=4,nc5=5,nc6=6,nc8=8} NR_UE_SEARCHSPACE_nbrCAND_t;
typedef enum {nsfi1=1,nsfi2=2} NR_UE_SEARCHSPACE_nbrCAND_SFI_t;
typedef enum {n2_3_1=1,n2_3_2=2} NR_UE_SEARCHSPACE_nbrCAND_2_3_t;
typedef enum {cformat0_0_and_1_0=0,cformat2_0=2,cformat2_1=3,cformat2_2=4,cformat2_3=5} NR_UE_SEARCHSPACE_CSS_DCI_FORMAT_t;
typedef enum {uformat0_0_and_1_0=0,uformat0_1_and_1_1=1} NR_UE_SEARCHSPACE_USS_DCI_FORMAT_t;
// Monitoring periodicity of SRS PDCCH in number of slots for DCI format 2-3
// Corresponds to L1 parameter 'SRS-Num-PDCCH-cand' (see 38.212, 38.213, section 7.3.1, 11.3)
typedef enum {mp1=1,mp2=2,mp4=4,mp5=5,mp8=8,mp10=10,mp16=16,mp20=20} NR_UE_SEARCHSPACE_MON_PERIOD_t;
//typedef enum {n1=1,n2=2} NR_UE_SEARCHSPACE_nbrCAND_2_3_t;
             // The number of PDCCH candidates for DCI format 2-3 for the configured aggregation level.
             // Corresponds to L1 parameter 'SRS-Num-PDCCH-cand' (see 38.212, 38.213, section 7.3.1, 11.3)
typedef enum {common=0,ue_specific=1} NR_SEARCHSPACE_TYPE_t;

typedef struct {

/*
 * searchSpaceType:      Indicates whether this is a common search space (present) or a UE specific search space (CHOICE)
 *                       as well as DCI formats to monitor for (description in struct NR_UE_PDCCH_SEARCHSPACE_TYPE
 *      common:          Configures this search space as common search space (CSS) and DCI formats to monitor
 *      ue-Specific:     Configures this search space as UE specific search space (USS)
 *                       The UE monitors the DCI format with CRC scrambled by
 *                       C-RNTI, CS-RNTI (if configured), TC-RNTI (if a certain condition is met),
 *                       and SP-CSI-RNTI (if configured)
 */

  NR_SEARCHSPACE_TYPE_t type;
  NR_UE_SEARCHSPACE_CSS_DCI_FORMAT_t  common_dci_formats;
  //NR_UE_SEARCHSPACE_nbrCAND_t nrofCandidates_SFI_aggr_level[5]; // FIXME! A table of five enum elements
  NR_UE_SEARCHSPACE_nbrCAND_SFI_t sfi_nrofCandidates_aggrlevel1;
  NR_UE_SEARCHSPACE_nbrCAND_SFI_t sfi_nrofCandidates_aggrlevel2;
  NR_UE_SEARCHSPACE_nbrCAND_SFI_t sfi_nrofCandidates_aggrlevel4;
  NR_UE_SEARCHSPACE_nbrCAND_SFI_t sfi_nrofCandidates_aggrlevel8;
  NR_UE_SEARCHSPACE_nbrCAND_SFI_t sfi_nrofCandidates_aggrlevel16;
  NR_UE_SEARCHSPACE_MON_PERIOD_t  srs_monitoringPeriodicity2_3;
  NR_UE_SEARCHSPACE_nbrCAND_2_3_t srs_nrofCandidates;
  NR_UE_SEARCHSPACE_USS_DCI_FORMAT_t  ue_specific_dci_formats;

} NR_UE_PDCCH_SEARCHSPACE_TYPE;

typedef struct {
/*
 * define SearchSpace structure according to 38.331
 *
 * searchSpaceId:        Identity of the search space. SearchSpaceId = 0 identifies the SearchSpace configured via PBCH (MIB)
 *                       The searchSpaceId is unique among the BWPs of a Serving Cell
 * controlResourceSetId: CORESET applicable for this SearchSpace
 *                       0 identifies the common CORESET configured in MIB
 *                       1..maxNrofControlResourceSets-1 identify CORESETs configured by dedicated signalling
 * monitoringSlotPeriodicityAndOffset:
 *                       Slots for PDCCH Monitoring configured as periodicity and offset.
 *                       Corresponds to L1 parameters 'Montoring-periodicity-PDCCH-slot' and
 *                       'Montoring-offset-PDCCH-slot' (see 38.213, section 10)
 * monitoringSymbolsWithinSlot:
 *                       Symbols for PDCCH monitoring in the slots configured for PDCCH monitoring
 *                       The most significant (left) bit represents the first OFDM in a slot
 *
 * nrofCandidates:       Number of PDCCH candidates per aggregation level
 *
 * searchSpaceType:      Indicates whether this is a common search space (present) or a UE specific search space
 *                       as well as DCI formats to monitor for (description in struct NR_UE_PDCCH_SEARCHSPACE_TYPE
 *      common:          Configures this search space as common search space (CSS) and DCI formats to monitor
 *      ue-Specific:     Configures this search space as UE specific search space (USS)
 *                       The UE monitors the DCI format with CRC scrambled by
 *                       C-RNTI, CS-RNTI (if configured), TC-RNTI (if a certain condition is met),
 *                       and SP-CSI-RNTI (if configured)
 */
  // INTEGER (0..maxNrofSearchSpaces-1) (0..40-1)
  int searchSpaceId;
  int controlResourceSetId;
  // FIXME! Verify type to be used for this parameter (sl1, sl2, sl4, sl5, sl8, sl10, sl16, sl20). Maybe enum.
  NR_UE_SLOT_PERIOD_OFFSET_t monitoringSlotPeriodicityAndOffset;
  int monitoringSlotPeriodicityAndOffset_offset;
  // bit string size 14. Bitmap to indicate symbols within slot where PDCCH has to be monitored
  // the MSB (left) bit represents first OFDM in slot
  uint16_t monitoringSymbolWithinSlot;
  NR_UE_SEARCHSPACE_nbrCAND_t nrofCandidates_aggrlevel1;
  NR_UE_SEARCHSPACE_nbrCAND_t nrofCandidates_aggrlevel2;
  NR_UE_SEARCHSPACE_nbrCAND_t nrofCandidates_aggrlevel4;
  NR_UE_SEARCHSPACE_nbrCAND_t nrofCandidates_aggrlevel8;
  NR_UE_SEARCHSPACE_nbrCAND_t nrofCandidates_aggrlevel16;
  NR_UE_PDCCH_SEARCHSPACE_TYPE searchSpaceType;

} NR_UE_PDCCH_SEARCHSPACE;
#endif
typedef struct {
  /// \brief Pointers to extracted PDCCH symbols in frequency-domain.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **rxdataF_ext;
  /// \brief Pointers to extracted and compensated PDCCH symbols in frequency-domain.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **rxdataF_comp;
  /// \brief Pointers to extracted channel estimates of PDCCH symbols.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **dl_ch_estimates_ext;
  /// \brief Pointers to channel cross-correlation vectors for multi-eNB detection.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **dl_ch_rho_ext;
  /// \brief Pointers to channel cross-correlation vectors for multi-eNB detection.
  /// - first index: rx antenna [0..nb_antennas_rx[
  /// - second index: ? [0..]
  int32_t **rho;
  /// \brief Pointer to llrs, 4-bit resolution.
  /// - first index: ? [0..48*N_RB_DL[
  uint16_t *llr;
  /// \brief Pointer to llrs, 16-bit resolution.
  /// - first index: ? [0..96*N_RB_DL[
  uint16_t *llr16;
  /// \brief \f$\overline{w}\f$ from 36-211.
  /// - first index: ? [0..48*N_RB_DL[
  uint16_t *wbar;
  /// \brief PDCCH/DCI e-sequence (input to rate matching).
  /// - first index: ? [0..96*N_RB_DL[
  int8_t *e_rx;
  /// number of PDCCH symbols in current subframe
  uint8_t num_pdcch_symbols;
  /// Allocated CRNTI for UE
  uint16_t crnti;
  /// 1: the allocated crnti is Temporary C-RNTI / 0: otherwise
  uint8_t crnti_is_temporary;
  /// Total number of PDU errors (diagnostic mode)
  uint32_t dci_errors;
  /// Total number of PDU received
  uint32_t dci_received;
  /// Total number of DCI False detection (diagnostic mode)
  uint32_t dci_false;
  /// Total number of DCI missed (diagnostic mode)
  uint32_t dci_missed;
  /// nCCE for PUCCH per subframe
  uint8_t nCCE[10];
  //Check for specific DCIFormat and AgregationLevel
  uint8_t dciFormat;
  uint8_t agregationLevel;
  #ifdef NR_PDCCH_DEFS_NR_UE
  // CORESET structure, where maximum number of CORESETs to be handled is 3 (according to 38.331 V15.1.0)
  NR_UE_PDCCH_CORESET coreset[NR_NBR_CORESET_ACT_BWP];
  // SEARCHSPACE structure, where maximum number of SEARCHSPACEs to be handled is 10 (according to 38.331 V15.1.0)
  // Each SearchSpace is associated with one ControlResourceSet 
  NR_UE_PDCCH_SEARCHSPACE searchSpace[NR_NBR_SEARCHSPACE_ACT_BWP];

  uint32_t nb_search_space;
  #endif
} NR_UE_PDCCH;

#define PBCH_A 24

typedef struct {
  /// \brief Pointers to extracted PBCH symbols in frequency-domain.
  /// - first index: rx antenna [0..nb_antennas_rx[
  /// - second index: ? [0..287] (hard coded)
  int32_t **rxdataF_ext;
  /// \brief Pointers to extracted and compensated PBCH symbols in frequency-domain.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..287] (hard coded)
  int32_t **rxdataF_comp;
  /// \brief Pointers to downlink channel estimates in frequency-domain extracted in PRBS.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..287] (hard coded)
  int32_t **dl_ch_estimates_ext;
  uint8_t pbch_a[NR_POLAR_PBCH_PAYLOAD_BITS>>3];
  uint8_t pbch_a_interleaved[NR_POLAR_PBCH_PAYLOAD_BITS>>3];
  uint8_t pbch_a_prime[NR_POLAR_PBCH_PAYLOAD_BITS>>3];
  uint8_t pbch_e[NR_POLAR_PBCH_E];
  double  demod_pbch_e[NR_POLAR_PBCH_E];
  /// \brief Pointer to PBCH llrs.
  /// - first index: ? [0..1919] (hard coded)
  int8_t *llr;
  /// \brief Pointer to PBCH decoded output.
  /// - first index: ? [0..63] (hard coded)
  uint8_t *decoded_output;
  /// \brief Total number of PDU errors.
  uint32_t pdu_errors;
  /// \brief Total number of PDU errors 128 frames ago.
  uint32_t pdu_errors_last;
  /// \brief Total number of consecutive PDU errors.
  uint32_t pdu_errors_conseq;
  /// \brief FER (in percent) .
  uint32_t pdu_fer;
} NR_UE_PBCH;

typedef struct {
  int16_t amp;
  int16_t *prachF;
  int16_t *prach;
} NR_UE_PRACH;

/*typedef enum {
  /// do not detect any DCIs in the current subframe
  NO_DCI = 0x0,
  /// detect only downlink DCIs in the current subframe
  UL_DCI = 0x1,
  /// detect only uplink DCIs in the current subframe
  DL_DCI = 0x2,
  /// detect both uplink and downlink DCIs in the current subframe
  UL_DL_DCI = 0x3} nr_dci_detect_mode_t;*/

typedef struct UE_NR_SCAN_INFO_s {
  /// 10 best amplitudes (linear) for each pss signals
  int32_t amp[3][10];
  /// 10 frequency offsets (kHz) corresponding to best amplitudes, with respect do minimum DL frequency in the band
  int32_t freq_offset_Hz[3][10];
} UE_NR_SCAN_INFO_t;

/// Top-level PHY Data Structure for UE
typedef struct {
  /// \brief Module ID indicator for this instance
  uint8_t Mod_id;
  /// \brief Component carrier ID for this PHY instance
  uint8_t CC_id;
  /// \brief Mapping of CC_id antennas to cards
  openair0_rf_map      rf_map;
  //uint8_t local_flag;
  /// \brief Indicator of current run mode of UE (normal_txrx, rx_calib_ue, no_L2_connect, debug_prach)
  runmode_t mode;
  /// \brief Indicator that UE should perform band scanning
  int UE_scan;
  /// \brief Indicator that UE should perform coarse scanning around carrier
  int UE_scan_carrier;
  /// \brief Indicator that UE is synchronized to an eNB
  int is_synchronized;
  /// Data structure for UE process scheduling
  UE_nr_proc_t proc;
  /// Flag to indicate the UE shouldn't do timing correction at all
  int no_timing_correction;
  /// \brief Total gain of the TX chain (16-bit baseband I/Q to antenna)
  uint32_t tx_total_gain_dB;
  /// \brief Total gain of the RX chain (antenna to baseband I/Q) This is a function of rx_gain_mode (and the corresponding gain) and the rx_gain of the card.
  uint32_t rx_total_gain_dB;
  /// \brief Total gains with maximum RF gain stage (ExpressMIMO2/Lime)
  uint32_t rx_gain_max[4];
  /// \brief Total gains with medium RF gain stage (ExpressMIMO2/Lime)
  uint32_t rx_gain_med[4];
  /// \brief Total gains with bypassed RF gain stage (ExpressMIMO2/Lime)
  uint32_t rx_gain_byp[4];
  /// \brief Current transmit power
  int16_t tx_power_dBm[MAX_NR_OF_SLOTS];
  /// \brief Total number of REs in current transmission
  int tx_total_RE[MAX_NR_OF_SLOTS];
  /// \brief Maximum transmit power
  int8_t tx_power_max_dBm;
  /// \brief Number of eNB seen by UE
  uint8_t n_connected_eNB;
  /// \brief indicator that Handover procedure has been initiated
  uint8_t ho_initiated;
  /// \brief indicator that Handover procedure has been triggered
  uint8_t ho_triggered;
  /// \brief Measurement variables.
  PHY_NR_MEASUREMENTS measurements;
  NR_DL_FRAME_PARMS  frame_parms;
  /// \brief Frame parame before ho used to recover if ho fails.
  NR_DL_FRAME_PARMS  frame_parms_before_ho;
  NR_UE_COMMON    common_vars;

  nr_ue_if_module_t *if_inst;
  nr_downlink_indication_t dl_indication;
  nr_uplink_indication_t ul_indication;
  fapi_nr_rx_indication_t rx_ind;
  fapi_nr_dci_indication_t dci_ind;

  // point to the current rxTx thread index
  uint8_t current_thread_id[10];

  NR_UE_PDSCH     *pdsch_vars[RX_NB_TH_MAX][NUMBER_OF_CONNECTED_eNB_MAX+1]; // two RxTx Threads
  NR_UE_PDSCH_FLP *pdsch_vars_flp[NUMBER_OF_CONNECTED_eNB_MAX+1];
  NR_UE_PDSCH     *pdsch_vars_SI[NUMBER_OF_CONNECTED_eNB_MAX+1];
  NR_UE_PDSCH     *pdsch_vars_ra[NUMBER_OF_CONNECTED_eNB_MAX+1];
  NR_UE_PDSCH     *pdsch_vars_p[NUMBER_OF_CONNECTED_eNB_MAX+1];
  NR_UE_PDSCH     *pdsch_vars_MCH[NUMBER_OF_CONNECTED_eNB_MAX];
  NR_UE_PBCH      *pbch_vars[NUMBER_OF_CONNECTED_eNB_MAX];
  NR_UE_PDCCH     *pdcch_vars[RX_NB_TH_MAX][NUMBER_OF_CONNECTED_eNB_MAX];
  NR_UE_PRACH     *prach_vars[NUMBER_OF_CONNECTED_eNB_MAX];
  NR_UE_DLSCH_t   *dlsch[RX_NB_TH_MAX][NUMBER_OF_CONNECTED_eNB_MAX][2]; // two RxTx Threads
  NR_UE_ULSCH_t   *ulsch[NUMBER_OF_CONNECTED_eNB_MAX];
  NR_UE_DLSCH_t   *dlsch_SI[NUMBER_OF_CONNECTED_eNB_MAX];
  NR_UE_DLSCH_t   *dlsch_ra[NUMBER_OF_CONNECTED_eNB_MAX];
  NR_UE_DLSCH_t   *dlsch_p[NUMBER_OF_CONNECTED_eNB_MAX];
  NR_UE_DLSCH_t   *dlsch_MCH[NUMBER_OF_CONNECTED_eNB_MAX];
  // This is for SIC in the UE, to store the reencoded data
  LTE_eNB_DLSCH_t  *dlsch_eNB[NUMBER_OF_CONNECTED_eNB_MAX];

  //Paging parameters
  uint32_t              IMSImod1024;
  uint32_t              PF;
  uint32_t              PO;

  // For abstraction-purposes only
  uint8_t               sr[10];
  uint8_t               pucch_sel[10];
  uint8_t               pucch_payload[22];

  UE_MODE_t        UE_mode[NUMBER_OF_CONNECTED_eNB_MAX];
  /// cell-specific reference symbols
  uint32_t lte_gold_table[7][20][2][14];

#if defined(UPGRADE_RAT_NR)

  /// demodulation reference signal for NR PBCH
  uint32_t dmrs_pbch_bitmap_nr[DMRS_PBCH_I_SSB][DMRS_PBCH_N_HF][DMRS_BITMAP_SIZE];

#endif

  /// PBCH DMRS sequence
  uint32_t nr_gold_pbch[2][64][NR_PBCH_DMRS_LENGTH_DWORD];

  /// PDSCH DMRS
  uint32_t nr_gold_pdsch[2][20][2][21];

  /// PDCCH DMRS
  uint32_t nr_gold_pdcch[7][20][3][10];

  uint32_t X_u[64][839];

  uint32_t high_speed_flag;
  uint32_t perfect_ce;
  int16_t ch_est_alpha;
  int generate_ul_signal[NUMBER_OF_CONNECTED_eNB_MAX];

  UE_NR_SCAN_INFO_t scan_info[NB_BANDS_MAX];

  char ulsch_no_allocation_counter[NUMBER_OF_CONNECTED_eNB_MAX];



  unsigned char ulsch_Msg3_active[NUMBER_OF_CONNECTED_eNB_MAX];
  uint32_t  ulsch_Msg3_frame[NUMBER_OF_CONNECTED_eNB_MAX];
  unsigned char ulsch_Msg3_subframe[NUMBER_OF_CONNECTED_eNB_MAX];
  PRACH_RESOURCES_t *prach_resources[NUMBER_OF_CONNECTED_eNB_MAX];
  int turbo_iterations, turbo_cntl_iterations;
  /// \brief ?.
  /// - first index: eNB [0..NUMBER_OF_CONNECTED_eNB_MAX[ (hard coded)
  uint32_t total_TBS[NUMBER_OF_CONNECTED_eNB_MAX];
  /// \brief ?.
  /// - first index: eNB [0..NUMBER_OF_CONNECTED_eNB_MAX[ (hard coded)
  uint32_t total_TBS_last[NUMBER_OF_CONNECTED_eNB_MAX];
  /// \brief ?.
  /// - first index: eNB [0..NUMBER_OF_CONNECTED_eNB_MAX[ (hard coded)
  uint32_t bitrate[NUMBER_OF_CONNECTED_eNB_MAX];
  /// \brief ?.
  /// - first index: eNB [0..NUMBER_OF_CONNECTED_eNB_MAX[ (hard coded)
  uint32_t total_received_bits[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_errors[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_errors_last[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_received[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_received_last[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_fer[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_SI_received[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_SI_errors[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_ra_received[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_ra_errors[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_p_received[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_p_errors[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_mch_received_sf[MAX_MBSFN_AREA][NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_mch_received[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_mcch_received[MAX_MBSFN_AREA][NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_mtch_received[MAX_MBSFN_AREA][NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_mcch_errors[MAX_MBSFN_AREA][NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_mtch_errors[MAX_MBSFN_AREA][NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_mcch_trials[MAX_MBSFN_AREA][NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_mtch_trials[MAX_MBSFN_AREA][NUMBER_OF_CONNECTED_eNB_MAX];
  int current_dlsch_cqi[NUMBER_OF_CONNECTED_eNB_MAX];
  unsigned char first_run_timing_advance[NUMBER_OF_CONNECTED_eNB_MAX];
  uint8_t               generate_prach;
  uint8_t               prach_cnt;
  uint8_t               prach_PreambleIndex;
  //  uint8_t               prach_timer;
  uint8_t               decode_SIB;
  uint8_t               decode_MIB;
  int              rx_offset; /// Timing offset
  int              rx_offset_diff; /// Timing adjustment for ofdm symbol0 on HW USRP
  int              time_sync_cell;
  int              timing_advance; ///timing advance signalled from eNB
  int              hw_timing_advance;
  int              N_TA_offset; ///timing offset used in TDD
  /// Flag to tell if UE is secondary user (cognitive mode)
  unsigned char    is_secondary_ue;
  /// Flag to tell if secondary eNB has channel estimates to create NULL-beams from.
  unsigned char    has_valid_precoder;
  /// hold the precoder for NULL beam to the primary eNB
  int              **ul_precoder_S_UE;
  /// holds the maximum channel/precoder coefficient
  char             log2_maxp;

  /// if ==0 enables phy only test mode
  int mac_enabled;

  /// Flag to initialize averaging of PHY measurements
  int init_averaging;

  /// \brief sinr for all subcarriers of the current link (used only for abstraction).
  /// - first index: ? [0..12*N_RB_DL[
  double *sinr_dB;

  /// \brief sinr for all subcarriers of first symbol for the CQI Calculation.
  /// - first index: ? [0..12*N_RB_DL[
  double *sinr_CQI_dB;

  /// sinr_effective used for CQI calulcation
  double sinr_eff;

  /// N0 (used for abstraction)
  double N0;

  /// PDSCH Varaibles
  PDSCH_CONFIG_DEDICATED pdsch_config_dedicated[NUMBER_OF_CONNECTED_eNB_MAX];

  /// PUSCH Varaibles
  PUSCH_CONFIG_DEDICATED pusch_config_dedicated[NUMBER_OF_CONNECTED_eNB_MAX];

  /// PUSCH contention-based access vars
  PUSCH_CA_CONFIG_DEDICATED  pusch_ca_config_dedicated[NUMBER_OF_eNB_MAX]; // lola

  /// PUCCH variables

  PUCCH_CONFIG_DEDICATED pucch_config_dedicated[NUMBER_OF_CONNECTED_eNB_MAX];

//#if defined(UPGRADE_RAT_NR)
#if 1

  SystemInformationBlockType1_nr_t systemInformationBlockType1_nr;

  CellGroupConfig_t          cell_group_config;
  PDSCH_ServingCellConfig_t  PDSCH_ServingCellConfig;
  PDSCH_Config_t             PDSCH_Config;

  PUCCH_ConfigCommon_nr_t    pucch_config_common_nr[NUMBER_OF_CONNECTED_eNB_MAX];
  PUCCH_Config_t             pucch_config_dedicated_nr[NUMBER_OF_CONNECTED_eNB_MAX];

  PUSCH_Config_t             pusch_config;

#endif

  uint8_t ncs_cell[20][7];

  /// UL-POWER-Control
  UL_POWER_CONTROL_DEDICATED ul_power_control_dedicated[NUMBER_OF_CONNECTED_eNB_MAX];

  /// TPC
  TPC_PDCCH_CONFIG tpc_pdcch_config_pucch[NUMBER_OF_CONNECTED_eNB_MAX];
  TPC_PDCCH_CONFIG tpc_pdcch_config_pusch[NUMBER_OF_CONNECTED_eNB_MAX];

  /// CQI reporting
  CQI_REPORT_CONFIG cqi_report_config[NUMBER_OF_CONNECTED_eNB_MAX];

  /// SRS Variables
  SOUNDINGRS_UL_CONFIG_DEDICATED soundingrs_ul_config_dedicated[NUMBER_OF_CONNECTED_eNB_MAX];

  /// Scheduling Request Config
  SCHEDULING_REQUEST_CONFIG scheduling_request_config[NUMBER_OF_CONNECTED_eNB_MAX];

//#if defined(UPGRADE_RAT_NR)
#if 1
  scheduling_request_config_t scheduling_request_config_nr[NUMBER_OF_CONNECTED_eNB_MAX];

#endif

  /// Transmission mode per eNB
  uint8_t transmission_mode[NUMBER_OF_CONNECTED_eNB_MAX];

  time_stats_t phy_proc[RX_NB_TH];
  time_stats_t phy_proc_tx;
  time_stats_t phy_proc_rx[RX_NB_TH];

  uint32_t use_ia_receiver;

  time_stats_t ofdm_mod_stats;
  time_stats_t ulsch_encoding_stats;
  time_stats_t ulsch_modulation_stats;
  time_stats_t ulsch_segmentation_stats;
  time_stats_t ulsch_rate_matching_stats;
  time_stats_t ulsch_turbo_encoding_stats;
  time_stats_t ulsch_interleaving_stats;
  time_stats_t ulsch_multiplexing_stats;

  time_stats_t generic_stat;
  time_stats_t generic_stat_bis[RX_NB_TH][LTE_SLOTS_PER_SUBFRAME];
  time_stats_t ue_front_end_stat[RX_NB_TH];
  time_stats_t ue_front_end_per_slot_stat[RX_NB_TH][LTE_SLOTS_PER_SUBFRAME];
  time_stats_t pdcch_procedures_stat[RX_NB_TH];
  time_stats_t pdsch_procedures_stat[RX_NB_TH];
  time_stats_t pdsch_procedures_per_slot_stat[RX_NB_TH][LTE_SLOTS_PER_SUBFRAME];
  time_stats_t dlsch_procedures_stat[RX_NB_TH];

  time_stats_t ofdm_demod_stats;
  time_stats_t dlsch_rx_pdcch_stats;
  time_stats_t rx_dft_stats;
  time_stats_t dlsch_channel_estimation_stats;
  time_stats_t dlsch_freq_offset_estimation_stats;
  time_stats_t dlsch_decoding_stats[2];
  time_stats_t dlsch_demodulation_stats;
  time_stats_t dlsch_rate_unmatching_stats;
  time_stats_t dlsch_turbo_decoding_stats;
  time_stats_t dlsch_deinterleaving_stats;
  time_stats_t dlsch_llr_stats;
  time_stats_t dlsch_llr_stats_parallelization[RX_NB_TH][LTE_SLOTS_PER_SUBFRAME];
  time_stats_t dlsch_unscrambling_stats;
  time_stats_t dlsch_rate_matching_stats;
  time_stats_t dlsch_turbo_encoding_stats;
  time_stats_t dlsch_interleaving_stats;
  time_stats_t dlsch_tc_init_stats;
  time_stats_t dlsch_tc_alpha_stats;
  time_stats_t dlsch_tc_beta_stats;
  time_stats_t dlsch_tc_gamma_stats;
  time_stats_t dlsch_tc_ext_stats;
  time_stats_t dlsch_tc_intl1_stats;
  time_stats_t dlsch_tc_intl2_stats;
  time_stats_t tx_prach;

  /// RF and Interface devices per CC
  openair0_device rfdevice;
  time_stats_t dlsch_encoding_SIC_stats;
  time_stats_t dlsch_scrambling_SIC_stats;
  time_stats_t dlsch_modulation_SIC_stats;
  time_stats_t dlsch_llr_stripping_unit_SIC_stats;
  time_stats_t dlsch_unscrambling_SIC_stats;

#if ENABLE_RAL
  hash_table_t    *ral_thresholds_timed;
  SLIST_HEAD(ral_thresholds_gen_poll_s, ral_threshold_phy_t) ral_thresholds_gen_polled[RAL_LINK_PARAM_GEN_MAX];
  SLIST_HEAD(ral_thresholds_lte_poll_s, ral_threshold_phy_t) ral_thresholds_lte_polled[RAL_LINK_PARAM_LTE_MAX];
#endif

} PHY_VARS_NR_UE;

/* this structure is used to pass both UE phy vars and
 * proc to the function UE_thread_rxn_txnp4
 */
struct nr_rxtx_thread_data {
  PHY_VARS_NR_UE    *UE;
  UE_nr_rxtx_proc_t *proc;
};

void exit_fun(const char* s);

/*static inline int wait_on_condition(pthread_mutex_t *mutex,pthread_cond_t *cond,int *instance_cnt,char *name) {

  if (pthread_mutex_lock(mutex) != 0) {
    LOG_E( PHY, "[SCHED][eNB] error locking mutex for %s\n",name);
    exit_fun("nothing to add");
    return(-1);
  }

  while (*instance_cnt < 0) {
    // most of the time the thread is waiting here
    // proc->instance_cnt_rxtx is -1
    pthread_cond_wait(cond,mutex); // this unlocks mutex_rxtx while waiting and then locks it again
  }

  if (pthread_mutex_unlock(mutex) != 0) {
    LOG_E(PHY,"[SCHED][eNB] error unlocking mutex for %s\n",name);
    exit_fun("nothing to add");
    return(-1);
  }
  return(0);
}

static inline int wait_on_busy_condition(pthread_mutex_t *mutex,pthread_cond_t *cond,int *instance_cnt,char *name) {

  if (pthread_mutex_lock(mutex) != 0) {
    LOG_E( PHY, "[SCHED][eNB] error locking mutex for %s\n",name);
    exit_fun("nothing to add");
    return(-1);
  }

  while (*instance_cnt == 0) {
    // most of the time the thread will skip this
    // waits only if proc->instance_cnt_rxtx is 0
    pthread_cond_wait(cond,mutex); // this unlocks mutex_rxtx while waiting and then locks it again
  }

  if (pthread_mutex_unlock(mutex) != 0) {
    LOG_E(PHY,"[SCHED][eNB] error unlocking mutex for %s\n",name);
    exit_fun("nothing to add");
    return(-1);
  }
  return(0);
}

static inline int release_thread(pthread_mutex_t *mutex,int *instance_cnt,char *name) {

  if (pthread_mutex_lock(mutex) != 0) {
    LOG_E( PHY, "[SCHED][eNB] error locking mutex for %s\n",name);
    exit_fun("nothing to add");
    return(-1);
  }

  *instance_cnt=*instance_cnt-1;

  if (pthread_mutex_unlock(mutex) != 0) {
    LOG_E( PHY, "[SCHED][eNB] error unlocking mutex for %s\n",name);
    exit_fun("nothing to add");
    return(-1);
  }
  return(0);
}
*/


/*
#include "PHY/INIT/defs.h"
#include "PHY/LTE_REFSIG/defs.h"
#include "PHY/MODULATION/defs.h"
#include "PHY/LTE_TRANSPORT/proto.h"
#include "PHY/LTE_ESTIMATION/defs.h"
*/
#include "SIMULATION/ETH_TRANSPORT/defs.h"
#endif