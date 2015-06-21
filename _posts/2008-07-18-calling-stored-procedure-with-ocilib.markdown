---
author: admin
comments: true
date: 2008-07-18 20:17:18+00:00
layout: post
slug: calling-stored-procedure-with-ocilib
title: Calling stored procedure with OCILIB
wordpress_id: 13
categories:
- Code
tags:
- Code
- OCILIB
- pl/sql
- stored procedure
---

Today, I was asked on the OCILIB SourceForge forum if OCILIB could handle calls to stored procedures...

Of course, it can ! Here is a basic simple example.

Let's say you've got the following stored procedure :

    
    
    my_function(code in number, str out varchar) return number;
    



Here is a full working C program source code to call this procedure:


    
    
    int main(int argc, char **argv)
    {
        int code, res;
        char str[31];
        OCI_Connection *cn;
        OCI_Statement *st;
    
        OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT);
    
        cn = OCI_ConnectionCreate("db", "user", "pwd", OCI_SESSION_DEFAULT);
        st = OCI_StatementCreate(cn);
    
        OCI_Prepare(st, "begin :res := my_function(:code, :str); end;");
        OCI_BindInt(st, ":res", &res;);
        OCI_BindInt(st, ":code", &code;);
        OCI_BindString(st, ":str", str, 30);
    
        code = 5;
        res = 0;
        str[0] = 0;
    
        OCI_Execute(st);
    
        printf("res = %d, str = %s", res, str);
    
        OCI_Cleanup();
    
        return EXIT_SUCCESS;
    }
    



That's all ! Calling stored procedures is really easy with OCILIB
