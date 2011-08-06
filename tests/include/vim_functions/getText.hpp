#ifndef _VIMFUNCTIONS_GETTEXT_HPP
#define _VIMFUNCTIONS_GETTEXT_HPP 1



#include "VimFunction.hpp"


class VimFunctionGetText : public VimFunction
{
#define SLOT_PARAMS VIMFUNCTION_CALLBACK_SLOT_PARAMS_PREFIX,std::string
    friend class VimFunction;

    protected:
        inline VimFunctionGetText(long bufID, sigc::slot<SLOT_PARAMS> callback)
        :   VimFunction("getText", bufID, 1, VimValue::STRING)
        ,   mCallback(callback)
        {
        }

    public:
        inline static void call(VimSocketInterfaceCommunicator& vim, long bufID, sigc::slot<SLOT_PARAMS> callback)
        {
            VimFunctionGetText* call = new VimFunctionGetText(bufID, callback);
            vim.call_function(bufID, call);
        }

    protected:
        inline void callback(VimSocketInterfaceCommunicator& vim, long bufID, const std::vector<VimValue>& parameters)
        {
            mCallback(vim, bufID, parameters[0].getString());
        }

    private:
        sigc::slot<SLOT_PARAMS> mCallback;
#undef SLOT_PARAMS
};

#endif // _VIMFUNCTIONS_GETTEXT_HPP

