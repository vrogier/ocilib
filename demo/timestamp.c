#include "ocilib.h"
 
#define SIZE_STR 260

int main(void)
{
    OCI_Timestamp *tm;
    OCI_Interval *itv;

    char str[SIZE_STR+1];
   
    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    tm  = OCI_TimestampCreate(NULL, OCI_TIMESTAMP);
    itv = OCI_IntervalCreate(NULL, OCI_INTERVAL_DS);

    OCI_TimestampSysTimeStamp(tm);
    OCI_TimestampToText(tm, "DD/MM/YYYY HH24:MI:SS:FF3", SIZE_STR, str, 3);
    printf("%s\n", str);

    OCI_IntervalSetDaySecond(itv, 1,1,1,1,0);
    OCI_IntervalToText(itv, 10, 10, SIZE_STR, str);
    printf("%s\n", str);
   
    OCI_TimestampIntervalAdd(tm, itv);
    OCI_TimestampToText(tm, "DD/MM/YYYY HH24:MI:SS:FF3", SIZE_STR, str, 3);
    printf("%s\n", str);

    OCI_Cleanup();
 
    return EXIT_SUCCESS;
}
