#include "BaseComponent.h"

namespace EPRI
{
	BaseComponent::BaseComponent(IBaseLibrary * pLibrary)
	{
		Initialize(pLibrary);
	}
	
	void BaseComponent::Initialize(IBaseLibrary * pLibrary)
	{
		m_pBaseLibrary = pLibrary;
	}
	
	IBaseLibrary * BaseComponent::GetBase()
	{
		return m_pBaseLibrary;
	}
}