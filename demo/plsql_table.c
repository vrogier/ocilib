#include "ocilib.h"

#define SIZE_ARRAY 10
#define SIZE_NAME  20
#define SIZE_VALUE 100

int main(void)
{
   OCI_Connection *cn;
   OCI_Statement *st;
   int i;

   char tab_names [SIZE_ARRAY][SIZE_NAME  + 1];
   char tab_values[SIZE_ARRAY][SIZE_VALUE + 1];

   if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
       return EXIT_FAILURE;

   cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
   st = OCI_StatementCreate(cn);

   /* set values */

   for (i = 0; i < SIZE_ARRAY; i++)
   {
       sprintf(tab_names[i],  "name  %03d", i+1); 
       sprintf(tab_values[i], "value %03d", i+1); 
   }

   /* prepare call and bind local arrays */

   OCI_Prepare(st, "BEGIN test.test(:tab_names, :tab_values); END;");
   OCI_BindArrayOfStrings(st, ":tab_names",  (char*) tab_names,  SIZE_NAME , SIZE_ARRAY);
   OCI_BindArrayOfStrings(st, ":tab_values", (char*) tab_values, SIZE_VALUE, SIZE_ARRAY);

   /* execute */

   OCI_Execute(st);
  
   /* cleanup */
   OCI_Cleanup();

   return EXIT_SUCCESS;
}