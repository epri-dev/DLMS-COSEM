#include <cstring>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <type_traits>

#include "DLMSVector.h"

namespace EPRI
{
    bool IsValueInVariant(const DLMSVariant& Variant, const DLMSVariant& Value)
    {
        return false;
    }
    
    DLMSVector::DLMSVector()
    {
    }

    DLMSVector::DLMSVector(size_t Size)
    {
        m_Data.resize(Size);
    }
    
    DLMSVector::DLMSVector(const std::initializer_list<uint8_t>& Value)
    {
        m_Data = Value;
    }

    DLMSVector::DLMSVector(const DLMSVector& Value)
    {
        m_Data = Value.m_Data;
    }

    DLMSVector::~DLMSVector()
    {
    }

    size_t DLMSVector::Size() const
    {
        return m_Data.size();
    }

    size_t DLMSVector::GetReadPosition() const
    {
        return m_ReadPosition;
    }

    bool DLMSVector::SetReadPosition(size_t Value)
    {
        if (Value >= m_Data.size())
            return false;
        m_ReadPosition = Value;
        return true;
    }

    bool DLMSVector::Zero(size_t Position /* = 0 */, size_t Count /* = 0 */)
    {
        if (0 == Count)
        {
            Count = m_Data.size();
        }
        if (Position + Count > m_Data.size())
        {
            return false;
        }
        std::memset(m_Data.data() + Position, '\0', Count);
        return true;
    }

    size_t DLMSVector::AppendFloat(float Value)
    {
        size_t RetVal = m_Data.size();
        union
        {
            float   Float;
            uint8_t Bytes[sizeof(float)];
        } Convert;
        Convert.Float = Value;
        m_Data.push_back(Convert.Bytes[3]);
        m_Data.push_back(Convert.Bytes[2]);
        m_Data.push_back(Convert.Bytes[1]);
        m_Data.push_back(Convert.Bytes[0]);
        return RetVal;
    }

    size_t DLMSVector::AppendDouble(double Value)
    {
        size_t RetVal = m_Data.size();
        union
        {
            double  Double;
            uint8_t Bytes[sizeof(double)];
        } Convert;
        Convert.Double = Value;
        m_Data.push_back(Convert.Bytes[7]);
        m_Data.push_back(Convert.Bytes[6]);
        m_Data.push_back(Convert.Bytes[5]);
        m_Data.push_back(Convert.Bytes[4]);
        m_Data.push_back(Convert.Bytes[3]);
        m_Data.push_back(Convert.Bytes[2]);
        m_Data.push_back(Convert.Bytes[1]);
        m_Data.push_back(Convert.Bytes[0]);
        return RetVal;
    }

    size_t DLMSVector::AppendBuffer(const void * pValue, size_t Count)
    {
        size_t RetVal = m_Data.size();
        const uint8_t * p = static_cast<const uint8_t *>(pValue);
        m_Data.insert(m_Data.end(), p, p + Count);
        return RetVal;
    }

    ssize_t DLMSVector::Append(const DLMSVector& Value, size_t Position /* = 0 */, size_t Count /* = 0 */)
    {
        ssize_t RetVal = m_Data.size();
        if (0 == Count)
        {
            Count = Value.Size();
        }
        if (Position + Count > Value.Size())
            return -1;
        AppendBuffer(Value.m_Data.data() + Position, Count);
        return RetVal;
    }

    size_t DLMSVector::Append(const std::string& Value)
    {
        size_t RetVal = m_Data.size();
        m_Data.insert(m_Data.end(), Value.begin(), Value.end());
        return RetVal;
    }
    
    size_t DLMSVector::Append(const std::vector<uint8_t>& Value)
    {
        size_t RetVal = m_Data.size();
        m_Data.insert(m_Data.end(), Value.begin(), Value.end());
        return RetVal;
    }
    
    class AppendVisitor : public static_visitor<>
    {
    public:
        AppendVisitor() = delete;
        AppendVisitor(DLMSVector * pVector, bool Trim = true)
            : m_pVector(pVector)
            , m_Trim(Trim)
        {
        }
        
        void operator()(const blank& Value)
        {
        }
        void operator()(const bool& Value)
        {
            m_pVector->Append<uint8_t>(Value ? 1 : 0);
        }
        void operator()(const int32_t& Value)
        {
            if (!m_Trim)
            {
                m_pVector->Append(Value);
            }
            else
            {
                AppendTrimmedInteger(Value);
            }
        }
        void operator()(const uint32_t& Value)
        {
            if (!m_Trim)
            {
                m_pVector->Append(Value);
            }
            else
            {
                AppendTrimmedInteger(Value);
            }
        }
        void operator()(const int64_t& Value)
        {
            if (!m_Trim)
            {
                m_pVector->Append(Value);
            }
            else
            {
                AppendTrimmedInteger(Value);
            }
        }
        void operator()(const uint64_t& Value)
        {
            if (!m_Trim)
            {
                m_pVector->Append(Value);
            }
            else
            {
                AppendTrimmedInteger(Value);
            }
        }
        void operator()(const std::string& Value)
        {
        }
        void operator()(const double& Value)
        {
        }
        void operator()(const DLMSVector& Value)
        {
        }
        void operator()(const std::initializer_list<uint32_t>& Value)
        {
        }
        
