/*******************************************************************************
       Copyright (c) 2005, Infineon Technologies.  All rights reserved.

                               No Warranty
   Because the program is licensed free of charge, there is no warranty for
   the program, to the extent permitted by applicable law.  Except when
   otherwise stated in writing the copyright holders and/or other parties
   provide the program "as is" without warranty of any kind, either
   expressed or implied, including, but not limited to, the implied
   warranties of merchantability and fitness for a particular purpose. The
   entire risk as to the quality and performance of the program is with
   you.  should the program prove defective, you assume the cost of all
   necessary servicing, repair or correction.

   In no event unless required by applicable law or agreed to in writing
   will any copyright holder, or any other party who may modify and/or
   redistribute the program as permitted above, be liable to you for
   damages, including any general, special, incidental or consequential
   damages arising out of the use or inability to use the program
   (including but not limited to loss of data or data being rendered
   inaccurate or losses sustained by you or third parties or a failure of
   the program to operate with any other programs), even if such holder or
   other party has been advised of the possibility of such damages.
********************************************************************************
   Module      : driver_vinetic_bbd.c
   Desription  :
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_vinetic_api.h"
#include "drv_vinetic_dwnld.h"
#include "drv_vinetic_bbd.h"

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* ============================= */
/* Global variable definition    */
/* ============================= */

/* registration of supported vinetic bbd blocks
   downloadable channelwise */
const IFX_uint16_t VINETIC_CH_BBD_Blocks[] =
{
   BBD_VIN_CRAM_BLOCK,
   BBD_VIN_SLIC_BLOCK,
   BBD_VINETIC_RING_CFG_BLOCK,
   BBD_VINETIC_DC_THRESHOLDS_BLOCK,
   BBD_VIN_AC_BLOCK,
   BBD_VIN_FWLECCOEFS_BLOCK
};

/* ============================= */
/* Local function definition     */
/* ============================= */

/* ============================= */
/* Global function definition    */
/* ============================= */

/**
   Sets EDSP pointers as read from BBD buffer, if any.
\param
   pBBDUsr  - handle to user bbd buffer
\param
   pEdsp    - handle to VINETIC_EDSP_FWDWLD structure
\param
   ram      - DRAM or PRAM
\return
   none
\remark
   -  Pointer pEdsp will be filled accordingly, if there is any EDSP data in BBD
      buffer.
   -  Caller must make sure that ptrs are valid and that bbd integrity is ok.
*/
IFX_void_t VINETIC_BBD_SetEdspPtr (bbd_format_t *pBBDUsr,
                                   VINETIC_EDSP_FWDWLD *pEdsp,
                                   VINETIC_FW_RAM ram)
{
   IFX_uint8_t  **pRam = NULL;
   IFX_uint32_t *ram_size = NULL;

   switch (ram)
   {
      case  P_RAM:
         pRam     = &pEdsp->pPRAMfw;
         ram_size = &pEdsp->pram_size;
         break;
      case  D_RAM:
         pRam     = &pEdsp->pDRAMfw;
         ram_size = &pEdsp->dram_size;
         break;
   }
   if (pBBDUsr->buf == NULL)
   {
      if (pRam != NULL)
         *pRam = NULL;
      TRACE (VINETIC, DBG_LEVEL_NORMAL,
             ("WARN: No BBD Buffer => No EDSP RAM!\r\n"));
      return;
   }
   /* parse BBD buffer and get RAM ptr
      \todo attend this with correct values once PRAM/DRAM specified in
            BBD format.*/
   if ((pRam != NULL) && (ram_size != 0))
   {
      *pRam = NULL;
      *ram_size  = 0;
   }
   TRACE (VINETIC, DBG_LEVEL_NORMAL,
          ("WARN: EDSP RAM not yet supported by BBD!\r\n"));
}

