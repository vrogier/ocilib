---
author: admin
comments: true
date: 2009-03-17 08:49:17+00:00
layout: post
slug: ocilib-320-available-soon
title: OCILIB 3.2.0 available soon !
wordpress_id: 281
categories:
- Code
- News
- Updates
tags:
- api
- direct
- loading
- path
- rebinding
---

OCILIB 3.2.0 will be available soon (beginning of April 09)... This release : 




  * adds support  for direct path interface


  * extends binding capabilities (rebinding, update of array size, ...)


  * extends objects API


  * adds Miscellaneous  changes and fixes



Here is an example of an full program that loads data into a table with direct path loading. 
It shows some possibilities of the direct path implementation :


    
    
    #include "ocilib.h"
    
    #define SIZE_TAB  1000
    #define SIZE_COL1 5
    #define SIZE_COL2 10
    #define SIZE_COL3 6
    #define NUM_COLS  3
    
    int main(void)
    {
        OCI_Connection *cn;
        OCI_DirPath    *dp;
    
        char arrval1[SIZE_TAB][SIZE_COL1+1];
        char arrval2[SIZE_TAB][SIZE_COL2+1];
        char arrval3[SIZE_TAB][SIZE_COL3+1];
    
        int arrsize1[SIZE_TAB];
        int arrsize2[SIZE_TAB];
        int arrsize3[SIZE_TAB];
    
        int i;
    
        if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
            return EXIT_FAILURE;
    
        cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
        dp = OCI_DirPathCreate(cn, "usr", "test_dp", NULL, NUM_COLS, SIZE_TAB);
    
        /* optionnal attributes to set */
    
        OCI_DirPathSetBufferSize(dp, 128000);
        OCI_DirPathEnableCache(dp, TRUE);
        OCI_DirPathSetCacheSize(dp, 1000);
        OCI_DirPathSetNoLog(dp, TRUE);
        OCI_DirPathSetParallel(dp, TRUE);
    
        /* describe the target table */
    
        OCI_DirPathSetColumn(dp, 1, "VAL1", SIZE_COL1, NULL    , 0, 0, FALSE);
        OCI_DirPathSetColumn(dp, 2, "VAL2", SIZE_COL2, NULL    , 0, 0, FALSE);
        OCI_DirPathSetColumn(dp, 3, "VAL3", SIZE_COL3, "DDMMYY", 0, 0, FALSE);
    
        OCI_DirPathPrepare(dp);
    
        /******* method 1 : set each array entry  ******/
    
        for (i = 0; i < SIZE_TAB; i++)
        {
            /* fill test values */
    
            sprintf(arrval1[i], "%d", i);
            sprintf(arrval2[i], "val %05d", i);
            sprintf(arrval3[i], "130309");
    
            OCI_DirPathSetEntry(dp, i, 1, arrval1[i], -1, TRUE);
            OCI_DirPathSetEntry(dp, i, 2, arrval2[i], (int) strlen(arrval2[i]), TRUE);
            OCI_DirPathSetEntry(dp, i, 3, arrval3[i], (int) SIZE_COL3, TRUE);
        }
    
        /* load data to the server */
    
        OCI_DirPathConvert(dp);
        OCI_DirPathLoad(dp);
    
        /* reset path stream */
    
        OCI_DirPathReset(dp);
    
        /****** method 2 : pass arrays in one call ******/
    
        for (i = 0; i < SIZE_TAB; i++)
        {
           /* fill test values */
    
            sprintf(arrval1[i], "%d", i);
            sprintf(arrval2[i], "val %05d", i);
            sprintf(arrval3[i], "130309");
    
            /* setup sizes */
    
            arrsize1[i] = -1;
            arrsize2[i] = (int) strlen(arrval2[i]);
            arrsize3[i] = SIZE_COL3;
    
        }
    
        OCI_DirPathSetArray(dp, 1, arrval1, arrsize1);
        OCI_DirPathSetArray(dp, 2, arrval2, arrsize2);
        OCI_DirPathSetArray(dp, 3, arrval3, arrsize3);
    
        /* load data to the server */
    
        OCI_DirPathConvert(dp);
        OCI_DirPathLoad(dp);
    
        /* commit data */
    
        OCI_DirPathFinish(dp);
    
        /* free direct path object */
    
        OCI_DirPathFree(dp);
    
        OCI_Cleanup();
    }
    
    



