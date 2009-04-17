/*
   +----------------------------------------------------------------------+   
   |                                                                      |
   |                     OCILIB - C Driver for Oracle                     |
   |                                                                      |
   |                      (C Wrapper for Oracle OCI)                      |
   |                                                                      |
   +----------------------------------------------------------------------+
   |                      Website : http://ocilib.net                     |
   +----------------------------------------------------------------------+
   |               Copyright (c) 2007-2009 Vincent ROGIER                 |
   +----------------------------------------------------------------------+
   | This library is free software; you can redistribute it and/or        |
   | modify it under the terms of the GNU Library General Public          |
   | License as published by the Free Software Foundation; either         |
   | version 2 of the License, or (at your option) any later version.     |
   |                                                                      |
   | This library is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    |
   | Library General Public License for more details.                     |
   |                                                                      |
   | You should have received a copy of the GNU Library General Public    |
   | License along with this library; if not, write to the Free           |
   | Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   |
   +----------------------------------------------------------------------+
   |          Author: Vincent ROGIER <vince.rogier@gmail.com>             |
   +----------------------------------------------------------------------+ 
*/

/* ------------------------------------------------------------------------ *
 * $Id: lob.c, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                             PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_LobInit
 * ------------------------------------------------------------------------ */

 OCI_Lob * OCI_LobInit(OCI_Connection *con, OCI_Lob **plob,
                       OCILobLocator *handle, ub4 type)
{
    ub2 csid      = OCI_DEFAULT;
    ub1 csfrm     = OCI_DEFAULT;
    OCI_Lob * lob = NULL;
    boolean res   = TRUE;
    ub1 lobtype   = 0;

    OCI_CHECK(plob == NULL, NULL);

    if (*plob == NULL)
        *plob = (OCI_Lob *) OCI_MemAlloc(OCI_IPC_LOB, sizeof(*lob), 1, TRUE);

    if (*plob != NULL)
    {
        lob = *plob;

        lob->type   = type;
        lob->con    = con;
        lob->handle = handle;
        lob->offset = 1;

        if (lob->handle == NULL)
        {
            ub4 empty = 0;

            /* allocate handle for non fetched lob (temporary lob) */

            lob->hstate = OCI_OBJECT_ALLOCATED;

            if (lob->type == OCI_NCLOB)
            {
                csfrm   = SQLCS_NCHAR;
                lobtype = OCI_TEMP_CLOB;
            }
            else if (lob->type == OCI_CLOB)
            {
                csfrm   = SQLCS_IMPLICIT;
                lobtype = OCI_TEMP_CLOB;
            }
            else
                lobtype = OCI_TEMP_BLOB;

            res = (OCI_SUCCESS == OCI_DescriptorAlloc((dvoid  *) OCILib.env,
                                                      (dvoid **) (void *) &lob->handle,
                                                      (ub4) OCI_DTYPE_LOB, 
                                                      (size_t) 0, (dvoid **) NULL));

            OCI_CALL2
            (
                res, lob->con, 
                
                OCIAttrSet((dvoid *) lob->handle, (ub4) OCI_DTYPE_LOB,
                           (dvoid *) &empty, (ub4) sizeof(empty), 
                           (ub4) OCI_ATTR_LOBEMPTY, lob->con->err)
            )

            OCI_CALL2
            (
                res, lob->con, 
                
                OCILobCreateTemporary(lob->con->cxt, lob->con->err,
                                      lob->handle, csid, csfrm, lobtype,
                                      FALSE, OCI_DURATION_SESSION)
            )

        }
        else
            lob->hstate = OCI_OBJECT_FETCHED_CLEAN;
    }
    else
        res = FALSE;

    /* check for failure */

    if (res == FALSE)
    {
        OCI_LobFree(lob);
        lob = NULL;
    }

    return lob;
}

/* ************************************************************************ *
 *                            PUBLIC FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_LobCreate
 * ------------------------------------------------------------------------ */

