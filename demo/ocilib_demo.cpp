#include <iostream>
#include <iomanip>
#include <string>
#include <time.h>
#include <sstream>
#include <vector>

#include "ocilib.hpp"

#ifdef _MSC_VER

#if defined(OCI_CHARSET_WIDE)
#pragma comment(lib, "ocilibw.lib")
#elif defined(OCI_CHARSET_ANSI)
#pragma comment(lib, "ociliba.lib")
#endif

#endif

using namespace ocilib;
using namespace std;

int main(int argc, char**argv)
{
    try
    {
        Environment::Initialize(Environment::Default);

        Connection con("db12c", "usr", "pwd");

        Statement st(con);

        st.Execute("select * from test_date");
        Resultset rs = st.GetResultset();
        rs.Next();
        Date date = rs.Get<Date>(1);

        std::cout << date;

        std::cout << date.IsNull();


        /*
        Collection<Object> coll(TypeInfo(con, "tatas", TypeInfo::Type));

        for (int i = 1; i <= 10; i++)
        {
        Object toto(TypeInfo(con, "toto", TypeInfo::Type));
        Object tata(TypeInfo(con, "tata", TypeInfo::Type));

        toto.Set<int>("code", i);
        toto.Set<ostring>("value", "value");

        tata.Set<int>("code", i);
        tata.Set<Object>("value", toto);

        coll.Append(tata);
        }
        */

        

    }
    catch (Exception & ex)
    {
        cerr << "OciLib::Exception" << endl;
        cerr << "Message     : " << ex.GetMessage() << endl;
        cerr << "Oracle error: " << ex.GetOracleErrorCode() << endl;
        cerr << "what        : " << ex.what() << endl;
        cerr << "Internal err: " << ex.GetInternalErrorCode() << endl;
    }
    catch (exception &ex)
    {
        cerr << "EXCEPTION:" << endl;
        cerr << ex.what() << endl;
    }

    Environment::Cleanup();

    return EXIT_SUCCESS;
}