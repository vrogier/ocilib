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
    OCI_Coll       *cl = NULL;
    OCI_Elem       *el = NULL;
    OCI_Object     *ob = NULL;
    OCI_Number     *no = NULL;
    OCI_Number     *nt = NULL;
    OCI_Number    **ar = NULL;

    unsigned int i, n;
    otext buf[128] = "";

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    tc = OCI_TypeInfoGet(cn, "test_num_coll_t", OCI_TIF_TYPE);
    to = OCI_TypeInfoGet(cn, "test_num_t", OCI_TIF_TYPE);
    cl = OCI_CollCreate(tc);
    ob = OCI_ObjectCreate(cn, to);
    el = OCI_ElemCreate(tc);
    nm = OCI_NumberCreate(cn);
    ar = OCI_NumberArrayCreate(cn, 5);

    // Testing fetching numbers (as number and using implicit conversion number to string)
    OCI_Prepare(st, "select value from test_number");
    OCI_Execute(st);
    rs = OCI_GetResultset(st);
    while (OCI_FetchNext(rs))
    {
        OCI_Number  *n = OCI_GetNumber(rs, 1);
        const otext *s = OCI_GetString(rs, 1);

        OCI_NumberToText(n, NULL, sizeof(buf), buf);

        printf("%s - %s\n", buf, s);
    }

    // Testing binding external single number
    OCI_Prepare(st, "begin :1 := :1 *2 ; end;");
    OCI_BindNumber(st, ":1", nm);
    OCI_NumberFromText(nm, "1234.4321", NULL);
    OCI_Execute(st);
    OCI_NumberToText(nm, NULL, sizeof(buf), buf);
    printf("%s\n", buf);

    // Testing binding internal single number
    OCI_Prepare(st, "begin :1 := :1 *2 ; end;");
    OCI_SetBindAllocation(st, OCI_BAM_INTERNAL);
    OCI_BindNumber(st, ":1", NULL);
    nt = OCI_BindGetData(OCI_GetBind(st, 1));
    OCI_NumberFromText(nt, "1234.4321", NULL);
    OCI_Execute(st);
    OCI_NumberToText(nt, NULL, sizeof(buf), buf);
    printf("%s\n", buf);
    OCI_SetBindAllocation(st, OCI_BAM_EXTERNAL);

    // Testing registering number for a returning into statement
    OCI_Prepare(st, "update test_number set value  = value *2 returning value into :1");
    OCI_RegisterNumber(st, ":1");
    OCI_Execute(st);
    rs = OCI_GetResultset(st);
    while (OCI_FetchNext(rs))
    {
        printf("%s\n", OCI_GetString(rs, 1));
    }

    // testing Collections
    OCI_NumberFromText(nm, "1111.2222", NULL);
    OCI_ElemSetNumber(el, nm);
    OCI_CollAppend(cl, el);
    OCI_NumberFromText(nm, "3333.4444", NULL);
    OCI_ElemSetNumber(el, nm);
    OCI_CollAppend(cl, el);
    printf("%d\n", OCI_CollGetCount(cl));

    for (i = 1, n = OCI_CollGetCount(cl); i <= n; i++)
    {
        OCI_Elem   *e = OCI_CollGetElem(cl, i);
        OCI_Number *n = OCI_ElemGetNumber(e);
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
    OCI_Prepare(st, "SELECT utl_raw.cast_to_number('FF65'), utl_raw.cast_to_number('00') from dual");
    OCI_Execute(st);
    rs = OCI_GetResultset(st);
    while (OCI_FetchNext(rs))
    {
        printf("pos infinite = [%s]\n", OCI_GetString(rs, 1));
        printf("neg infinite = [%s]\n", OCI_GetString(rs, 2));
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
    OCI_ElemFree(el);
    OCI_CollFree(cl);
    OCI_ObjectFree(ob);

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
