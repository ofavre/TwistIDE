#include "LangOptionsBuilder.h"

clang::LangOptionsBuilder& clang::LangOptionsBuilder::newToFree()
{
    return *(new clang::LangOptionsBuilder());
}

clang::LangOptions clang::LangOptionsBuilder::constructor()
{
    clang::LangOptions rtn;
    return rtn;
}


clang::LangOptions clang::LangOptionsBuilder::getLangOptions()
{
    return langOptions;
}

void clang::LangOptionsBuilder::free()
{
    delete this;
}

clang::LangOptions clang::LangOptionsBuilder::getLangOptionsAndFree()
{
    clang::LangOptions rtn (langOptions);
    free();
    return rtn;
}


typedef clang::Visibility Visibility;
typedef clang::LangOptions::GCMode GCMode;
typedef clang::LangOptions::StackProtectorMode StackProtectorMode;
typedef clang::LangOptions::SignedOverflowBehaviorTy SignedOverflowBehaviorTy;
#define LANGOPT(Name, Bits, Default, Description) \
unsigned clang::LangOptionsBuilder::get##Name() const { return langOptions.Name; } \
clang::LangOptionsBuilder& clang::LangOptionsBuilder::set##Name(unsigned Value) { langOptions.Name = Value; return *this; }  
#define ENUM_LANGOPT(Name, Type, Bits, Default, Description) \
Type clang::LangOptionsBuilder::get##Name() const { return langOptions.get##Name(); } \
clang::LangOptionsBuilder& clang::LangOptionsBuilder::set##Name(Type Value) { langOptions.set##Name(Value); return *this; }  
#include "clang/Basic/LangOptions.def"