/**
   Sets Others pointers (!= EDSP) as read from BBD buffer, if any.
\param
   pBBDUsr  - handle to user bbd buffer
\param
   pPtr     - handle to bbd_format_t structure which stores download data.
\param
   nBBDTag  - BBD tag of download type.
\return
   none
\remark
   -  Pointer pPtr will be filled accordingly, if there is any data of given
      tag in BBD  buffer.
   -  Caller must make sure that ptrs are valid and that bbd integrity is ok.
*/
IFX_void_t VINETIC_BBD_SetPtr (bbd_format_t *pBBDUsr, bbd_format_t *pPtr,
                               IFX_uint16_t nBBDTag)
{
   bbd_block_t bbd_block;

   if (pBBDUsr->buf == NULL)
   {
      pPtr->buf = NULL;
      TRACE (VINETIC, DBG_LEVEL_LOW,
             ("WARN: No BBD Buffer => No Block of Tag %04X!\r\n", nBBDTag));
      return;
   }
   /* set bbd block characteristics */
   memset (&bbd_block, 0, sizeof (bbd_block));
   bbd_block.identifier = BBD_VIN_MAGIC;
   bbd_block.tag        = nBBDTag;
   /* go through bbd buffer and check if there is any block of given tag */
   bbd_get_block (pBBDUsr, &bbd_block);
   if ((bbd_block.data == NULL) || (bbd_block.size == 0))
   {
      /* sorry ... no block of given tag available in bbd data buffer */
      pPtr->buf = NULL;
      TRACE (VINETIC, DBG_LEVEL_NORMAL,
             ("No Block of Tag 0x%4X in given BBD buffer\r\n", nBBDTag));
      return;
   }
   /* set pointers to BBD  */
   pPtr->buf  = pBBDUsr->buf;
   pPtr->size = pBBDUsr->size;
}

