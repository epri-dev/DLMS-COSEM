#pragma once

#include "IBaseComponent.h"

namespace EPRI
{

	class BaseComponent : public IBaseComponent
	{
	public:
		BaseComponent() = delete;
		BaseComponent(IBaseLibrary * pLibrary);
		virtual void Initialize(IBaseLibrary * pLibrary);
		virtual IBaseLibrary * GetBase();

	protected:
		IBaseLibrary * m_pBaseLibrary;

	};

}
