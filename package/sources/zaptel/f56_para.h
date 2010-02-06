/*##########################################################################
  #                         Infineon Technologies                          #
  #                          Communication ICs                             #
  #                         Device Driver Software                         #
  #                  Copyright (c) Infineon Technologies,  2002            #
  #========================================================================#
  #   Project       FALC56 Driver Software                                 #
  #   Module        FALC56                                                 #
  #------------------------------------------------------------------------#
  #   Description   header for the device driver (parameter description)   #
  #------------------------------------------------------------------------#
  #   $Workfile::   f56_para.h                                            $#
  #   $Revision::   1.0                                                   $#
  #       $Date::   May 21 2003 09:18:30                                  $#
  *------------------------------------------------------------------------*
 .* Any user of this software should understand that Infineon will not be  *
  * responsible for any consequences resulting from the use of this        *
  * software.                                                              *
  ##########################################################################*/

#ifndef __F56_PARA_H
#define __F56_PARA_H

/***** parameter for commands ****/

/* Layer 1 state definitions */
#define  LINE_INTERFACE_DEACT_SEC    5
#define  LINE_INTERFACE_ACT_SEC      6
#define  LINE_INTERFACE_ACT_AWATING  3
#define  LINE_INTERFACE_PDEN_LOS     4
#define  LINE_INTERFACE_DEACT        1
#define  LINE_INTERFACE_ACT          2


/* line interface modes */
#define PCM_30_MODE              0x00
#define PCM_24_MODE              0x01


/* clocking modes */
#define SLAVE_MODE               0x00
#define MASTER_MODE              0x01

/* type of loops */
#define LOCAL_LOOP               0x00
#define PAYLOAD_LOOP             0x01
#define REMOTE_LOOP              0x02
#define CHANNEL_LOOP             0x03

/* HDLC/SS7 defines */
#define HDLC_FDL_LINE            0x00
#define HDLC_SA_LINE             0x00
#define HDLC_TS_LINE             0x01
#define HDLC_FDL_SYSTEM          0x03
#define HDLC_SA_SYSTEM           0x04
#define HDLC_TS_SYSTEM           0x05

#define HDLC_MODE_SS7            0x01
#define HDLC_MODE_1BYTE_LOW      0x02
#define HDLC_MODE_2BYTE          0x03
#define HDLC_MODE_NO_ADDRESS     0x04
#define HDLC_MODE_1BYTE_HIGH     0x05
#define HDLC_MODE_NO_HDLC        0x07

#define REMOTE_ALARM_MODE1       0x00
#define REMOTE_ALARM_MODE2       0x01
#define EVEN_ODD                 0x00
#define ODD                      0x01
#define EVEN                     0x02

#define NO_ADDRESS_COMPARISON    0x00
#define BYTE1_AC_RAL12           0x01
#define BYTE1_AC_RAH12           0x02
#define BYTE2_AC_RA12            0x03
#define NO_HDLC_FRAMING_MODE     0x04

#define CFS_1_DISABLE            0x00
#define CFS_1_ENABLE             0x01
#define CFS_2_DISABLE            0x02
#define CFS_2_ENABLE             0x03

#define SUET_32                  0x00
#define SUET_64                  0x01

#define AFX_DISABLE              0x00
#define AFX_ENABLE               0x01

#define NORMAL                   0x00
#define INVERSE                  0x01

/* PRBS definitions */
#define PRBS_MONITOR             0x00
#define PRBS_GENERATOR           0x01
#define PRBS_FRAMED              0x00
#define PRBS_UNFRAMED            0x01
#define PRBS_NO                  0x02
#define PRBS_AUTO                0x02
#define PRBS_15                  0x00
#define PRBS_20                  0x01
#define PRBS_NORMAL              0x00
#define PRBS_INVERTED            0x01

/* inband loop definitions */
#define IBLOOP_MONITOR           0x00
#define IBLOOP_GENERATOR         0x01
#define IB_LOOP_UP               0x00
#define IB_LOOP_DOWN             0x01
#define IB_LOOP_UP_ACT           0x00
#define IB_LOOP_DOWN_ACT         0x01
#define IB_LOOP_UP_DEACT         0x02
#define IB_LOOP_DOWN_DEACT       0x03
#define LLB_DEACT_5BIT           0x00
#define LLB_DEACT_6BIT           0x01
#define LLB_DEACT_7BIT           0x02
#define LLB_DEACT_8BIT           0x03
#define LLB_ACT_5BIT             0x00
#define LLB_ACT_6BIT             0x01
#define LLB_ACT_7BIT             0x02
#define LLB_ACT_8BIT             0x03

