// Zaachry Basnight ControlLogic 1756 - L61 FindARMFunctions.idc
// harryskon
#include <idc.idc>

static main()
{
	 auto ea;
	 auto eaFunc;
	 auto str;
	 auto i;
	 auto name;
	 auto len;
	 auto end;
	 auto start;
	 auto ch;
	 auto refAddr;
	 auto funcName;
	 auto suffix;
	 auto NamesArray;
	 auto arrayLen;
	 auto idx;

	 ea = MinEA();
	 SetStatus(IDA_STATUS_WORK);

	 str = "2E 2E 5C"; // "..\" 
	 NamesArray = CreateArray("Names");
	 arrayLen = 0 ;
	
	 while (1) { //search entire binary
		ea = FindBinary (ea, SEARCH_DOWN, str); // search for magic string
	 	if (ea == BADADDR) 
			break; // break if error or past the end
		name = Name (ea); //get the IDA database name of the current location
		len = 1;

		if (name != "") { //if the location is not already named
				Message("%s\n", name);
				name = GetString(ea, -1, ASCSTR_C); // get binary as a string and determine its length
				len = strlen(name);
				end = len;

			do { //working backwards, remove the file extension from "name"
				end = end -1;
				ch = substr(name, end, end+1);
			} while (ch != ".");
		
			start = end;
			do { // still working backwards, find the start of the filename (not the path)
				start = start - 1;
				ch = substr(name, start, start+1);
			} while (ch != "\\");
			start = start + 1;
			name = substr (name, start, end); // "name" is now just the filename sans extension
			Message("%s\n", name);

			// search for name in array of names already found/used
			idx = arrayLen;
			suffix = -1;
			for (i=0; i<arrayLen; i++){
				if ( name == GetArrayElement(AR_STR, NamesArray, i*2)) {
					suffix = GetArrayElement(AR_LONG, NamesArray, (i*2)+1);
					idx = i*2;
					break;
				}
			}
		
			// if not found, add it
			if ( idx == arrayLen) {
				SetArrayString (NamesArray, idx, name);
				SetArrayLong (NamesArray, idx+1, 0);
				arrayLen = arrayLen + 2;
			}

			refAddr = DfirstB(ea); // get the first address that references the filename string
			//Message("%x\n", refAddr);

			// ofr every cross reference address, name that address as the filename 
			while (refAddr != BADADDR) {
				funcName = GetFunctionName(refAddr);
				if (funcName != "") { //if the address is a named function ..
					if (strstr(funcName, "sub_") == 0) { // that starts with "sub_" (autoname)..
						suffix = suffix + 1;
						eaFunc = LocByName (funcName); //rename func and add the appropriate suffix
						MakeNameEx (eaFunc, sprintf("%s_%d", name,suffix), SN_NOCHECK & SN_NON_AUTO & SN_NOWARN);
					}
				}
				refAddr = DnextB(ea,refAddr);
				//Message("%x\n",refAddr);
			}

			SetArrayLong(NamesArray, idx+1, suffix);
		}
		ea = ea + len; //increment past the string and keep going
	}
	Message ("Done\n");
	SetStatus(IDA_STATUS_READY);
}	
