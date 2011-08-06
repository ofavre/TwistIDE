#ifndef _VIMFUNCTIONS_GETMODIFIED_HPP
#define _VIMFUNCTIONS_GETMODIFIED_HPP 1



#include "VimFunction.hpp"


class VimFunctionGetModified : public VimFunction
{
#define SLOT_PARAMS VIMFUNCTION_CALLBACK_SLOT_PARAMS_PREFIX,long
    friend class VimFunction;

    protected:
        inline VimFunctionGetModified(long bufID, sigc::slot<SLOT_PARAMS> callback)
        :   VimFunction("getModified", bufID, 1, VimValue::NUMBER)
        ,   mCallback(callback)
        {
        }

    public:
        inline static void call(VimSocketInterfaceCommunicator& vim, long bufID, sigc::slot<SLOT_PARAMS> callback)
        {
            VimFunctionGetModified* call = new VimFunctionGetModified(bufID, callback);
            vim.call_function(bufID, call);
        }

    protected:
        inline void callback(VimSocketInterfaceCommunicator& vim, long bufID, const std::vector<VimValue>& parameters)
        {
            mCallback(vim, bufID, parameters[0].getNumber());
        }

    private:
        sigc::slot<SLOT_PARAMS> mCallback;
#undef SLOT_PARAMS
};

#endif // _VIMFUNCTIONS_GETMODIFIED_HPP

