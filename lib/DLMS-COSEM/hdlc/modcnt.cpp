#include <cstdint>

#include "hdlc/modcnt.h"

namespace EPRI
{

	modcounter& modcounter::operator=(uint8_t cRightHandSide)
	{
		SetModCounter(cRightHandSide);
		return (*this);
	}
 
	modcounter& modcounter::operator=(modcounter &RightHandSide)
	{
		SetModCounter(RightHandSide.m_cCounter);
		return (*this);
	}

	modcounter modcounter::operator+(modcounter &Other)
	{
		modcounter	NewModCounter( m_cBase, 
			(uint8_t)(m_cCounter + Other.m_cCounter));
		return NewModCounter;
	}

	modcounter modcounter::operator+(uint8_t Other)
	{
		modcounter	NewModCounter( m_cBase, 
			(uint8_t)(m_cCounter + Other));
		return NewModCounter;
	}

	modcounter modcounter::operator+(int nOther)
	{
		modcounter	NewModCounter( m_cBase, 
			(uint8_t)(m_cCounter + (uint8_t) nOther));
		return NewModCounter;
	}

	modcounter modcounter::operator+=(modcounter &Other)
	{
		modcounter	NewModCounter( m_cBase, 
			(uint8_t)(m_cCounter + Other.m_cCounter));
		return NewModCounter;
	}

	modcounter modcounter::operator+=(uint8_t Other)
	{
		modcounter	NewModCounter( m_cBase, 
			(uint8_t)(m_cCounter + Other));
		return NewModCounter;
	}

	modcounter modcounter::operator-(const modcounter &Other)
	{
		modcounter	NewModCounter( m_cBase, 
			(uint8_t)(m_cCounter - Other.m_cCounter));
		return NewModCounter;
	}

	modcounter modcounter::operator-(uint8_t Other)
	{
		modcounter	NewModCounter( m_cBase, 
			(uint8_t)(m_cCounter - Other));
		return NewModCounter;
	}

	modcounter modcounter::operator-(int nOther)
	{
		modcounter	NewModCounter( m_cBase, 
			(uint8_t)(m_cCounter - (uint8_t) nOther));
		return NewModCounter;
	}

	modcounter modcounter::operator-=(modcounter &Other)
	{
		modcounter	NewModCounter( m_cBase, 
			(uint8_t)(m_cCounter - Other.m_cCounter));
		return NewModCounter;
	}

	modcounter modcounter::operator-=(uint8_t Other)
	{
		modcounter	NewModCounter( m_cBase, 
			(uint8_t)(m_cCounter - Other));
		return NewModCounter;
	}

	modcounter& modcounter::operator++()
	{
		SetModCounter((uint8_t)(m_cCounter + 1));
		return (*this);
	}

	modcounter modcounter::operator++(int)
	{
		modcounter temp = *this;
		++*this;
		return temp;
	}

	modcounter& modcounter::operator--()
	{
		SetModCounter((uint8_t)(m_cCounter - 1));
		return (*this);
	}

	modcounter modcounter::operator--(int)
	{
		modcounter temp = *this;
		--*this;
		return temp;
	}

	void modcounter::SetModCounter(uint8_t cCounter)
	{
		m_cCounter = (uint8_t)(cCounter % m_cBase);
	}

	modcounter::operator uint8_t() const
	{
		return ((uint8_t) m_cCounter);
	}

	modcounter::operator int() const
	{
		return ((int) m_cCounter);
	}

	modcounter::operator short() const
	{
		return ((short) m_cCounter);
	}

	short operator==(modcounter &Param1, modcounter &Param2)
	{
		return (Param1.m_cCounter == Param2.m_cCounter);
	}

	mod8counter::mod8counter()
		: modcounter(8)
	{
	}

	mod8counter::mod8counter(uint8_t cCounter)
		: modcounter(8, cCounter)
	{
	}

	modcounter& mod8counter::operator=(uint8_t RightHandSide)
	{
		return (modcounter::operator=(RightHandSide));
	}

	modcounter& mod8counter::operator=(mod8counter &RightHandSide)
	{
		return (modcounter::operator=(RightHandSide));
	}

}
