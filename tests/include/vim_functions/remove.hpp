#ifndef _VIMFUNCTIONS_REMOVE_HPP
#define _VIMFUNCTIONS_REMOVE_HPP 1



#include "VimFunction.hpp"


class VimFunctionRemove : public VimFunction
{
#define SLOT_PARAMS VIMFUNCTION_CALLBACK_SLOT_PARAMS_PREFIX,bool
    friend class VimFunction;

    protected:
        inline VimFunctionRemove(long bufID, sigc::slot<SLOT_PARAMS> callback)
        :   VimFunction("remove", bufID)
        ,   mCallback(callback)
        {
        }

    public:
        inline static void call(VimSocketInterfaceCommunicator& vim, long bufID, sigc::slot<SLOT_PARAMS> callback, long off, long length)
        {
            VimFunctionRemove* call = new VimFunctionRemove(bufID, callback);
            vim.call_function(bufID, call, VimValue(VimValue::OFFSET, off), VimValue(VimValue::NUMBER, length));
        }

    protected:
        inline void callback(VimSocketInterfaceCommunicator& vim, long bufID, const std::vector<VimValue>& parameters)
        {
            mCallback(vim, bufID, true);
        }

        inline virtual void error_callback(VimSocketInterfaceCommunicator& vim, long bufID, const std::string& message)
        {
            mCallback(vim, bufID, false);
        }

    private:
        sigc::slot<SLOT_PARAMS> mCallback;
#undef SLOT_PARAMS
};

#endif // _VIMFUNCTIONS_REMOVE_HPP

