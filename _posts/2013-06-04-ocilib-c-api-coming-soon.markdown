---
author: admin
comments: false
date: 2013-06-04 22:11:37+00:00
layout: post
slug: ocilib-c-api-coming-soon
title: OCILIB C++ API coming soon !
wordpress_id: 764
categories:
- Code
- News
- Updates
---

Hello all !

OCILIB C++ API is coming soon ! 
Thus OCILIB will become a **C and C++ driver for Oracle**
Code is already available from the OCILIB SVN repository. 
It is a single C++ header file that wraps the entire OCILIB C API .
It uses and exposes C++ objects and is based on the standard library and templates.
No usage of new or delete, just objects that automatically handle the lifetime of OCILIB handles :) Automatic and built-in object scope management !
I'm still working on the documentation
C main demo application and various C demos have been ported to C++.

Just have a look and try it :)

here is a basic example :


    
    
    #include "ocilib.hpp"
    
    using namespace ocilib;
    
    int main(void)
    {
        try
        {
            Environment::Initialize();
    
            Connection con("db", "usr", "pwd");
            
            Statement st(con);
            st.Execute("select * from products");
    
            Resultset rs = st.GetResultset();
            while (rs.Next())
            {
                std::cout << "code:" << rs.Get<int>(1) << " name: " <<  rs.Get<dstring>(2) << std::endl;
            }
     
            std::cout << "=> Total fetched rows : " << rs.GetCount() << std::endl;
    
        }
        catch(Exception &ex;)
        {
             std::cout << ex.GetMessage() << std::endl;
        }
    
        Environment::Cleanup();
     
        return EXIT_SUCCESS;
    }
    



