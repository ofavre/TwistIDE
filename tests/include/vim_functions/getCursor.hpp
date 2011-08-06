#ifndef _VIMFUNCTIONS_GETCURSOR_HPP
#define _VIMFUNCTIONS_GETCURSOR_HPP 1



#include "VimFunction.hpp"


class VimFunctionGetCursor : public VimFunction
{
#define SLOT_PARAMS VIMFUNCTION_CALLBACK_SLOT_PARAMS_PREFIX,long,long,long,long
    friend class VimFunction;

    protected:
        inline VimFunctionGetCursor(long bufID, sigc::slot<SLOT_PARAMS> callback)
        :   VimFunction("getCursor", bufID, 4, VimValue::NUMBER, VimValue::LINE_NUMBER, VimValue::COLUMN_NUMBER, VimValue::OFFSET)
        ,   mCallback(callback)
        {
        }

    public:
        inline static void call(VimSocketInterfaceCommunicator& vim, long bufID, sigc::slot<SLOT_PARAMS> callback)
        {
            VimFunctionGetCursor* call = new VimFunctionGetCursor(bufID, callback);
            vim.call_function(bufID, call);
        }

    protected:
        inline void callback(VimSocketInterfaceCommunicator& vim, long bufID, const std::vector<VimValue>& parameters)
        {
            // Abbreviated version mCallback(vim, bufID, ...); doesn't work...
            mCallback.operator()(vim, bufID, parameters[0].getNumber(), parameters[1].getLineNumber(), parameters[2].getColumnNumber(), parameters[3].getOffset());
        }

    private:
        sigc::slot<SLOT_PARAMS> mCallback;
#undef SLOT_PARAMS
};

#endif // _VIMFUNCTIONS_GETCURSOR_HPP

