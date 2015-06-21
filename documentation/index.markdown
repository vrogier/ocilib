---
author: admin
comments: true
date: 2008-08-16 20:56:57+00:00
layout: page
slug: documentation
title: Documentation
wordpress_id: 65
---

* [Browse online HTML documentation]({{site.baseurl}}/documentation/html/)


### Example of a minimal OCILIB C application  
 
```C
    #include "ocilib.h"
    
    int main(int argc, char *argv[])
    {
        OCI_Connection* cn;
        OCI_Statement* st;
        OCI_Resultset* rs;
         
        OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT);
     
        cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
        st = OCI_StatementCreate(cn);
       
        OCI_ExecuteStmt(st, "select intcol, strcol from table");
        
        rs = OCI_GetResultset(st);
       
        while (OCI_FetchNext(rs))
        {
            printf("%i - %s\n", OCI_GetInt(rs, 1), OCI_GetString(rs,2));
        }
     
        OCI_Cleanup();
     
        return EXIT_SUCCESS;
    }
```
 
### Example of a minimal OCILIB C++ application

```C
    #include "ocilib.hpp"
    
    using namespace ocilib;
    
    int main(void)
    {
        try
        {
            Environment::Initialize();
    
            Connection con("db", "usr", "pwd");
            
            Statement st(con);
            st.Execute("select intcol, strcol from table");
    
            Resultset rs = st.GetResultset();
            while (rs.Next())
            {
                std::cout << rs.Get<int>(1) << " - " <<  rs.Get<ostring>(2) << std::endl;
            }
        }
        catch(std::exception &ex;)
        {
             std::cout << ex.what() << std::endl;
        }
    
        Environment::Cleanup();
     
        return EXIT_SUCCESS;
    }
```