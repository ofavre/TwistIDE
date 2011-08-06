#ifndef _VIMFUNCTIONS_GETANNO_HPP
#define _VIMFUNCTIONS_GETANNO_HPP 1



#include "VimFunction.hpp"


class VimFunctionGetAnno : public VimFunction
{
#define SLOT_PARAMS VIMFUNCTION_CALLBACK_SLOT_PARAMS_PREFIX,long
    friend class VimFunction;

    protected:
        inline VimFunctionGetAnno(long bufID, sigc::slot<SLOT_PARAMS> callback)
        :   VimFunction("getAnno", bufID, 1, VimValue::LINE_NUMBER)
        ,   mCallback(callback)
        {
        }

    public:
        inline static void call(VimSocketInterfaceCommunicator& vim, long bufID, sigc::slot<SLOT_PARAMS> callback, long serNum)
        {
            VimFunctionGetAnno* call = new VimFunctionGetAnno(bufID, callback);
            vim.call_function(bufID, call, VimValue(VimValue::NUMBER, serNum));
        }

    protected:
        inline void callback(VimSocketInterfaceCommunicator& vim, long bufID, const std::vector<VimValue>& parameters)
        {
            mCallback(vim, bufID, parameters[0].getLineNumber());
        }

    private:
        sigc::slot<SLOT_PARAMS> mCallback;
#undef SLOT_PARAMS
};

#endif // _VIMFUNCTIONS_GETANNO_HPP

