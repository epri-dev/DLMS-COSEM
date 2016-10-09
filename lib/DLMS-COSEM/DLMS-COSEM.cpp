#include "DLMS-COSEM.h"
#include "IBaseLibrary.h"

namespace EPRI
{

	static IBaseLibrary * g_pBaseLibrary;

	IBaseLibrary * Base()
	{
		return g_pBaseLibrary;
	}
	
	void SetBase(IBaseLibrary * pBase)
	{
		// Only allow setting once
		//
		if (!g_pBaseLibrary)
		{
			g_pBaseLibrary = pBase;
		}
	}
	
}