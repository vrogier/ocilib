#include "ocilib.h"

#define SIZE_STR 260

int main(void)
{
    OCI_Date *d1, *d2;

    char str[SIZE_STR+1];
   
    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    d1 = OCI_DateCreate(NULL);
    d2 = OCI_DateCreate(NULL);

    strcpy(str, "13041978 20:20:12");

    OCI_DateFromText(d1, str, "DDMMYYYY HH24:MI:SS");
    OCI_DateToText(d1, "DD/MM/YYYY HH24:MI:SS", SIZE_STR, str);
    printf("\nDate is %s\n", str);

    OCI_DateSysDate(d1);
    OCI_DateToText(d1, "DD/MM/YYYY HH24:MI:SS", SIZE_STR, str);
    printf("\nSysdate is %s\n", str);

    OCI_DateAddDays(d1, 5);
    OCI_DateAddMonths(d1, 2);
    OCI_DateToText(d1, "DD/MM/YYYY HH24:MI:SS", SIZE_STR, str);
    printf("\nDate + 5 days and 2 months is %s\n", str);

    OCI_DateAssign(d2, d1);
    OCI_DateLastDay(d1);
    OCI_DateToText(d1, "DD/MM/YYYY HH24:MI:SS", SIZE_STR, str);
    printf("\nLast day of the month : %s\n", str);

    printf("\nNumber of days until the end of the months : %i\n",
            OCI_DateDaysBetween(d1, d2));

    OCI_DateFree(d1);
    OCI_DateFree(d2);

    OCI_Cleanup();
 
    return EXIT_SUCCESS;
}
