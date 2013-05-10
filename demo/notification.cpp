#include "ocilib.hpp"

using namespace ocilib;

#ifdef _WINDOWS
#define sleep(x) Sleep(x*1000)
#endif

#define WaitForEvents()   sleep(5)
#define WaitForDatabase() sleep(60)


static std::map<unsigned int, mstring> EventNames;
static std::map<unsigned int, mstring> ActionNames;

void EventHandler(Event &evt);
void SetupNames();

int main(int argc, char* argv[])
{
    SetupNames();
    try
    {
        Environment::Initialize(OCI_ENV_EVENTS);

        Connection con("xe", "usr", "pwd");
        con.SetAutoCommit(true);

        Statement st(con);      
        st.Execute("create table table1(code number)");
        st.Execute("create table table2(str varchar2(10))");

        Subscription sub;
        sub.Register(con, "sub-00", OCI_CNT_ALL, EventHandler, 5468, 0);
        sub.Watch("select * from table1");
        sub.Watch("select * from table2");

        st.Execute("alter table table1 add price number");
        WaitForEvents();

        st.Execute( "insert into table1 values(1, 10.5)");
        st.Execute("insert into table2 values('shoes')");
        WaitForEvents();

        st.Execute("update table1 set price = 13.5 where code = 1");
        st.Execute("delete from table2 ");
        WaitForEvents();

        st.Execute("drop table table1");
        st.Execute("drop table table2");
        WaitForEvents();

        con.Close();      

        Environment::ShutdownDatabase("xe", "sys", "sys", OCI_DB_SDF_IMMEDIATE, OCI_DB_SDM_FULL, OCI_SESSION_SYSDBA);
        WaitForDatabase();

        Environment::StartDatabase("xe", "sys", "sys", OCI_DB_SPF_FORCE, OCI_DB_SPM_FULL, OCI_SESSION_SYSDBA);
        WaitForDatabase();

        sub.Unregister();
        
    }
    catch(Exception &ex)
    {
         std::cout << ex.GetMessage() << std::endl;
    }

    Environment::Cleanup();
}

void SetupNames()
{
    EventNames[ OCI_ENT_STARTUP         ] = "Startup";
    EventNames[ OCI_ENT_SHUTDOWN        ] = "Shutdown";
    EventNames[ OCI_ENT_SHUTDOWN_ANY    ] = "Shutdown Any";
    EventNames[ OCI_ENT_DROP_DATABASE   ] = "Drop Database";
    EventNames[ OCI_ENT_DEREGISTER      ] = "Deregister";
    EventNames[ OCI_ENT_OBJECT_CHANGED  ] = "Object Changed";

    ActionNames[ OCI_ONT_INSERT         ] = "Insert";
    ActionNames[ OCI_ONT_UPDATE         ] = "Update";
    ActionNames[ OCI_ONT_DELETE         ] = "Delete";
    ActionNames[ OCI_ONT_ALTER          ] = "Alter";
    ActionNames[ OCI_ONT_DROP           ] = "Drop";
    ActionNames[ OCI_ONT_GENERIC        ] = "Generic";
}

void EventHandler(Event &evt)
{
    std::cout << "** Notification : " << evt.GetSubscription().GetName() << std::endl;
    std::cout << "** Database     : " << evt.GetDatabaseName()           << std::endl;
    std::cout << "** Event        : " << EventNames[evt.GetType()]       << std::endl;
    
    if (evt.GetType() == OCI_ENT_OBJECT_CHANGED)
    {
        std::cout << ".... Object : " << evt.GetObjectName()             << std::endl;
        std::cout << ".... Action : " << ActionNames[evt.GetOperation()] << std::endl;
        std::cout << ".... RowID  : " << evt.GetRowID()                  << std::endl;
    }

    std::cout << std::endl;
}

