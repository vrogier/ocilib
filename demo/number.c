#include "ocilib.h"

/* requires script demo/number.sql */

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Connection *cn = NULL;
    OCI_Statement  *st = NULL;
    OCI_Resultset  *rs = NULL;
    OCI_TypeInfo   *tc = NULL;
    OCI_TypeInfo   *to = NULL;
    OCI_Number     *nm = NULL;
    OCI_Collection       *cl = NULL;
    OCI_Element       *el = NULL;
    OCI_Object     *ob = NULL;
    OCI_Number     *no = NULL;
    OCI_Number     *nt = NULL;
    OCI_Number    **ar = NULL;

    OCI_UInt32 i, n;
    otext buf[128] = "";

    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    const char *db  = getenv("OCILIB_DEMO_DB");
    const char *usr = getenv("OCILIB_DEMO_USR");
    const char *pwd = getenv("OCILIB_DEMO_PWD");

    cn = OCI_ConnectionCreate(env, db, usr, pwd, OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    tc = OCI_TypeInfoGet(cn, "test_num_coll_t", OCI_TIF_TYPE);
    to = OCI_TypeInfoGet(cn, "test_num_t", OCI_TIF_TYPE);
    cl = OCI_CollectionCreate(tc);
    ob = OCI_ObjectCreate(to);
    el = OCI_ElementCreate(tc);
    nm = OCI_NumberCreate(env);
    ar = OCI_NumberArrayCreate(env, 5);

    // Testing fetching numbers (as number and using implicit conversion number to string)
    OCI_StatementPrepare(st, "select value from test_number");
    OCI_StatementExecutePrepared(st);
    rs = OCI_StatementGetResultset(st);
    while (OCI_ResultsetFetchNext(rs))
    {
        OCI_Number  *n = OCI_ResultsetGetNumber(rs, 0);
        const otext *s = OCI_ResultsetGetString(rs, 0);

        OCI_NumberToText(n, NULL, sizeof(buf), buf);

        printf("%s - %s\n", buf, s);
    }

    // Testing binding external single number
    OCI_StatementPrepare(st, "begin :1 := :1 *2 ; end;");
    OCI_StatementBindNumber(st, ":1", nm);
    OCI_NumberFromText(nm, "1234.4321", NULL);
    OCI_StatementExecutePrepared(st);
    OCI_NumberToText(nm, NULL, sizeof(buf), buf);
    printf("%s\n", buf);

    // Testing binding internal single number
    OCI_StatementPrepare(st, "begin :1 := :1 *2 ; end;");
    OCI_StatementSetBindAllocation(st, OCI_BAM_INTERNAL);
    OCI_StatementBindNumber(st, ":1", NULL);
    nt = OCI_BindGetData(OCI_StatementGetBind(st, 0));
    OCI_NumberFromText(nt, "1234.4321", NULL);
    OCI_StatementExecutePrepared(st);
    OCI_NumberToText(nt, NULL, sizeof(buf), buf);
    printf("%s\n", buf);
    OCI_StatementSetBindAllocation(st, OCI_BAM_EXTERNAL);

    // Testing registering number for a returning into statement
    OCI_StatementPrepare(st, "update test_number set value  = value *2 returning value into :1");
    OCI_StatementRegisterNumber(st, ":1");
    OCI_StatementExecutePrepared(st);
    rs = OCI_StatementGetResultset(st);
    while (OCI_ResultsetFetchNext(rs))
    {
        printf("%s\n", OCI_ResultsetGetString(rs, 0));
    }

    // testing Collections
    OCI_NumberFromText(nm, "1111.2222", NULL);
    OCI_ElementSetNumber(el, nm);
    OCI_CollectionAppend(cl, el);
    OCI_NumberFromText(nm, "3333.4444", NULL);
    OCI_ElementSetNumber(el, nm);
    OCI_CollectionAppend(cl, el);
    printf("%d\n", OCI_CollectionGetCount(cl));

    for (i = 0, n = OCI_CollectionGetCount(cl); i <= n; i++)
    {
        OCI_Element   *e = OCI_CollectionGetElement(cl, i);
        OCI_Number *n = OCI_ElementGetNumber(e);
        OCI_NumberToText(n, NULL, sizeof(buf), buf);
        printf("%s\n", buf);
    }

    // Testing objects
    OCI_NumberFromText(nm, "5555.6666", NULL);
    OCI_NumberToText(nm, NULL, sizeof(buf), buf);
    printf("%s\n", buf);
    OCI_ObjectSetNumber(ob, "value", nm);
    no = OCI_ObjectGetNumber(ob, "value");
    OCI_NumberToText(no, NULL, sizeof(buf), buf);
    printf("%s\n", buf);

    // Testing fetching infinite values to string
    OCI_StatementPrepare(st, "SELECT utl_raw.cast_to_number('FF65'), utl_raw.cast_to_number('00') from dual");
    OCI_StatementExecutePrepared(st);
    rs = OCI_StatementGetResultset(st);
    while (OCI_ResultsetFetchNext(rs))
    {
        printf("pos infinite = [%s]\n", OCI_ResultsetGetString(rs, 0));
        printf("neg infinite = [%s]\n", OCI_ResultsetGetString(rs, 1));
    }

    // Testing array of numbers
    OCI_NumberFromText(ar[0], "1.2", NULL);
    OCI_NumberFromText(ar[1], "~", NULL);
    OCI_NumberFromText(ar[2], "5.6", NULL);
    OCI_NumberFromText(ar[3], "-~", NULL);
    OCI_NumberFromText(ar[4], "9.2", NULL);

    OCI_NumberToText(ar[0], NULL, sizeof(buf), buf);
    printf("%s\n", buf);
    OCI_NumberToText(ar[1], NULL, sizeof(buf), buf);
    printf("%s\n", buf);
    OCI_NumberToText(ar[2], NULL, sizeof(buf), buf);
    printf("%s\n", buf);
    OCI_NumberToText(ar[3], NULL, sizeof(buf), buf);
    printf("%s\n", buf);
    OCI_NumberToText(ar[4], NULL, sizeof(buf), buf);
    printf("%s\n", buf);

    OCI_NumberArrayFree(ar);
    OCI_NumberFree(nm);
    OCI_ElementFree(el);
    OCI_CollectionFree(cl);
    OCI_ObjectFree(ob);

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
