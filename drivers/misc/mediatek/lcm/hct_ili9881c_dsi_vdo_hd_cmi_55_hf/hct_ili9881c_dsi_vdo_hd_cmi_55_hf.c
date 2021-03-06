/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/


#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif

#include "lcm_drv.h"




// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH                                         (720)
#define FRAME_HEIGHT                                        (1280)
#define LCM_ID                       (0x9881)

#define REGFLAG_DELAY               (0XFE)
#define REGFLAG_END_OF_TABLE        (0x100) // END OF REGISTERS MARKER


#define LCM_DSI_CMD_MODE                                    0



// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v)                                    (lcm_util.set_reset_pin((v)))

#define UDELAY(n)                                           (lcm_util.udelay(n))
#define MDELAY(n)                                           (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V3(para_tbl,size,force_update)        lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)    lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)       lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)                                      lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)                  lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)               lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#define   LCM_DSI_CMD_MODE							0
 struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {
    
    /*
    Note :

    Data ID will depends on the following rule.
    
        count of parameters > 1 => Data ID = 0x39
        count of parameters = 1 => Data ID = 0x15
        count of parameters = 0 => Data ID = 0x05

    Structure Format :

    {DCS command, count of parameters, {parameter list}}
    {REGFLAG_DELAY, milliseconds of time, {)},

    ...

    Setting ending by predefined flag
    
    {REGFLAG_END_OF_TABLE, 0x00, {}}
    */
/*
{0xFF,3,(0x98,0x81,0x03)},
{0x01,1,(0x08)},
{0x02,1,(0x00)},
{0x03,1,(0x73)},
{0x04,1,(0x73)},
{0x05,1,(0x14)},
{0x06,1,(0x06)}, 
{0x07,1,(0x02)}, 
{0x08,1,(0x05)}, 
{0x09,1,(0x00)}, 
{0x0a,1,(0x0c)}, 
{0x0b,1,(0x00)},
{0x0c,1,(0x1c)},
{0x0d,1,(0x1c)},
{0x0e,1,(0x00)},
{0x0f,1,(0x0c)},
{0x10,1,(0x0c)},
{0x11,1,(0x01)},
{0x12,1,(0x01)},
{0x13,1,(0x1b)},
{0x14,1,(0x0b)},
{0x15,1,(0x00)},
{0x16,1,(0x00)},
{0x17,1,(0x00)},
{0x18,1,(0x00)},
{0x19,1,(0x00)},
{0x1a,1,(0x00)},
{0x1b,1,(0x00)},
{0x1c,1,(0x00)},
{0x1d,1,(0x00)},
{0x1e,1,(0xC8)},
{0x1f,1,(0x80)}, 
{0x20,1,(0x02)}, 
{0x21,1,(0x00)}, 
{0x22,1,(0x02)}, 
{0x23,1,(0x00)}, 
{0x24,1,(0x00)}, 
{0x25,1,(0x00)}, 
{0x26,1,(0x00)}, 
{0x27,1,(0x00)}, 
{0x28,1,(0xFB)}, 
{0x29,1,(0x43)}, 
{0x2a,1,(0x00)}, 
{0x2b,1,(0x00)}, 
{0x2c,1,(0x07)}, 
{0x2d,1,(0x07)}, 
{0x2e,1,(0xFF)}, 
{0x2f,1,(0xFF)}, 
{0x30,1,(0x11)}, 
{0x31,1,(0x00)}, 
{0x32,1,(0x00)}, 
{0x33,1,(0x00)}, 
{0x34,1,(0x84)}, 
{0x35,1,(0x80)}, 
{0x36,1,(0x07)}, 
{0x37,1,(0x00)}, 
{0x38,1,(0x00)}, 
{0x39,1,(0x00)}, 
{0x3a,1,(0x00)}, 
{0x3b,1,(0x00)}, 
{0x3c,1,(0x00)}, 
{0x3d,1,(0x00)}, 
{0x3e,1,(0x00)}, 
{0x3f,1,(0x00)}, 
{0x40,1,(0x00)}, 
{0x41,1,(0x00)}, 
{0x42,1,(0x00)}, 
{0x43,1,(0x80)}, 
{0x44,1,(0x08)},                                     
{0x50,1,(0x01)}, 
{0x51,1,(0x23)}, 
{0x52,1,(0x45)}, 
{0x53,1,(0x67)}, 
{0x54,1,(0x89)}, 
{0x55,1,(0xAB)}, 
{0x56,1,(0x01)}, 
{0x57,1,(0x23)}, 
{0x58,1,(0x45)}, 
{0x59,1,(0x67)}, 
{0x5a,1,(0x89)}, 
{0x5b,1,(0xAB)}, 
{0x5c,1,(0xCD)}, 
{0x5d,1,(0xEF)},                                       
{0x5e,1,(0x10)}, 
{0x5f,1,(0x02)}, 
{0x60,1,(0x02)}, 
{0x61,1,(0x02)}, 
{0x62,1,(0x02)}, 
{0x63,1,(0x02)}, 
{0x64,1,(0x02)}, 
{0x65,1,(0x02)}, 
{0x66,1,(0x08)}, 
{0x67,1,(0x09)}, 
{0x68,1,(0x02)}, 
{0x69,1,(0x0c)}, 
{0x6a,1,(0x0e)}, 
{0x6b,1,(0x0d)}, 
{0x6c,1,(0x0f)}, 
{0x6d,1,(0x10)}, 
{0x6e,1,(0x12)}, 
{0x6f,1,(0x11)}, 
{0x70,1,(0x13)}, 
{0x71,1,(0x06)}, 
{0x72,1,(0x07)}, 
{0x73,1,(0x02)}, 
{0x74,1,(0x02)}, 
{0x75,1,(0x02)}, 
{0x76,1,(0x02)}, 
{0x77,1,(0x02)}, 
{0x78,1,(0x02)}, 
{0x79,1,(0x02)}, 
{0x7a,1,(0x02)}, 
{0x7b,1,(0x02)}, 
{0x7c,1,(0x07)}, 
{0x7d,1,(0x06)}, 
{0x7e,1,(0x02)}, 
{0x7f,1,(0x0f)}, 
{0x80,1,(0x0d)}, 
{0x81,1,(0x0e)}, 
{0x82,1,(0x0c)},
{0x83,1,(0x11)},
{0x84,1,(0x13)},
{0x85,1,(0x10)},
{0x86,1,(0x12)},
{0x87,1,(0x09)},
{0x88,1,(0x08)},
{0x89,1,(0x02)},
{0x8A,1,(0x02)},
{0xFF,3,(0x98,0x81,0x04)},
{0x6C,1,(0x15)},        
{0x6E,1,(0x2B)},
{0x6F,1,(0x35)},
{0x3A,1,(0x24)},
{0x8D,1,(0x1F)},
{0x87,1,(0xBA)},
{0x26,1,(0x76)},
{0xB2,1,(0xD1)},
{0xFF,3,(0x98,0x81,0x01)},
{0x22,1,(0x3a)},
{0x31,1,(0x0B)},
{0x53,1,(0x4C)},
{0x55,1,(0x8F)},
{0x50,1,(0x96)},
{0x51,1,(0x96)},
{0x60,1,(0x14)},                                      
{0xA0,1,(0x08)},
{0xA1,1,(0x12)},
{0xA2,1,(0x1B)},
{0xA3,1,(0x10)},
{0xA4,1,(0x11)},
{0xA5,1,(0x21)},
{0xA6,1,(0x14)},
{0xA7,1,(0x19)},
{0xA8,1,(0x63)},
{0xA9,1,(0x1C)},
{0xAA,1,(0x29)},
{0xAB,1,(0x61)},
{0xAC,1,(0x17)},
{0xAD,1,(0x10)},
{0xAE,1,(0x3D)},
{0xAF,1,(0x08)},
{0xB0,1,(0x0A)},
{0xB1,1,(0x57)},
{0xB2,1,(0x67)},
{0xB3,1,(0x39)},                                      
{0xC0,1,(0x08)}, 
{0xC1,1,(0x12)}, 
{0xC2,1,(0x1B)}, 
{0xC3,1,(0x10)}, 
{0xC4,1,(0x10)}, 
{0xC5,1,(0x20)}, 
{0xC6,1,(0x15)}, 
{0xC7,1,(0x19)}, 
{0xC8,1,(0x64)}, 
{0xC9,1,(0x1C)}, 
{0xCA,1,(0x29)}, 
{0xCB,1,(0x60)}, 
{0xCC,1,(0x17)}, 
{0xCD,1,(0x11)}, 
{0xCE,1,(0x3D)}, 
{0xCF,1,(0x09)}, 
{0xD0,1,(0x0A)}, 
{0xD1,1,(0x57)}, 
{0xD2,1,(0x67)}, 
{0xD3,1,(0x39)}, 
{0xFF,3,(0x98,0x81,0x00)},
*/


{0xFF,3,{0x98,0x81,0x03}},

//GIP_1
{0x01,1,{0x00}},
{0x02,1,{0x00}},
{0x03,1,{0x53}},
{0x04,1,{0x13}},
{0x05,1,{0x13}},
{0x06,1,{0x06}},
{0x07,1,{0x00}},
{0x08,1,{0x04}},
{0x09,1,{0x00}},  //04
{0x0A,1,{0x00}},  //03
{0x0B,1,{0x00}},  //03
{0x0C,1,{0x00}},
{0x0D,1,{0x00}},
{0x0E,1,{0x00}},
{0x0F,1,{0x00}},  //04
{0x10,1,{0x00}},  //04
{0x11,1,{0x00}},
{0x12,1,{0x00}},
{0x13,1,{0x00}},
{0x14,1,{0x00}},
{0x15,1,{0x00}},
{0x16,1,{0x00}},
{0x17,1,{0x00}},
{0x18,1,{0x00}},
{0x19,1,{0x00}},
{0x1A,1,{0x00}},
{0x1B,1,{0x00}},
{0x1C,1,{0x00}},
{0x1D,1,{0x00}},
{0x1E,1,{0xC0}},
{0x1F,1,{0x80}},
{0x20,1,{0x04}},
{0x21,1,{0x0B}},
{0x22,1,{0x00}},
{0x23,1,{0x00}},
{0x24,1,{0x00}},
{0x25,1,{0x00}},
{0x26,1,{0x00}},
{0x27,1,{0x00}},
{0x28,1,{0x55}},
{0x29,1,{0x03}},
{0x2A,1,{0x00}},
{0x2B,1,{0x00}},
{0x2C,1,{0x00}},  //06
{0x2D,1,{0x00}},
{0x2E,1,{0x00}},
{0x2F,1,{0x00}},
{0x30,1,{0x00}},
{0x31,1,{0x00}},
{0x32,1,{0x00}},
{0x33,1,{0x00}},  //30
{0x34,1,{0x04}},
{0x35,1,{0x05}},
{0x36,1,{0x05}},
{0x37,1,{0x00}},
{0x38,1,{0x3c}},  //00
{0x39,1,{0x00}},
{0x3A,1,{0x40}},
{0x3B,1,{0x40}},
{0x3C,1,{0x00}},
{0x3D,1,{0x00}},
{0x3E,1,{0x00}},
{0x3F,1,{0x00}},
{0x40,1,{0x00}},
{0x41,1,{0x00}},
{0x42,1,{0x00}},
{0x43,1,{0x00}},
{0x44,1,{0x00}},
{0x50,1,{0x01}},
{0x51,1,{0x23}},
{0x52,1,{0x45}},
{0x53,1,{0x67}},
{0x54,1,{0x89}},
{0x55,1,{0xAB}},
{0x56,1,{0x01}},
{0x57,1,{0x23}},
{0x58,1,{0x45}},
{0x59,1,{0x67}},
{0x5A,1,{0x89}},
{0x5B,1,{0xAB}},
{0x5C,1,{0xCD}},
{0x5D,1,{0xEF}},
{0x5E,1,{0x01}},
{0x5F,1,{0x14}},
{0x60,1,{0x15}},
{0x61,1,{0x0C}},
{0x62,1,{0x0D}},
{0x63,1,{0x0E}},
{0x64,1,{0x0F}},
{0x65,1,{0x10}},
{0x66,1,{0x11}},
{0x67,1,{0x08}},
{0x68,1,{0x02}},
{0x69,1,{0x0A}},
{0x6A,1,{0x02}},
{0x6B,1,{0x02}},
{0x6C,1,{0x02}},
{0x6D,1,{0x02}},
{0x6E,1,{0x02}},
{0x6F,1,{0x02}},
{0x70,1,{0x02}},
{0x71,1,{0x02}},
{0x72,1,{0x06}},
{0x73,1,{0x02}},
{0x74,1,{0x02}},
{0x75,1,{0x14}},
{0x76,1,{0x15}},
{0x77,1,{0x11}},
{0x78,1,{0x10}},
{0x79,1,{0x0F}},
{0x7A,1,{0x0E}},
{0x7B,1,{0x0D}},
{0x7C,1,{0x0C}},
{0x7D,1,{0x06}},
{0x7E,1,{0x02}},
{0x7F,1,{0x0A}},
{0x80,1,{0x02}},
{0x81,1,{0x02}},
{0x82,1,{0x02}},
{0x83,1,{0x02}},
{0x84,1,{0x02}},
{0x85,1,{0x02}},
{0x86,1,{0x02}},
{0x87,1,{0x02}},
{0x88,1,{0x08}},
{0x89,1,{0x02}},
{0x8A,1,{0x02}},

{0xFF,3,{0x98,0x81,0x04}},
{0x6c,1,{0x15}},               
{0x6e,1,{0x3b}},               
{0x6f,1,{0x53}},               
{0x3a,1,{0xa4}},               
{0x8d,1,{0x15}},              
{0x87,1,{0xba}}, 
{0xb2,1,{0xd1}},              
{0x26,1,{0x76}},

//{0x69,1,{0x57}},
//{0x33,1,{0x44}},
//{0x88,1,{0x0B}},


{0xFF,3,{0x98,0x81,0x01}},
{0x22,1,{0x0A}},				
{0x31,1,{0x00}},      			
//{0x53,1,{0x80}},		           //VCOM1	  51
{0x55,1,{0x88}},
{0x50,1,{0xA6}},
{0x51,1,{0xA6}},
{0x60,1,{0x14}},
{0xA0,1,{0x08}},
{0xA1,1,{0x27}},
{0xA2,1,{0x36}},
{0xA3,1,{0x15}},
{0xA4,1,{0x17}},
{0xA5,1,{0x2B}},
{0xA6,1,{0x1E}},
{0xA7,1,{0x1F}},
{0xA8,1,{0x96}},
{0xA9,1,{0x1C}},
{0xAA,1,{0x28}},
{0xAB,1,{0x7C}},
{0xAC,1,{0x1B}},
{0xAD,1,{0x1A}},
{0xAE,1,{0x4D}},
{0xAF,1,{0x23}},
{0xB0,1,{0x29}},
{0xB1,1,{0x4B}},
{0xB2,1,{0x5A}},
{0xB3,1,{0x2C}},
{0xC0,1,{0x08}},
{0xC1,1,{0x26}},
{0xC2,1,{0x36}},
{0xC3,1,{0x15}},
{0xC4,1,{0x17}},
{0xC5,1,{0x2B}},
{0xC6,1,{0x1F}},
{0xC7,1,{0x1F}},
{0xC8,1,{0x96}},
{0xC9,1,{0x1C}},
{0xCA,1,{0x29}},
{0xCB,1,{0x7C}},
{0xCC,1,{0x1A}},
{0xCD,1,{0x19}},
{0xCE,1,{0x4D}},
{0xCF,1,{0x22}},
{0xD0,1,{0x29}},
{0xD1,1,{0x4B}},
{0xD2,1,{0x59}},
{0xD3,1,{0x2C}},


{0xFF,3,{0x98,0x81,0x00}},
{0x35,1,{0x00}},//lin
{0x11,01,{0x00}},
{REGFLAG_DELAY, 120, {}},  
{0x29,01,{0x00}},	
{REGFLAG_DELAY, 20, {}},  

};