OCI_Lob * OCI_API OCI_LobCreate(OCI_Connection *con, unsigned int type)
{
    OCI_Lob *lob = NULL;

    OCI_CHECK_INITIALIZED(NULL);

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    lob = OCI_LobInit(con, &lob, NULL, type);

    OCI_RESULT(lob != NULL);

    return lob;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobFree(OCI_Lob *lob)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, FALSE);
 
    OCI_CHECK_OBJECT_FETCHED(lob, FALSE);

    if (OCI_LobIsTemporary(lob) == TRUE)
    {
        OCI_CALL2
        (
            res, lob->con, 
            
            OCILobFreeTemporary(lob->con->cxt, lob->con->err, lob->handle)
        )
    }

    if (lob->hstate == OCI_OBJECT_ALLOCATED)
    {
        OCI_DescriptorFree((dvoid *) lob->handle, (ub4) OCI_DTYPE_LOB);
    }

    OCI_FREE(lob);

    OCI_RESULT(res);
    
    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobGetType
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_LobGetType(OCI_Lob *lob)
{
    OCI_CHECK_PTR(OCI_IPC_LOB, lob,OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return lob->type;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobSeek
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobSeek(OCI_Lob *lob, big_uint offset, unsigned int mode)
{
    boolean res   = TRUE;
    big_uint size = 0;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, FALSE);

    size = OCI_LobGetLength(lob);

    if ((mode == OCI_SEEK_CUR && (offset + lob->offset-1) > size))
        res = FALSE;
    else if (mode == OCI_SEEK_SET)
        lob->offset  = offset + 1;
    else if (mode == OCI_SEEK_END)
        lob->offset  = size-offset + 1;
    else if (mode == OCI_SEEK_CUR)
        lob->offset += offset;
    else
        res = FALSE;

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobGetOffset
 * ------------------------------------------------------------------------ */

big_uint OCI_API OCI_LobGetOffset(OCI_Lob *lob)
{
    OCI_CHECK_PTR(OCI_IPC_LOB, lob, 0);

    OCI_RESULT(TRUE);

    return lob->offset - 1;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobRead
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_LobRead(OCI_Lob *lob, void *buffer, unsigned int len)
{
    boolean res  = TRUE;
    ub4 size_in  = 0;
    ub4 size_out = 0;
    ub2 csid     = 0;
    ub1 csfrm    = 0;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, 0);
    OCI_CHECK_MIN(lob->con, NULL, len, 1, 0);

    size_out = size_in = len;

    if (lob->type != OCI_BLOB)
    {

#ifndef OCI_CHARSET_ANSI

        csid = OCI_UTF16ID;

#endif        
        
        size_in *= sizeof(odtext);
    }

    if (lob->type == OCI_NCLOB)
        csfrm = SQLCS_NCHAR;
    else
        csfrm = SQLCS_IMPLICIT;

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 size_char = (ub8) len;
        ub8 size_byte = (ub8) size_in;
        
        OCI_CALL2
        (
            res, lob->con, 
            
            OCILobRead2(lob->con->cxt, lob->con->err,
                        lob->handle, &size_byte,
                        &size_char,  (ub8) lob->offset, 
                        buffer,(ub8) size_in, 
                        (ub1) OCI_ONE_PIECE,  (void *) NULL, 
                        NULL, csid, csfrm)
        )

        if (lob->type == OCI_BLOB)
            size_out = (ub4) size_byte;
        else
            size_out = (ub4) size_char;
    }

    else

#endif

    {
        ub4 offset = (ub4) lob->offset;

        OCI_CALL2
        (
            res, lob->con, 
            
            OCILobRead(lob->con->cxt, lob->con->err,
                       lob->handle,  &size_out, offset,
                       buffer, size_in, (void *) NULL,
                       NULL, csid, csfrm)
        )
    }

    if (res == TRUE)
    {
        if (lob->type != OCI_BLOB)
            OCI_ConvertString(buffer, (int) size_out, sizeof(odtext), sizeof(dtext));
        
        lob->offset += (big_uint) size_out;
    }

    OCI_RESULT(res);

    return size_out;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobWrite
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_LobWrite(OCI_Lob *lob, void *buffer, unsigned int len)
{
    boolean res  = TRUE;
    ub4 size_in  = 0;
    ub4 size_out = 0;
    ub2 csid     = 0;
    ub1 csfrm    = 0;
    void *obuf   = NULL;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, 0);
    OCI_CHECK_MIN(lob->con, NULL, len, 1, 0);

    size_out = size_in = len;
    
    if (lob->type != OCI_BLOB)
    {

#ifndef OCI_CHARSET_ANSI

        csid = OCI_UTF16ID;

#endif

        size_in *= sizeof(dtext);
        obuf     = OCI_GetInputDataString(buffer, (int *) &size_in);
    }
    else
        obuf = buffer;

    if (lob->type == OCI_NCLOB)
        csfrm = SQLCS_NCHAR;
    else
        csfrm = SQLCS_IMPLICIT;

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 size_char = (ub8) len;
        ub8 size_byte = (ub8) size_in;
        
        OCI_CALL2
        (
            res, lob->con, 
            
            OCILobWrite2(lob->con->cxt, lob->con->err, 
                         lob->handle, &size_byte,
                         &size_char, (ub8) lob->offset, 
                         obuf, (ub8) size_in,
                         (ub1) OCI_ONE_PIECE, (void *) NULL,
                         NULL , csid, csfrm)
        )

        if (lob->type == OCI_BLOB)
            size_out = (ub4) size_byte;
        else
            size_out = (ub4) size_char;
    }

    else

#endif

    {
        ub4 offset = (ub4) lob->offset;
   
        OCI_CALL2
        (
            res, lob->con, 
            
            OCILobWrite(lob->con->cxt, lob->con->err,
                        lob->handle, &size_out,
                        offset, obuf, size_in, 
                        (ub1) OCI_ONE_PIECE, (void *) NULL, 
                        NULL, csid, csfrm)
        )
    }

    if (lob->type != OCI_BLOB)
        OCI_ReleaseDataString(obuf);

    if (res == TRUE)
        lob->offset += (big_uint) size_out;

    OCI_RESULT(res);

    return size_out;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobTruncate
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobTruncate(OCI_Lob *lob, big_uint size)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, FALSE);

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
         OCI_CALL2
         (
            res, lob->con, 
            
            OCILobTrim2(lob->con->cxt, lob->con->err, lob->handle, (ub8) size)
         )
    }
    else

