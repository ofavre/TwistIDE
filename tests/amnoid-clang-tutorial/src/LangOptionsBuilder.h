#ifndef _LANGOPTIONSBUILDER_H
#define _LANGOPTIONSBUILDER_H 1



#include <string>

#include "clang/Basic/LangOptions.h"


namespace clang {

/**
 * A class for inline construction and initialization
 * of clang::LangOptions instances.
 */
class LangOptionsBuilder
{
    private:
        clang::LangOptions langOptions;

    public:
        inline static LangOptionsBuilder& newToFree();
        inline static clang::LangOptions constructor();

        // Define accessors/mutators for language options of enumeration type.
        typedef clang::LangOptions::GCMode GCMode;
        typedef clang::LangOptions::StackProtectorMode StackProtectorMode;
        typedef clang::LangOptions::SignedOverflowBehaviorTy SignedOverflowBehaviorTy;
#define LANGOPT(Name, Bits, Default, Description) \
        inline unsigned get##Name() const; \
        inline LangOptionsBuilder& set##Name(unsigned Value);
#define ENUM_LANGOPT(Name, Type, Bits, Default, Description) \
        inline Type get##Name() const; \
        inline LangOptionsBuilder& set##Name(Type Value);
#include "clang/Basic/LangOptions.def"

        inline clang::LangOptions getLangOptions();
        inline void free();
        inline clang::LangOptions getLangOptionsAndFree();
};

}

typedef clang::LangOptionsBuilder LangOptionsBuilder;

#include "LangOptionsBuilder.inl"



#endif //! _LANGOPTIONSBUILDER_H