//static int vcom=0x40;
static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
    unsigned int i;

    for(i = 0; i < count; i++) {
        
        unsigned cmd;
        cmd = table[i].cmd;
        
        switch (cmd) {
			/*case 0xd9:
			table[i].para_list[0]=vcom;
			dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
            vcom+=2;
			break;
			*/
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
                
            case REGFLAG_END_OF_TABLE :
                break;
                
            default:
                dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
                MDELAY(1);
        }
    }
    
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
    memset(params, 0, sizeof(LCM_PARAMS));
    params->type   = LCM_TYPE_DSI;
    params->width  = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;

    // enable tearing-free
    params->dbi.te_mode             = LCM_DBI_TE_MODE_DISABLED;
    params->dbi.te_edge_polarity        = LCM_POLARITY_RISING;



    params->dsi.mode   = SYNC_EVENT_VDO_MODE;


    // DSI
    /* Command mode setting */
    params->dsi.LANE_NUM                = LCM_FOUR_LANE;
    //The following defined the fomat for data coming from LCD engine. 
    params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST; 
    params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
    params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;
    // Highly depends on LCD driver capability.
    // Not support in MT6573
    params->dsi.packet_size=256;
    // Video mode setting       
    params->dsi.intermediat_buffer_num = 2;
    params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
     	params->physical_width = 69;
     	params->physical_height = 122;
	params->dsi.vertical_sync_active				= 8;
	params->dsi.vertical_backporch					= 24;
	params->dsi.vertical_frontporch					= 16; 
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 

	params->dsi.horizontal_sync_active				= 40;	//2;
	params->dsi.horizontal_backporch				= 100;//120;	//60;	//42;
	params->dsi.horizontal_frontporch				= 70;
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
  params->dsi.HS_TRAIL = 20;