#endif
 
    {
        ub4 size32 = (ub4) size;

        OCI_CALL2
        (
            res, lob->con, 
            
            OCILobTrim(lob->con->cxt, lob->con->err, lob->handle, size32)
        )
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobErase
 * ------------------------------------------------------------------------ */

big_uint OCI_API OCI_LobErase(OCI_Lob *lob, big_uint offset, big_uint size)
{
    boolean res  = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, 0);
    OCI_CHECK_MIN(lob->con, NULL, size, 1, 0);

 #ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        OCI_CALL2
        (
            res, lob->con, 
            
            OCILobErase2(lob->con->cxt, lob->con->err, lob->handle, 
                         (ub8 *) &size, (ub8) (offset + 1))
         )
    }
    else

#endif
 
    {
        ub4 size32   = (ub4) size;
        ub4 offset32 = (ub4) offset;

        OCI_CALL2
        (
            res, lob->con, 
            
            OCILobErase(lob->con->cxt, lob->con->err, lob->handle, 
                        &size32, offset32 + 1)
        )

        size = (big_uint) size32;
    }
                            
    OCI_RESULT(res);

    return size;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobGetLength
 * ------------------------------------------------------------------------ */

big_uint OCI_API OCI_LobGetLength(OCI_Lob *lob)
{
    boolean res   = TRUE;
    big_uint size = 0;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, 0);
 
