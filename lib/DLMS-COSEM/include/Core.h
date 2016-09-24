#pragma once

#include "ICore.h"

namespace EPRI
{
	class Core : public ICore
	{
	public:
		Core();
		virtual ~Core();

		ISerial * GetSerial();
		
	protected:
		Core(ISerial * pSerial);
		void SetSerial(ISerial * pSerial);
		
	private:
		ISerial * m_pSerial;

	};

}
