---
author: admin
comments: true
date: 2008-08-06 07:32:58+00:00
layout: post
slug: calling-plsql-procedures-with-plsql-tables-parameters
title: Calling PL/SQL procedures with PL/SQL tables parameters
wordpress_id: 43
categories:
- Code
tags:
- pl/sql bind procedure array
---

Since version 2.5.0, OCILIB handles PL/SQL tables for input/output binds when calling PL/SQL procedures.

Because PL/SQL tables are arrays, binding C arrays to a PL/SQL call is implemented in OCILIB though the OCI_BindArrayXXXX() functions.

Thoses functions were initially designed for the array interface (bulk operations). In this context, all arrays binded to a statement must have the same size. This size is set by calling OCI_BindArraySetSize().

But in the context of PL/SQL tables binds, each bind (each array) is independent and can have it own size.

That why, in 2.5.0, i introduced an extra params to all of the OCI_BindArrayXXX() calls in order to specify the size of the array for PL/SQL tables binds.

For regular bulk operations, this extra param is not used and must be set to 0.

Because PL/SQL tables binds have their own array size specified with the new last param ('nbelem') of OCI_BindArrayXXX() call, OCI_BindArraySetSize() MUST not be called.

A little summary:

For bulk operations :



	
  * prepare statement with OCI_Prepare()

	
  * set array size with OCI_BindArraySetSize()

	
  * bind C arrays with OCI_BindArrayXXX() with 0 for param 'nbelem'

	
  * execute statement with OCI_Execute()


For PL/SQL procedure call with PL/SQ/ tables binds :

	
  * prepare statement with OCI_Prepare()

	
  * bind C arrays with OCI_BindArrayXXX() specifying each array size with param 'nbelem'

	
  * execute statement with OCI_Execute()


The OCILIB documentation and changelog were not clear about that point.

Next release will improve the documentation.
