#ifndef _VIMFUNCTIONS_SAVEANDEXIT_HPP
#define _VIMFUNCTIONS_SAVEANDEXIT_HPP 1



#include "VimFunction.hpp"


class VimFunctionSaveAndExit : public VimFunction
{
#define SLOT_PARAMS VIMFUNCTION_CALLBACK_SLOT_PARAMS_PREFIX,long
    friend class VimFunction;

    protected:
        inline VimFunctionSaveAndExit(long bufID, sigc::slot<SLOT_PARAMS> callback)
        :   VimFunction("saveAndExit", bufID, 1, VimValue::NUMBER)
        ,   mCallback(callback)
        {
        }

    public:
        inline static void call(VimSocketInterfaceCommunicator& vim, long bufID, sigc::slot<SLOT_PARAMS> callback)
        {
            VimFunctionSaveAndExit* call = new VimFunctionSaveAndExit(bufID, callback);
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

#endif // _VIMFUNCTIONS_SAVEANDEXIT_HPP

