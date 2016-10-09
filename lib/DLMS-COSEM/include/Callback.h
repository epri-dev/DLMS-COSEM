#pragma once

#include <functional>
#include <map>

namespace EPRI
{
    class BaseCallbackParameter
    {
    public:
        virtual ~BaseCallbackParameter() 
        {
        }
    };

    template<typename TRetValue, typename TKeyValue, typename TParameter = BaseCallbackParameter>
    class Callback
    {
    public:
        Callback() 
        {
        }
        virtual ~Callback()
        {
        }
        
        typedef std::function<TRetValue(const TParameter&)> CallbackFunction;
        virtual void RegisterCallback(TKeyValue Key, 
            CallbackFunction Handler)
        {
            m_Handlers.insert(CallbackHandlerPair(Key, Handler));
        }
        
        virtual bool FireCallback(TKeyValue Key, const TParameter& Params, TRetValue * pRetVal)
        {
            if (pRetVal && m_Handlers.find(Key) != m_Handlers.end())
            {
                *pRetVal = m_Handlers[Key](Params);
                return true;
            }
            return false;
        }
        
    protected:
        typedef std::pair<TKeyValue, CallbackFunction> CallbackHandlerPair;
        std::map<TKeyValue, CallbackFunction>          m_Handlers;

    };

}