/**
   Does a BBD download on given channel.
\param
   pCh   - handle to channel structure
\param
   pBBD  - handle to user BBD buffer
\return
   IFX_SUCCESS or IFX_ERROR
\remark
   ioctl call to download a BBD buffer on given channel
*/
IFX_int32_t VINETIC_BBD_Download (VINETIC_CHANNEL *pCh, bbd_format_t *pBBD)
{
   IFX_int32_t       err = IFX_SUCCESS;
   IFX_uint32_t      i, j, block_num;
   IFX_uint16_t      slic_val;
   bbd_format_t      bbd;
   bbd_error_t       bbd_err;
   bbd_block_t       bbd_vin_block;
   VINETIC_RingCfg_t ringCfg;
   VINETIC_DcThr_t   dcThr;

   /* initializations */
   memset (&bbd, 0, sizeof(bbd));
   memset (&bbd_vin_block, 0, sizeof (bbd_vin_block));
   memset (&ringCfg, 0, sizeof (ringCfg));
   memset (&dcThr, 0, sizeof (dcThr));
   /* get local memory for bbd buffer */
   bbd.buf  = OS_MapBuffer(pBBD->buf, pBBD->size);
   if (bbd.buf == NULL)
      return IFX_ERROR;
   /* set size */
   bbd.size = pBBD->size;
   /* check BBD Buffer integrity */
   bbd_err = bbd_check_integrity (&bbd, BBD_VIN_MAGIC);
   if (bbd_err != BBD_INTG_OK)
   {
      OS_UnmapBuffer(bbd.buf);
      return IFX_ERROR;
   }
   /* let go through bbd buffer and download
      any channel block of relevance found */
   block_num  = (sizeof (VINETIC_CH_BBD_Blocks) / sizeof (IFX_uint16_t));
   bbd_vin_block.identifier = BBD_VIN_MAGIC;
   for (i = 0; i < block_num; i++)
   {
      bbd_vin_block.tag = VINETIC_CH_BBD_Blocks [i];
      /* sniff blocks of this tag upto maximum allowed */
      for (j = 0; j < BBD_VIN_BLOCK_MAXDWNLD ; j++)
      {
         /* look at block of this index and download it if available */
         bbd_vin_block.index = j;
         bbd_get_block (&bbd, &bbd_vin_block);
         if ((bbd_vin_block.data == NULL) || (bbd_vin_block.size == 0))
            break;
         switch (bbd_vin_block.tag)
         {
            case  BBD_VIN_CRAM_BLOCK:
               err = VINETIC_Host_BBD_DownloadChCram (pCh, &bbd_vin_block);
               break;
            case  BBD_VIN_AC_BLOCK:
               err = VINETIC_Host_BBD_DownloadChAC (pCh, &bbd_vin_block);
               break;
            case BBD_VIN_SLIC_BLOCK:
               cpb2w (&slic_val, bbd_vin_block.data, sizeof (IFX_uint16_t));
               err = VINETIC_Host_SetSlic (pCh, IFX_FALSE, slic_val);
               break;
            case BBD_VINETIC_RING_CFG_BLOCK:
               cpb2w (&ringCfg.ring_freq, &bbd_vin_block.data [0],
                      sizeof (IFX_uint16_t));
               cpb2w (&ringCfg.ring_amp, &bbd_vin_block.data [2],
                      sizeof (IFX_uint16_t));
               cpb2w (&ringCfg.ring_hook_level, &bbd_vin_block.data [4],
                      sizeof (IFX_uint16_t));
               /* version 2 or greater of the bbd block also has a field for
                  the fast ring trip type */
               if (bbd_vin_block.version >= 2)
               {
                  IFX_uint16_t nVal;
                  cpb2w (&nVal, &bbd_vin_block.data [6], sizeof (IFX_uint16_t));
                  switch (nVal)
                  {
                     case VINETIC_RING_TRIP_TYPE_NORMAL:
                     case VINETIC_RING_TRIP_TYPE_FAST:
                        ringCfg.ring_trip_type = (VINETIC_RING_TRIP_TYPE_t)nVal;
                        break;
                     default:
                        err = IFX_ERROR;
                        break;
                  }
               }
               else
               {
                  /* for all other versions of this block, use the default
                     fast ring trip setting (normal) */
                  ringCfg.ring_trip_type = VINETIC_RING_TRIP_TYPE_NORMAL;
               }
               /* version 3 or greater of bbd block also has the field for
                  ring trip dup time */
               if (bbd_vin_block.version >= 3)
               {
                  cpb2w (&ringCfg.ring_trip_dup_time, &bbd_vin_block.data [8],
                         sizeof (IFX_uint16_t));
               }
               else
               {
                  /* for all other versions of this block, use the default
                     ring trip dup time */
                  ringCfg.ring_trip_dup_time = VIN_2CPE_DEFAULT_RING_TRIP_DUP_TIME;
               }
               if (err == IFX_SUCCESS)
                  err = VINETIC_Host_RingCfg (pCh, IFX_FALSE, &ringCfg);
               break;
            case BBD_VINETIC_DC_THRESHOLDS_BLOCK:
               cpb2w (&dcThr.hook_dup_time, &bbd_vin_block.data [0],
                      sizeof (IFX_uint16_t));
               cpb2w (&dcThr.onhook_time, &bbd_vin_block.data [2],
                      sizeof (IFX_uint16_t));
               /* version 2 or greater of bbd block also has the field for
                  overtemperature dup time */
               if (bbd_vin_block.version >= 2)
               {
                  cpb2w (&dcThr.ovt_dup_time, &bbd_vin_block.data [4],
                         sizeof (IFX_uint16_t));
               }
               else
               {
                  /* for all other versions of this block, use the default
                     overtemperature dup time */
                  dcThr.ovt_dup_time = VIN_2CPE_DEFAULT_OVT_DUP_TIME;
               }
               err =  VINETIC_Host_SetDcThr (pCh, IFX_FALSE, &dcThr);
               break;
            default:
               TRACE (VINETIC, DBG_LEVEL_HIGH, ("WARN:Block tag 0x%04X not "
                      "yet supported\r\n", bbd_vin_block.tag));
               /* parsing continues */
               break;
         }
         /* stop every thing if the previous download went wrong */
         if (err == IFX_ERROR)
            break;
      }
      /* stop every thing if the previous download went wrong */
      if (err == IFX_ERROR)
         break;
   }

   OS_UnmapBuffer(bbd.buf);

   return err;
}


