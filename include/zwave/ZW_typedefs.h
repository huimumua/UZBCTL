/*******************************  ZW_typedefs.h  *******************************
 *           #######
 *           ##  ##
 *           #  ##    ####   #####    #####  ##  ##   #####
 *             ##    ##  ##  ##  ##  ##      ##  ##  ##
 *            ##  #  ######  ##  ##   ####   ##  ##   ####
 *           ##  ##  ##      ##  ##      ##   #####      ##
 *          #######   ####   ##  ##  #####       ##  #####
 *                                           #####
 *          Products that speak Z-Wave work together better
 *
 *              Copyright (c) 2008
 *              Zensys A/S
 *              Denmark
 *
 *              All Rights Reserved
 *
 *    This source file is subject to the terms and conditions of the
 *    Zensys Software License Agreement which restricts the manner
 *    in which it may be used.
 *
 *---------------------------------------------------------------------------
 *
 * Description: Module description
 *
 * Author:   Ivar Jeppesen
 *
 * Last Changed By:  $Author: jbu $
 * Revision:         $Revision: 17876 $
 * Last Changed:     $Date: 2010-06-10 21:25:40 +0800 (Thu, 10 Jun 2010) $
 *
 ****************************************************************************/
#ifndef _ZW_TYPEDEFS_H_
#define _ZW_TYPEDEFS_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

/****************************************************************************/
/*                                 MACROS                                   */
/****************************************************************************/

/* offset of field m in a struct s */
#define offsetof(s,m)   (WORD)( (BYTE_P)&(((s *)0)->m) - (BYTE_P)0 )

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/
#ifndef BYTE
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;

#define IIBYTE BYTE idata  /* Internal indexed data byte */
#define IBYTE  BYTE data   /* Internal data byte */
#define IWORD  WORD data   /* Internal data word */
#define IDWORD DWORD data  /* Internal data double word*/

#define XBYTE  BYTE xdata  /* External data byte */
#define XWORD  WORD xdata  /* External data word */
#define XDWORD DWORD xdata /* External data double word */
#define BBYTE  BYTE bdata  /* Internal bit adressable byte */

#ifdef ZW040x
#define PBYTE  XBYTE  /* External data byte residing in lower 256 byte in xdata */
#define PWORD  XWORD  /* External data word residing in lower 256 byte in xdata */
#define PDWORD XDWORD /* External data double word residing in lower 256 byte in xdata */
#else
#define PBYTE  BYTE pdata  /* External data byte residing in lower 256 byte in xdata */
#define PWORD  WORD pdata  /* External data word residing in lower 256 byte in xdata */
#define PDWORD DWORD pdata /* External data double word residing in lower 256 byte in xdata */
#endif

#define BOOL   bit         /* Internal bit */

#define CODE   code        /* Used for defining callback function which allways */
                           /* resides in code space. */

typedef BYTE * BYTE_P;
typedef WORD * WORD_P;
typedef DWORD * DWORD_P;
#endif

#ifndef NULL
#define NULL  (0)
#endif

#ifndef TRUE
#define TRUE  (1)
#define FALSE (0)
#endif

/* Define for making easy and consistent callback definitions */
#define VOID_CALLBACKFUNC(completedFunc)  void (CODE *completedFunc)

#ifdef LIB_PATCH_ENABLE
#define PATCHABLE_FUNC reentrant
#define STATIC
#define BOOL_ BYTE
#else
#define PATCHABLE_FUNC
#define STATIC static
#define BOOL_ BOOL
#endif

#endif /* _ZW_TYPEDEFS_H_ */
