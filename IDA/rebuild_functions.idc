/* Ruben Santamarta www.reversemode.com */
/* Fix functions - Schneider NOE 110 firmware */
 
/* Edited by Charalambos Konstantinou Feb. 2015*/
 
/* FixARMfunctions.idc*/
 
#include <idc.idc>
 
static main() {
  auto ae;
  auto funct;
  auto minea;
  auto prolog;
  auto j;
  auto fun_array;
 
  minea = MinEA();
  SetStatus(IDA_STATUS_WORK);
  fun_array = CreateArray("ProGos");
 
  if (fun_array == -1) 
    fun_array = GetArrayId("ProGos");
 
  SetArrayString(fun_array,0,"2D E9"); 
 
  for (j=0;j<1;j++){
    ae = minea;
    prolog = GetArrayElement(AR_STR,fun_array,j);
    while (1) {
      funct = FindBinary ( ae, SEARCH_DOWN, prolog); 
      if (funct == BADADDR )
        break; 
      funct = funct - 2;
      MakeCode (funct);
      MakeFunction (funct, BADADDR); 
      ae = funct + 4;
   }
 }
 SetStatus(IDA_STATUS_READY); 
}
