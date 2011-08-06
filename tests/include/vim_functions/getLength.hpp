#ifndef _VIMFUNCTIONS_GETLENGTH_HPP
#define _VIMFUNCTIONS_GETLENGTH_HPP 1



#include "VimFunction.hpp"


class VimFunctionGetLength : public VimFunction
{
#define SLOT_PARAMS VIMFUNCTION_CALLBACK_SLOT_PARAMS_PREFIX,long
    friend class VimFunction;

    protected:
        inline VimFunctionGetLength(long bufID, sigc::slot<SLOT_PARAMS> callback)
        :   VimFunction("getLength", bufID, 1, VimValue::NUMBER)
        ,   mCallback(callback)
        {
        }

    public:
        inline static void call(VimSocketInterfaceCommunicator& vim, long bufID, sigc::slot<SLOT_PARAMS> callback)
        {
            VimFunctionGetLength* call = new VimFunctionGetLength(bufID, callback);
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

#endif // _VIMFUNCTIONS_GETLENGTH_HPP

