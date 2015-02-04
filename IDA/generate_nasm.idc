/*
   Copyright (c) 2008 Chris Eagle (cseagle at gmail d0t c0m)
*/
// Edited by Charalambos Konstantinou, January 2015
/*script to dump the current (as defined by cursor location) function to the output window using mostly nasm compatible syntax. You could potentially modify to save output to a file and iterate over every function in your database.*/

#include <idc.idc>

static formatOperand(addr, num) {
   auto op, type, ptr, seg, var, idx, ch;
   seg = "";

   type = GetOpType(addr, num);
   op = GetOpnd(addr, num);

   if (op == "") {
      return op;
   }

   if ((ptr = strstr(op, " ptr ")) != -1) {
      op = substr(op, 0, ptr) + substr(op, ptr + 4, -1);
   }

   if ((ptr = strstr(op, "offset ")) == 0) {
      op = substr(op, ptr + 7, -1);
   }
   
   if ((ptr = strstr(op, "large ")) == 0) {
      op = substr(op, ptr + 6, -1);
   }
   
   if ((ptr = strstr(op, "ds:")) != -1) {
      op = substr(op, 0, ptr) + substr(op, ptr + 3, -1);
   }

   if ((ptr = strstr(op, ":")) != -1) {
      seg = substr(op, ptr - 2, ptr + 1); 
      op = substr(op, 0, ptr - 2) + substr(op, ptr + 1, -1);
   }
   
   if (type == 2) {
      if ((ptr = strstr(op, "[")) == -1) {
         op = "[" + op + "]";
      }
   }
   else if (type == 4) {
      ptr = strstr(op, "[");
      if (ptr > 0) {
         idx = ptr - 1;
         var = "";
         while (idx >= 0) {
            ch = substr(op, idx, idx + 1);
            if (ch == " ") break;
            var = ch + var;
            idx = idx - 1;
         }
         if (var != "") {
            op = substr(op, 0, idx + 1) + "[" + var + "+" + substr(op, ptr + 1, -1);
         }
      }
   }

   if (seg != "") {
      if ((ptr = strstr(op, "[")) != -1) {
         op = substr(op, 0, ptr + 1) + seg + substr(op, ptr + 1, -1);
      }
   }
   
   return op;
}

static main() {
   auto addr, funcEnd, funcStart, name, line;
   auto fname, op0, op1, op0Type, op1Type, ptr;
   auto func, mnem, oper0, oper1, disp;
   auto minEA, maxEA;
   
   minEA = MinEA();
   maxEA = MaxEA();
   
   funcStart = GetFunctionAttr(ScreenEA(), FUNCATTR_START);
   funcEnd = GetFunctionAttr(ScreenEA(), FUNCATTR_END);   
   
   fname = GetFunctionName(funcStart);
   
   Message("USE32\n\n");
   Message("section .text\n\n");
   Message("global _%s\n\n", fname);
   Message("_%s:\n", fname);
   
   for (addr = funcStart; addr != BADADDR; addr = NextHead(addr, funcEnd)) {
      if (addr != funcStart) {
         name = NameEx(addr, addr);
         if (name != "") {
            Message("%s:\n", name);
         }
      }
      
      op0 = 0;
      op1 = 0;
      
      op0Type = GetOpType(addr, 0);
      op1Type = GetOpType(addr, 1);
      
      if (op0Type == 3 || op0Type == 4) {
         disp = GetOperandValue(addr, 0);
         if (disp >= minEA && disp <= maxEA) {
         }
         else {
            op0 = OpNumber(addr, 0);
         }
      }
      if (op1Type == 3 || op1Type == 4) {
         disp = GetOperandValue(addr, 1);
         if (disp >= minEA && disp <= maxEA) {
         }
         else {
            op0 = OpNumber(addr, 1);
         }
      }
            
      line = GetDisasm(addr);
      mnem = GetMnem(addr);
      oper0 = formatOperand(addr, 0);
      oper1 = formatOperand(addr, 1);

      if (mnem == "call") {
         if (op0Type >= 5 && op0Type <= 7) { 
            if (strstr(oper0, "_") != 0) { 
               oper0 = "_" + oper0;
            }
         }
      }
      
      if (oper0 == "" && oper1 == "") {
         if (strstr(line, mnem) != 0) {
            mnem = line;
         }
         line = form("%-8s", mnem);
      }
      else if (oper1 == "") {
         line = form("%-7s %s", mnem, oper0);
      }
      else if (oper0 == "") {
         line = form("%-7s %s", mnem, oper1);
      }
      else {
         line = form("%-7s %s, %s", mnem, oper0, oper1);
      }

      Message("   %s\n", line);
      if (op0) {
         OpStkvar(addr, 0);
      }
      if (op1) {
         OpStkvar(addr, 1);
      }
   }
}
