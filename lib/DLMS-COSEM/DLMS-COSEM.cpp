#include "DLMS-COSEM.h"

/*
	To test the library, include "DLMS-COSEM.h" from an application project
	and call DLMS-COSEMTest().
	
	Do not forget to add the library to Project Dependencies in Visual Studio.
*/

static int s_Test = 0;

extern "C" int DLMS_COSEMTest();

int DLMS_COSEMTest()
{
	return ++s_Test;
}