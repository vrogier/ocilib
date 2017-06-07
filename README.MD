# OCILIB - C and C++ Drivers for Oracle 
[![Build Status](https://img.shields.io/travis/vrogier/ocilib/master.svg)](https://travis-ci.org/vrogier/ocilib)
[![Code Analysis](https://img.shields.io/coverity/scan/8019.svg)](https://scan.coverity.com/projects/vrogier-ocilib)

## 1. About

OCILIB is an open source and cross platform Oracle Driver that delivers efficient access to Oracle databases.

The OCILIB library :

- offers a rich, full featured and easy to use API
- runs on all Oracle platforms
- is written in pure ISO C99 code with native ISO C Unicode support
- provides also a C++ API written isn standard C++ 3 with support for some C++11 features
- Enables high productivity
- encapsulates OCI (Oracle Call Interface)
- is the most complete available OCI wrapper

OCILIB is developed by Vincent Rogier. Get the latest package, install and enjoy it !

## 2. License

The source code is free source code licensed under **Apache License, Version 2.0** - see [LICENSE file](LICENSE)

## 3. Features 
 
- ISO C API
- ISO C ++ API
- Full Ansi and Unicode support on all platforms (ISO C wide strings or UTF8 strings)
- Full 32/64 bits compatibility
- Comptabile with all Oracle version >= 8i
- Automatic adaptation to the runtime Oracle client version
- Runtime loading of Oracle libraries
- Builtin error handling (global and thread context)
- Full support for SQL API and Object API
- Full support for ALL Oracle SQL and PL/SQL datatypes (scalars, objects, refs, collections, ..)
- Full support for PL/SQL (blocks, cursors, Index by Tables and Nested tables)
- Support for non scalar datatype with trough library objects
- Oracle Pooling (connections and sessions pools)
- Oracle XA connectivity (X/Open Distributed Transaction Processing XA interface)
- Oracle AQ (Advanded Queues)
- Oracle TAF (Transparent Application Failover) and HA (High availabality) support
- Binding array Interface
- Returning DML feature
- Scrollable statements
- Statement cache
- Direct Path loading
- Remote Instances Startup/Shutdown
- Oracle Database Change notification / Continuous Query Notification
- Oracle warnings support
- Global and local transactions
- Describe database schema objects
- Hash tables API
- Portable Threads and mutexes API

## 5. Installation

### Windows platforms

- unzip the archive
- copy ocilib\include\ocilib.h to any place located in your include’s path
- copy ocilib\lib32|64\ocilib[x].lib to any place located in your libraries path
- copy ocilib\lib32|64\ocilib[x].dll to any place located in your windows path

### GNU (Unix / Linux) platforms

- untar the archive
- $ cd ocilib-x.y.z
- $ ./configure
- $ make
- $ make install (you might need to `su` to make install)

Make sure Oracle and OCILIB libraries paths are defined in your shared library environment variable
You need to provide extra configure parameters when using Instant Clients – see Installation section)

## 6. Examples

Example of a minimal OCILIB C application

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
Example of a minimal OCILIB C++ application


```C++
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
    catch(std::exception &ex)
    {
         std::cout << ex.what() << std::endl;
    }
 
    Environment::Cleanup();
 
    return EXIT_SUCCESS;
}
```
