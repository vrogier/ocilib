---
author: admin
comments: true
date: 2026-11-12 22:00:00+01:00
layout: page
slug: documentation
title: Documentation
---

* [Browse online HTML documentation]({{site.baseurl}}/doc/html/)


## Tutorials (upcoming v5.0.0)

* [A guided, dual-language (C/C++) tutorial series — ships with OCILIB packages]({{site.baseurl}}/doc/tutorials/html/)

## Sample Applications

### Minimal OCILIB C application  
 
{% highlight c linenos %}
    #include "ocilib.h"
    
    int main(int argc, char *argv[])
    {
        OCI_Environment* env;
        OCI_Connection* cn;
        OCI_Statement* st;
        OCI_Resultset* rs;
         
        env = OCI_EnvironmentCreate(NULL, NULL, OCI_ENV_DEFAULT);
     
        cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
        st = OCI_StatementCreate(cn);
       
        OCI_StatementExecute(st, "select intcol, strcol from table");
        
        rs = OCI_StatementGetResultset(st);
       
        while (OCI_ResultsetFetchNext(rs))
        {
            printf("%i - %s\n", OCI_ResultsetGetInt(rs, 1), OCI_ResultsetGetString(rs, 2));
        }
     
        OCI_StatementFree(st);
        OCI_ConnectionFree(cn);
        OCI_EnvironmentFree(env);
     
        return EXIT_SUCCESS;
    }
{% endhighlight %}
 
### Minimal OCILIB C++ application

{% highlight cpp linenos %}
    #include "ocilib.hpp"
    
    using namespace ocilib;
    
    int main(void)
    {
        try
        {
            Environment env;
    
            Connection con("db", "usr", "pwd");
            
            Statement st(con);
            st.Execute("select intcol, strcol from table");
    
            Resultset rs = st.GetResultset();
            while (rs.Next())
            {
                std::cout << rs.Get<int>(1) << " - " <<  rs.Get<ostring>(2) << std::endl;
            }
        }
        catch(std::exception &ex)
        {
             std::cout << ex.what() << std::endl;
        }
     
        return EXIT_SUCCESS;
    }
{% endhighlight %}