#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        OCI_CALL2
        (
            res, lob->con, 
            
            OCILobGetLength2(lob->con->cxt, lob->con->err, lob->handle, 
                             (ub8 *) &size)
        )
    }
    else

#endif
 
    {
        ub4 size32 = (ub4) size;
   
        OCI_CALL2
        (
            res, lob->con, 
            
            OCILobGetLength(lob->con->cxt, lob->con->err, lob->handle, &size32)
        )

        size = (big_uint) size32;
    }

    OCI_RESULT(res);

    return size;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobCopy
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobCopy(OCI_Lob *lob, OCI_Lob *lob_src, big_uint offset_dst,
                            big_uint offset_src, big_uint count)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob,     FALSE);
    OCI_CHECK_PTR(OCI_IPC_LOB, lob_src, FALSE);

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
 
        OCI_CALL2
        (
            res, lob->con, 
            
            OCILobCopy2(lob->con->cxt, lob->con->err, lob->handle,
                       lob_src->handle, (ub8) count, 
                       (ub8) (offset_dst + 1), 
                       (ub8) (offset_src + 1))
        )
    }
    else

#endif
 
    {
       ub4 count32      = (ub4) count;
       ub4 offset_src32 = (ub4) offset_src;
       ub4 offset_dst32 = (ub4) offset_dst;


        OCI_CALL2
        (
            res, lob->con, 
            
            OCILobCopy(lob->con->cxt, lob->con->err, lob->handle,
                       lob_src->handle, count32, offset_dst32 + 1,
                       offset_src32 + 1)
         )
    }
    
    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobCopyFromFile
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobCopyFromFile(OCI_Lob *lob, OCI_File *file,
                                    big_uint offset_dst,
                                    big_uint offset_src,
                                    big_uint count)
{
    boolean res  = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob,   FALSE);
    OCI_CHECK_PTR(OCI_IPC_FILE, file, FALSE);

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        OCI_CALL2
        (
            res, lob->con, 
            
            OCILobLoadFromFile2(lob->con->cxt, lob->con->err,
                                lob->handle, file->handle,
                                (ub8) count,
                                (ub8) (offset_dst + 1),
                                (ub8) (offset_src + 1))
        )
    }
    else

#endif
 
    {
        ub4 count32      = (ub4) count;
        ub4 offset_src32 = (ub4) offset_src;
        ub4 offset_dst32 = (ub4) offset_dst;

        OCI_CALL2
        (
            res, lob->con, 
            
            OCILobLoadFromFile(lob->con->cxt, lob->con->err,
                               lob->handle, file->handle, count32,
                               offset_dst32 + 1, offset_src32 + 1)
        )
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobAppend
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_LobAppend(OCI_Lob *lob, void *buffer, unsigned int len)
{ 
    ub4 size_in  = 0;
    ub4 size_out = 0;
    ub2 csid     = 0;
    ub1 csfrm    = 0;
    void *obuf   = NULL;
    boolean res  = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, 0);
    OCI_CHECK_MIN(lob->con, NULL, len, 1, 0);

#ifndef OCI_CHARSET_ANSI
    csid = OCI_UTF16ID;
#endif

    /* OCILobWriteAppend() seems to cause problems on Oracle client 8.1 and 9.0 
       It's an Oracle known bug #886191
       So we use OCI_LobSeek() + OCI_LobWrite() instead */

    if (OCILib.ver_runtime < OCI_10)
    {
       return OCI_LobSeek(lob, OCI_LobGetLength(lob), OCI_SEEK_SET) &&
              OCI_LobWrite(lob, buffer, len);
    }        

    size_out = size_in = len;
    
    if (lob->type != OCI_BLOB)
    {
        size_in *= sizeof(dtext);
        obuf  = OCI_GetInputDataString(buffer, (int *) &size_in);
    }
    else
       obuf = buffer;

    if (lob->type == OCI_NCLOB)
        csfrm = SQLCS_NCHAR;
    else
        csfrm = SQLCS_IMPLICIT;

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 size_char = (ub8) len;
        ub8 size_byte = (ub8) size_in;
        
        OCI_CALL2
        (
            res, lob->con, 
            
            OCILobWriteAppend2(lob->con->cxt, lob->con->err, 
                               lob->handle, &size_byte,
                               &size_char, obuf, (ub8) size_in, 
                               (ub1) OCI_ONE_PIECE, 
                               (dvoid *) NULL,
                               NULL, csid, csfrm)
        )

        if (lob->type == OCI_BLOB)
            size_out = (ub4) size_byte;
        else
            size_out = (ub4) size_char;
    }

    else