/*line interface modes */
#define MODE_E1                  0x00
#define MODE_T1                  0x01
#define MODE_J1                  0x02
#define MODE_E1_75_OHM           0x03

/* line coding modes */
#define LINE_CODING_NRZ          0x00
#define LINE_CODING_CMI          0x01
#define LINE_CODING_AMI          0x02
#define LINE_CODING_B8ZS         0x03
#define LINE_CODING_HDB3         0x04
#define LINE_CODING_SYNC         0x05

/* line build out (LBO) */
#define LBO_0DB                  0x00
#define LBO_7_5DB                0x01
#define LBO_15DB                 0x02
#define LBO_22_5DB               0x03

/* line interface status */
#define LINE_INTERFACE_LOS       0x00
#define LINE_INTERFACE_AIS       0x01

/* framing modes */
/* E1+T1/J1 */
#define FRAMING_NO               0x08
/* E1 */
#define FRAMING_DOUBLEFRAME      0x00
#define  FRAMING_MULTIFRAME_TX   0x01   
#define  FRAMING_MULTIFRAME_RX   0x02   
#define FRAMING_MULTIFRAME_MOD   0x03
/* T1/J1 */
#define FRAMING_F12              0x04
#define FRAMING_F4               0x05            
#define FRAMING_ESF              0x06
#define FRAMING_F72              0x07

/* framer status */
#define FRAMER_LFA               0x00
#define FRAMER_LMFA              0x01
#define FRAMER_RA                0x02
#define FRAMER_RAR               0x03
#define FRAMER_FAR               0x04
#define FRAMER_AIS_ACT           0x05
#define FRAMER_AIS_DEACT         0x06
#define FRAMER_RRA               0x07
#define FRAMER_MFAR              0x08
#define FRAMER_T400MS            0x09

/* framer transmit path defines */
#define FRAMER_TX_RA             0x00
#define FRAMER_TX_AIS            0x01

/* system data rate, clock rate, channel translation mode */
#define CR_E1_2048               0x00
#define CR_E1_4096               0x01
#define CR_E1_8192               0x02
#define CR_E1_16384              0x03
#define CR_T1_1544               0x04
#define CR_T1_3088               0x05
#define CR_T1_6176               0x06
#define CR_T1_12352              0x07

#define DR_E1_2048               0x00
#define DR_E1_4096               0x01
#define DR_E1_8192               0x02
#define DR_E1_16384              0x03
#define DR_T1_1544               0x04
#define DR_T1_3088               0x05
#define DR_T1_6176               0x06
#define DR_T1_12352              0x07

#define CTM_0                     0x00
#define CTM_1                     0x01

/* clocking modes */
#define RCLK_INPUT                  0x00
#define RCLK_OUTPUT                 0x01
#define SCLKR_INPUT                 0x00
#define SCLKR_OUTPUT                0x02
#define SCLKX_INPUT                 0x00
#define SCLKX_OUTPUT                0x04

#define SCLKX_EXT                   0x00
#define CLK_INT_IN                  0x01
#define SYPXQ_EXT                   0x00
#define SYNC_TX_INT                 0x02
#define SCLKR_EXT                   0x00
#define CLK_INT_OUT                 0x04
#define SYPRQ_EXT                   0x00
#define SYNC_RX_INT                 0x08

#define CLK_FALLING_OUT             0x00
#define CLK_RISING_OUT              0x01
#define CLK_FALLING_IN              0x00
#define CLK_RISING_IN               0x02

#define RX_FRAME_SYNC               0x00
#define RX_SYNC_CLEAR               0x01
#define TX_FRAME_SYNC               0x00
#define TX_FRAME_CLEAR              0x02

#define TCLK_IGNORE                 0x00
#define TCLK_DEJITTERED             0x01
#define TCLK_DIRECT                 0x02

#define TCLK_2048                   0x00
#define TCLK_8192                   0x01
   
/*buffer size */
#define RX_BUFFER_2_FRAME           0x00
#define RX_BUFFER_1_FRAME           0x01 
#define RX_BUFFER_96_bits           0x02
#define RX_BUFFER_BYPASS            0x03
#define TX_BUFFER_2_FRAME           0x02
#define TX_BUFFER_1_FRAME           0x01 
#define TX_BUFFER_96_bits           0x03
#define TX_BUFFER_BYPASS            0x00

/*BOM receiver */
#define BOM_10_BYTE_PACKETS         0x00
#define BOM_CONTINUOUS_RECEPTION    0x01
#define MODE_OFF                    0x00
#define MODE_TS                     0x01
#define MODE_DL                     0x02