// Bit rate calculation
//1 Every lane speed
//params->dsi.pll_div1=0;		// div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps  1:273Mbps
//params->dsi.pll_div2=0;		// div2=0,1,2,3;div1_real=1,2,4,4	
//params->dsi.fbk_div =0x12;    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	

// zhangxiaofei add for test
params->dsi.PLL_CLOCK = 220;//208;	
  params->dsi.esd_check_enable = 1;
  params->dsi.customization_esd_check_enable = 1;
  params->dsi.lcm_esd_check_table[0].cmd          = 0x0a;
  params->dsi.lcm_esd_check_table[0].count        = 1;
  params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;
}

static void lcm_init(void)
{
    SET_RESET_PIN(1);
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(120);

    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_suspend(void)
{





    unsigned int data_array[16];


    data_array[0]=0x00280500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(10);
    data_array[0]=0x00100500;

    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(100);
    SET_RESET_PIN(1);
    MDELAY(120);
    SET_RESET_PIN(0);
	MDELAY(1); // 1ms
	SET_RESET_PIN(1);
	MDELAY(120); 
}


    //for LGE backlight IC mapping table


    // Refresh value of backlight level.


static void lcm_resume(void)
{
     lcm_init();

    /// please notice: the max return packet size is 1
    /// if you want to change it, you can refer to the following marked code
    /// but read_reg currently only support read no more than 4 bytes....
    /// if you need to read more, please let BinHan knows.
    /*
            unsigned int data_array[16];
            unsigned int max_return_size = 1;
            
            data_array[0]= 0x00003700 | (max_return_size << 16);    
            
            dsi_set_cmdq(&data_array, 1, 1);
    


	unsigned int data_array[16];

    MDELAY(100);
    data_array[0]=0x00290500;
    dsi_set_cmdq(data_array,1,1);
    MDELAY(10);
*/
}


static unsigned int lcm_compare_id(void)
{
        int array[4];
        char buffer[5];
		unsigned int id_high;
		unsigned int id_low;
		unsigned int id=0;
        //Do reset here
        SET_RESET_PIN(1);
        MDELAY(10);
        SET_RESET_PIN(0);
		MDELAY(10);
        SET_RESET_PIN(1);
		MDELAY(30);
       
        array[0]=0x00043902;
	array[1]=0x018198ff;
	dsi_set_cmdq(array, 2, 1);
        MDELAY(10);
        array[0]=0x00023700;
        dsi_set_cmdq(array, 1, 1);
        //read_reg_v2(0x04, buffer, 3);
    
        read_reg_v2(0x00, buffer,1);
        id_high = buffer[0]; ///////////////////////0x98
        read_reg_v2(0x01, buffer,1);
	id_low = buffer[0]; ///////////////////////0x06
       // id = (id_midd &lt;&lt; 8) | id_low;
	id = (id_high << 8) | id_low;

		#if defined(BUILD_LK)
		printf("ILI9881 %s id_high = 0x%04x, id_low = 0x%04x\n,id=0x%x\n", __func__, id_high, id_low,id);
#else
		printk("ILI9881 %s id_high = 0x%04x, id_low = 0x%04x\n,id=0x%x\n", __func__, id_high, id_low,id);
#endif
	//return 1;	
	 return (LCM_ID == id)?1:0;

}

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER hct_ili9881c_dsi_vdo_hd_cmi_55_hf = 
{
    .name           = "hct_ili9881c_dsi_vdo_hd_cmi_55_hf",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params     = lcm_get_params,
    .init           = lcm_init,
    .suspend        = lcm_suspend,
    .resume         = lcm_resume,   
    .compare_id    = lcm_compare_id,    
#if 0//defined(LCM_DSI_CMD_MODE)
    //.set_backlight    = lcm_setbacklight,
    //.esd_check   = lcm_esd_check, 
    //.esd_recover   = lcm_esd_recover, 
    .update         = lcm_update,
#endif  //wqtao
};