#endif

    {
        OCI_CALL2
        (
            res, lob->con, 
            
            OCILobWriteAppend(lob->con->cxt, lob->con->err,
                              lob->handle, &size_out,
                              obuf, size_in, 
                              (ub1) OCI_ONE_PIECE,
                              (dvoid *) NULL,
                              NULL, csid, csfrm)
        )
    }

    if (lob->type != OCI_BLOB)
        OCI_ReleaseDataString(obuf);

    if (res == TRUE)
        lob->offset += (big_uint) size_out;

    OCI_RESULT(res);

    return size_out;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobAppendLob
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobAppendLob(OCI_Lob *lob, OCI_Lob *lob_src)
{
    boolean res  = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob,     FALSE);
    OCI_CHECK_PTR(OCI_IPC_LOB, lob_src, FALSE);

    /* 
       this might cause an ORA-24805 on Oracle 8.1.x only !
       I couldn’t find a bug ID on Metalink, but Oracle 9i had many fixes for
       lobs ! 
    */

    OCI_CALL2
    (
        res, lob->con, 
        
        OCILobAppend(lob->con->cxt, lob->con->err, lob->handle, lob_src->handle)
    )

    if (res == TRUE)
        lob->offset = OCI_LobGetLength(lob);

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobIsTemporary
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobIsTemporary(OCI_Lob *lob)
{
    boolean value = FALSE;
    boolean res   = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, FALSE);

    OCI_CALL2
    (
        res, lob->con, 
        
        OCILobIsTemporary(OCILib.env, lob->con->err, lob->handle, &value)
    )

    OCI_RESULT(res);

    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobOpen
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobOpen(OCI_Lob *lob, unsigned int mode)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, FALSE);

    OCI_CALL2
    (
        res, lob->con, 
        
        OCILobOpen(lob->con->cxt, lob->con->err, lob->handle, (ub1) mode)
    )

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobClose
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobClose(OCI_Lob *lob)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, FALSE);

    OCI_CALL2
    (
        res, lob->con, 
        
        OCILobClose(lob->con->cxt, lob->con->err, lob->handle)
    )
    
    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobIsEqual
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobIsEqual(OCI_Lob *lob, OCI_Lob *lob2)
{
    boolean value = FALSE;
    boolean res   = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob,FALSE);
    OCI_CHECK_PTR(OCI_IPC_LOB, lob2, FALSE);

    OCI_CALL2
    (
        res, lob->con, 
        
        OCILobIsEqual(OCILib.env, lob->handle, lob2->handle, &value)
    )

    OCI_RESULT(res);

    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobAssign
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobAssign(OCI_Lob *lob, OCI_Lob *lob_src)
{
    boolean res   = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob,     FALSE);
    OCI_CHECK_PTR(OCI_IPC_LOB, lob_src, FALSE);

    if (lob->hstate == OCI_OBJECT_ALLOCATED)
    {
        OCI_CALL2
        (
            res, lob->con, 
            
            OCILobLocatorAssign(lob->con->cxt, lob->con->err,
                                lob_src->handle, &lob->handle)
        )
  
    }
    else
    {
        OCI_CALL2
        (
            res, lob->con, 
            
            OCILobAssign(OCILib.env, lob->con->err,
                         lob_src->handle, &lob->handle)
        )
    }

    OCI_RESULT(res);

    return res;
}
