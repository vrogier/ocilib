---
author: admin
comments: false
date: 2013-03-26 23:54:19+00:00
layout: post
slug: working-on-ocilib-c-support
title: Working on OCILIB C++ support
wordpress_id: 757
categories:
- Code
- News
- Updates
---

Hello all,

I finally decided to work on a C++ wrapper around OCILIB !

It will be implemented as a single C++ header file that will offer C++ classes wrapping the OCILIB C API...

If the results sounds good, the next OCILIB release  will include this C++ header to simplify C++ developper's life :)

If any of you are interested, drop me a email !!

Here is an example of my early basic implementation :


    
    
    #include "ocilib++"
    
    using namespace ocilib;
    
    int main(int argc, char* argv[])
    {
        Environment::Initialize(Environment::EnvironmentDefault);
    
        try
        {
            Connection con("db", "usr", "pwd", Connection::SessionDefault);
            std::cout << con.getServerVersion() << std::endl;
    
            Statement st(con);
            st.execute("select table_name from user_tables");
    
            Resultset rs(st);
            while (rs.fetchNext())
            {
                std::cout << rs.getString(1) << std::endl;
            }
    
            Lob lob(con, Lob::LobTypeClob); 
            std::cout << "char written " << lob.write("test") << std::endl;
            lob.seek(Lob::LobSeekSet, 0);        
            std::cout << lob.read(4) << std::endl;
        }
        catch(Exception &ex;)
        {
             std::cout << ex.what() << std::endl;
        }
            
        Environment::Cleanup();
    }
    