/* error counter defines */
#define ERROR_COUNTER_FEC           0x00
#define ERROR_COUNTER_CVC           0x01
#define ERROR_COUNTER_CEC           0x02
#define ERROR_COUNTER_CEC2_EBC      0x03
#define ERROR_COUNTER_CEC3_BEC      0x04
#define ERROR_COUNTER_EBC_COEC      0x05

/* reset type */
#define CTRL_TX                     0x00
#define CTRL_RX                     0x01
#define CTRL_SIG                    0x02

/* SSM defines */
#define SSM_SA4                     0x04
#define SSM_SA5                     0x05
#define SSM_SA6                     0x06
#define SSM_SA7                     0x07
#define SSM_SA8                     0x08


/* Receive Multi Function Ports 
---------------------- E1 & T1 ---------------------*/
#define MFP_SYPRQ          0x00
#define MFP_RFM            0x01
#define MFP_RMFB           0x02
#define MFP_RSIGM          0x03
#define MFP_RSIG           0x04
#define MFP_DLR            0x05
#define MFP_FREEZ          0x06
#define MFP_RFSPQ          0x07
#define MFP_LOS            0x08

/* Transmit Multi Function Ports 
---------------------- E1 & T1 ---------------------*/
#define MFP_SYPXQ          0x00
#define MFP_XMFS           0x01
#define MFP_XSIG           0x02
#define MFP_TCLK           0x03
#define MFP_XMFB           0x04
#define MFP_XSIGM          0x05
#define MFP_DLX            0x06
#define MFP_XCLK           0x07
#define MFP_XLT            0x08

/* Transmit & Receive  Multi Function Ports 
---------------------- E1 & T1 ---------------------*/
#define MFP_MP_ACCESS      0x09
#define MFP_STATIC_1       0x0A
#define MFP_STATIC_0       0x0B

/* status Multi Function Port Input 1
----------------------------------------------------*/
#define SYSTEM_INTERFACE_RMFP_A   0x00
#define SYSTEM_INTERFACE_RMFP_B   0x01
#define SYSTEM_INTERFACE_RMFP_C   0x02
#define SYSTEM_INTERFACE_RMFP_D   0x03
#define SYSTEM_INTERFACE_XMFP_A   0x04
#define SYSTEM_INTERFACE_XMFP_B   0x05
#define SYSTEM_INTERFACE_XMFP_C   0x06
#define SYSTEM_INTERFACE_XMFP_D   0x07

/* Termination
----------------------------------------------------*/
#define TERMINATION_ANALOG_SWITCH       0x00
#define TERMINATION_INTERNAL_RESISTOR   0x01

/* Tristate
----------------------------------------------------*/
#define TRISTATE_ALL               0x00
#define TRISTATE_TRANSMIT_LINE     0x01
#define TRISTATE_FSC               0x02
#define TRISTATE_RDO_RSIG          0x03
#define TRISTATE_RCLK              0x04
/*-----end of parameters------------------------------*/




/* define parameters for callback----------------------------*/

#define HDLC_TX_DATA_XDU        0
#define HDLC_RX_FRAME_TOO_LONG  1
#define HDLC_RX_FRAME_IN_ERROR  2

#define BOM_ZERO                0
#define BOM_CONFIG_ERROR        1 
#define BOM_DATA_INVALIDATED    2

#define SS7_DATA_FISU           1
#define SS7_DATA_MSU            2
#define SS7_DATA_LSSU           3

#define SS7_DATA_SU_IN_ERROR    0
#define SS7_DATA_MSG_TOO_LONG   1

#define E1_ALARM_CB_TS          128

#define SYSTEM_INTERFACE_RSP    0
#define SYSTEM_INTERFACE_RSN    1
#define SYSTEM_INTERFACE_XSP    2
#define SYSTEM_INTERFACE_XSN    3

#define SSM_E1_QUALITY_UNKNOWN  0
#define SSM_E1_REC_G_811        2
#define SSM_E1_SSU_A            4
#define SSM_E1_SSU_B            8
#define SSM_E1_SETS             11
#define SSM_E1_DO_NOT_USE       15

#define SSM_T1_G_811            0x04
#define SSM_T1_G_812_TYPE_II    0x0C
#define SSM_T1_G_812_TYPE_III   0x7C
#define SSM_T1_G_812_TYPE_IV    0x10
#define SSM_T1_STRATUM_4        0x28
#define SSM_T1_G_813_OPTION_3   0x22
#define SSM_T1_G_812_TYPE_V     0x78
#define SSM_T1_QUALITY_UNKNOWN  0x08
#define SSM_T1_DO_NOT_USE       0x30
#define SSM_T1_PROVISIONABLE    0x40

/* end of parameters for callbacks ----------------------------*/


#endif  /*** __F56_PARA_H ***/

