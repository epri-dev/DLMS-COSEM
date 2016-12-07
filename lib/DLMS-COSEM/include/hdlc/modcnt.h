#pragma once

#include <cstdint>

namespace EPRI
{
	class modcounter
	{
	public:
		modcounter(uint8_t cBase)
		{
			m_cBase = cBase;
			m_cCounter = (uint8_t)(m_cBase - 1);
		}

		modcounter(uint8_t cBase, uint8_t cCounter)
		{
			m_cBase = cBase;
			m_cCounter = (uint8_t)(cCounter % m_cBase);
		}

		modcounter(modcounter &Source)
		{
			m_cBase = Source.m_cBase;
			m_cCounter = Source.m_cCounter;
		}

		modcounter& operator=(uint8_t RightHandSide);
		modcounter& operator=(modcounter &RightHandSide);
		modcounter operator+(modcounter &Other);
		modcounter operator+(uint8_t Other);
		modcounter operator+(int Other);
		modcounter operator+=(modcounter &Other);
		modcounter operator+=(uint8_t Other);
		modcounter operator-(const modcounter &Other);
		modcounter operator-(uint8_t Other);
		modcounter operator-(int Other);
		modcounter operator-=(modcounter &Other);
		modcounter operator-=(uint8_t Other);
		modcounter& operator++();
		modcounter operator++(int);
		modcounter& operator--();
		modcounter operator--(int);
		operator uint8_t() const;
		operator int() const;
		operator short() const;

		friend short operator==(modcounter& Param1, modcounter& Param2);
	
	protected:
		void SetModCounter(uint8_t cCounter);

		uint8_t	m_cBase;
		uint8_t	m_cCounter;
	};

	short operator==(modcounter& Param1, modcounter& Param2);

	class mod8counter : public modcounter
	{
	public:
		mod8counter();
		mod8counter(uint8_t cCounter);

		modcounter& operator=(uint8_t RightHandSide);
		modcounter& operator=(mod8counter &RightHandSide);

	};	
    
    template <uint8_t Base>
        class ModCounter : public modcounter
        {
        public:
            ModCounter()
                : modcounter(Base)
            {
            }
            ModCounter(uint8_t InitialValue)
                : modcounter(Base, InitialValue)
            {
            }
        };

} /* namespace EPRI */