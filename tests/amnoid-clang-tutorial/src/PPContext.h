#ifndef _PP_CONTEXT_H
#define _PP_CONTEXT_H 1

#include <iostream>
#include <string>

#include "llvm/Config/config.h"
#include "llvm/Support/raw_os_ostream.h"

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/DiagnosticIDs.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/FileManager.h"

#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/Preprocessor.h"

#include "clang/Frontend/DiagnosticOptions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"


#include "TargetOptionsBuilder.h"



struct PPContext
{
    // Takes ownership of client.
    PPContext(clang::DiagnosticClient* client = 0, const std::string& triple = LLVM_HOSTTRIPLE)
    : rawOstream(std::cout)
    , diagClient(client == 0 ? new clang::TextDiagnosticPrinter(rawOstream, diagOpts) : client)
    , diags(refs, diagClient, true) // Takes ownership of client
    , targetOptions(TargetOptionsBuilder::constructor(triple))
    , target(clang::TargetInfo::CreateTargetInfo(diags, targetOptions))
    , fm(fso)
    , headers(fm)
    , sm(diags, fm)
    , pp(diags, opts, *target, sm, headers)
    {
        // Configure warnings to be similar to what command-line `clang` outputs
        // (see tut03).
        // XXX: ove warning initialization to libDriver

        using namespace clang;
        diags.setDiagnosticMapping(diag::DIAG_START_ANALYSIS, diag::MAP_IGNORE, loc);
    }

    ~PPContext()
    {
        delete target;
    }

    llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> refs;
    llvm::raw_os_ostream rawOstream;
    clang::DiagnosticOptions diagOpts;
    clang::DiagnosticClient* diagClient; // Owned by diags, do not free
    clang::Diagnostic diags;
    clang::LangOptions opts;
    clang::TargetOptions targetOptions;
    clang::TargetInfo* target;
    clang::FileSystemOptions fso;
    clang::FileManager fm;
    clang::HeaderSearch headers;
    clang::SourceManager sm;
    clang::Preprocessor pp;
    clang::SourceLocation loc;
};



#endif //! _PP_CONTEXT_H

