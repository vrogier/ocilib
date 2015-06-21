---
author: admin
comments: true
date: 2008-08-15 12:35:08+00:00
layout: post
slug: plsql-tables-in-parameters-bug-reported
title: PL/SQL tables IN parameters bug reported
wordpress_id: 57
categories:
- Code
tags:
- bind
- bug
- in
- pl/sql table
---

A bug was reported when binding host arrays to PL/SQL tables parameters of PL/SQL procedures.

This affects only pure IN parameter mode. When using OUT or IN/OUT, everything is working fine.

This bug gives an ORA-01403 when accessing the PL/SQL table parameter from inside the PL/SQL procedure.

This will be fixed in the next release (2.5.2 or 2.6.0).

You can patch the actual version (2.5.1) like this :

in ocilib.c, line 2451, was :

 
    
      
           pnbelem       = &bnd-;>plnbelem;
    



and it should be :


    
      
            bnd->plnbelem = nbelem;
            pnbelem       = &bnd-;>plnbelem;
    