    protected:
        void AppendTrimmedInteger(uint64_t Value)
        {
            DLMSVector Vector;
            int        VectorIndex = 0;
            Vector.Append(Value);
            for (; VectorIndex < sizeof(uint64_t); VectorIndex += 2)
            {
                if (Vector[VectorIndex] ||
                    Vector[VectorIndex + 1])
                {
                    break;
                }
            }
            if (6 == VectorIndex && Vector[VectorIndex] == 0x00)
            {
                ++VectorIndex;
            }
            if (VectorIndex == sizeof(uint64_t))
            {
                m_pVector->Append<uint8_t>(0x00);
            }
            else
            {
                m_pVector->Append(Vector, VectorIndex, sizeof(uint64_t) - VectorIndex);        
            }
        }
        
        DLMSVector * m_pVector;
        bool         m_Trim;
    };
        
    size_t DLMSVector::Append(const DLMSVariant& Value, bool Trim /* = true*/)
    {
        size_t RetVal = m_Data.size();
        AppendVisitor Visitor(this, Trim);
        apply_visitor(Visitor, Value);
        return RetVal;
    }

    void DLMSVector::Clear()
    {
        m_Data.clear();
    }
    
//    bool DLMSVector::GetFloat(DLMSVariant * pValue)
//    {
//        bool RetVal = PeekFloat(pValue);
//        if (RetVal)
//        {
//            m_ReadPosition += sizeof(float);
//        }
//        return RetVal;
//    }
//    
//    bool DLMSVector::GetDouble(DLMSVariant * pValue)
//    {
//        bool RetVal = PeekDouble(pValue);
//        if (RetVal)
//        {
//            m_ReadPosition += sizeof(double);
//        }
//        return RetVal;
//    }

    bool DLMSVector::GetBuffer(uint8_t * pValue, size_t Count)
    {
        if (m_ReadPosition + Count <= m_Data.size())
        {
            std::memcpy(pValue, &m_Data.data()[m_ReadPosition], Count);
            m_ReadPosition += Count;
            return true;
        }
        return false;
    }
    
    bool DLMSVector::GetVector(std::vector<uint8_t> * pValue, size_t Count)
    {
        if (m_ReadPosition + Count <= m_Data.size())
        {
            pValue->insert(pValue->end(), m_Data.begin(), m_Data.begin() + Count);
            m_ReadPosition += Count;
            return true;
        }
        return false;
    }

    std::vector<uint8_t> DLMSVector::GetBytes() const
    {
        return m_Data;
    }
    
    int DLMSVector::PeekByte(size_t OffsetFromGetPosition /* = 0 */) const
    {
        if (m_ReadPosition + OffsetFromGetPosition < m_Data.size())
        {
            return m_Data[m_ReadPosition + OffsetFromGetPosition];
        }
        return -1;
    }
    
    bool DLMSVector::PeekBuffer(uint8_t * pValue, size_t Count) const
    {
        if (m_ReadPosition + Count < m_Data.size())
        {
            std::memcpy(pValue, &m_Data[m_ReadPosition], Count);
            return true;
        }
        return false;
    }
    
//    bool DLMSVector::PeekFloat(DLMSVariant * pValue) const
//    {
//        if (m_ReadPosition + sizeof(float) < m_Data.size())
//        {
//            union
//            {
//                float   Float;
//                uint8_t Bytes[sizeof(float)];
//            } Convert;
//            if (PeekBuffer(Convert.Bytes, sizeof(Convert.Bytes)))
//            {
//                pValue->set<float>(Convert.Float);
//                return true;
//            }
//        }
//        return false;
//    }
//    
//    bool DLMSVector::PeekDouble(DLMSVariant * pValue) const 
//    {
//        if (m_ReadPosition + sizeof(double) < m_Data.size())
//        {
//            union
//            {
//                double   Double;
//                uint8_t  Bytes[sizeof(double)];
//            } Convert;
//            if (PeekBuffer(Convert.Bytes, sizeof(Convert.Bytes)))
//            {
//                pValue->set<double>(Convert.Double);
//                return true;
//            }
//        }
//        return false;
//    }
    
    std::string DLMSVector::ToString()
    {
        std::ostringstream Output;
        std::for_each(m_Data.begin(), m_Data.end(),
            [&Output](uint8_t Value)
            {
                Output << std::setw(2) << Value << ' ';
            });
        return Output.str();
    }

    DLMSVector& DLMSVector::operator=(DLMSVector& lval)
    {
        m_Data = lval.m_Data;
        return *this;
    }
    
    uint8_t& DLMSVector::operator[](size_t Index)
    {
        return m_Data[Index];
    }

    const uint8_t& DLMSVector::operator[](size_t Index) const
    {
        return m_Data[Index];
    }
   
}