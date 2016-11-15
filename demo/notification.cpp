#include <iostream>

#include "ocilib.hpp"

using namespace ocilib;

#if defined(_WINDOWS)
    #define sleep(x) Sleep(x*1000)
#else
    #include <unistd.h>
#endif

#define WaitForEvents()   sleep(5)
#define WaitForDatabase() sleep(60)


static std::map<unsigned int, ostring> EventTypes;
static std::map<unsigned int, ostring> ObjectEvents;

void EventHandler(Event &evt);
void SetupNames();

int main(int argc, char* argv[])
{
    SetupNames();
    try
    {
        Environment::Initialize(Environment::Events);

        Connection con("db", "usr", "pwd");
        con.SetAutoCommit(true);

        Statement st(con);
        st.Execute("create table table1(code number)");
        st.Execute("create table table2(str varchar2(10))");

        Subscription sub;
        sub.Register(con, "sub-00", Subscription::AllChanges, EventHandler, 5468, 0);
        sub.Watch("select * from table1");
        sub.Watch("select * from table2");

        st.Execute("alter table table1 add price number");
        WaitForEvents();

        st.Execute("insert into table1 values(1, 10.5)");
        st.Execute("insert into table2 values('shoes')");
        WaitForEvents();

        st.Execute("update table1 set price = 13.5 where code = 1");
        st.Execute("delete from table2 ");
        WaitForEvents();

        st.Execute("drop table table1");
        st.Execute("drop table table2");
        WaitForEvents();

        con.Close();

        /* start remote instance */
        Environment::StartDatabase("db", "sys_usr", "sys_pwd", Environment::StartForce, Environment::StartFull);

        /* shutdown remote instance */
        Environment::ShutdownDatabase("db", "sys_usr", "sys_pwd", Environment::ShutdownAbort, Environment::ShutdownFull);
        WaitForDatabase();

        sub.Unregister();

    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    Environment::Cleanup();
}

void SetupNames()
{
    EventTypes[Event::DatabaseStart] = "Startup";
    EventTypes[Event::DatabaseShutdown] = "Shutdown";
    EventTypes[Event::DatabaseShutdownAny] = "Shutdown Any";
    EventTypes[Event::DatabaseDrop] = "Drop Database";
    EventTypes[Event::Unregister] = "Unregister";
    EventTypes[Event::ObjectChanged] = "Object Changed";

    ObjectEvents[Event::ObjectInserted] = "Insert";
    ObjectEvents[Event::ObjectUpdated] = "Update";
    ObjectEvents[Event::ObjectDeleted] = "Delete";
    ObjectEvents[Event::ObjectAltered] = "Alter";
    ObjectEvents[Event::ObjectDropped] = "Drop";
    ObjectEvents[Event::ObjectGeneric] = "Generic";
}

void EventHandler(Event &evt)
{
    std::cout << "** Notification : " << evt.GetSubscription().GetName() << std::endl;
    std::cout << "** Database     : " << evt.GetDatabaseName() << std::endl;
    std::cout << "** Event        : " << EventTypes[evt.GetType()] << std::endl;

    if (evt.GetType() == Event::ObjectChanged)
    {
        std::cout << ".... Object : " << evt.GetObjectName() << std::endl;
        std::cout << ".... Action : " << ObjectEvents[evt.GetObjectEvent()] << std::endl;
        std::cout << ".... RowID  : " << evt.GetRowID() << std::endl;
    }

    std::cout << std::endl;
}

