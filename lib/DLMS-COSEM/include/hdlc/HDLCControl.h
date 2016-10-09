#pragma once

#include <cstdint>

namespace EPRI
{

	class HDLCControl
	{
	public:
		enum Control : uint8_t
		{
			INFO    = 0x00,
			RR      = 0x01,
			RNR     = 0x05,
			SNRM    = 0x83,
			DISC    = 0x43,
			UA      = 0x63,
			DM      = 0x0F,
			FRMR    = 0x87,
			UI      = 0x03,
			UNKNOWN = 0xFF
		};
    	
    	enum PollFinal : uint8_t
    	{
        	NOT_SET  = 0b00000000,
        	SET      = 0b00010000
    	};

    	HDLCControl() :
        	m_Value(UNKNOWN)
		{ 
		}
    	
		HDLCControl(uint8_t Value) :
        	m_Value(Value)
		{
		}
    	
    	HDLCControl(Control Ctrl, PollFinal PF = SET, uint8_t NR = 0, uint8_t NS = 0)
    	{
        	switch (Ctrl)
        	{
    		case INFO:
            	m_Value = Ctrl | ((NR & 0b00000111) << 5) | (PF) | (NS << 1);
            	break;
			case RR:
            	m_Value = Ctrl | ((NR & 0b00000111) << 5) | (PF);
            	break;
			case RNR:
            	m_Value = Ctrl | ((NR & 0b00000111) << 5) | (PF);
            	break;
			case SNRM:
			case DISC:
			case UA:
			case DM:
			case FRMR:
			case UI:
            	m_Value = Ctrl | (PF);
            	break;
        	default:
            	m_Value = UNKNOWN;
            	break;
        	}
    	}
    	
		inline uint8_t NR() const
		{ 
			return (uint8_t)((m_Value & 0b11100000) >> 5); 
		}
    	
		inline uint8_t NS() const
		{ 
			return (uint8_t)((m_Value & 0b00001110) >> 1); 
		}
    	
		inline bool PF() const
		{ 
			return (bool)(m_Value & 0b00010000); 
		}

		inline bool IsResponse(void) const
		{
			switch (PacketType())
			{
			case INFO:
			case RR:
			case RNR:
			case UA:
			case DM:
			case FRMR:
			case UI:
				return (true); 
			default:
				return (false); 
			}
		}

		inline Control PacketType() const
		{
			if (0x00 == (m_Value & 0x01))
			{
				return INFO;
			}
			switch (m_Value & 0x0F)
			{
			case RR:
				return RR;
			case RNR:
				return RNR;
			default:
				switch (m_Value & 0b11101111)
				{
				case SNRM:
				case DISC:
				case UA:
				case DM:
				case FRMR:
				case UI:
					return Control(m_Value & 0b11101111);
				}
			}
			return UNKNOWN;
		}

		inline uint8_t * operator &()
		{
			return (&m_Value);
		}
    	
    	inline operator uint8_t() const
    	{
        	return m_Value;
    	}
    	
    	const char * ToString() const
    	{
        	switch (PacketType())
        	{
        	case INFO:
            	return "INFO";
        	case RR:
            	return "RR";
        	case RNR:
            	return "RNR";
        	case SNRM:
            	return "SNRM";
        	case DISC:
            	return "DISC";
        	case UA:
            	return "UA";
        	case DM:
            	return "DM";
        	case FRMR:
            	return "FRMR";
        	case UI:
            	return "UI";
        	default:
            	return "UNKNOWN";
        	}
    	}

	private:
		uint8_t	  m_Value;

	};

}