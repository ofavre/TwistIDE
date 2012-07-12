#ifndef _PP_CONTEXT_H
#define _PP_CONTEXT_H 1

#include <iostream>
#include <string>

#include <llvm/Config/config.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Support/Host.h> // getDefaultTargetTriple()

#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/DiagnosticIDs.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Basic/TargetOptions.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Basic/FileManager.h>

#include <clang/Lex/HeaderSearch.h>
#include <clang/Lex/Preprocessor.h>

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/DiagnosticOptions.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>


#include "TargetOptionsBuilder.h"
#include "LangOptionsBuilder.h"



struct PPContext
{
    // Takes ownership of client.
    PPContext(clang::DiagnosticConsumer* client = 0, const std::string& triple = llvm::sys::getDefaultTargetTriple())
    : rawOstream(llvm::outs()) // can use std::cout too
    , diagClient(client == 0 ? new clang::TextDiagnosticPrinter(rawOstream, diagOpts) : client)
    , diagsEngine(refs, diagClient, true) // Takes ownership of client
    , diags(&diagsEngine)
    , opts(LangOptionsBuilder::newToFree().setC99(true).getLangOptionsAndFree())
    , targetOptions(TargetOptionsBuilder::constructor(triple))
    , target(clang::TargetInfo::CreateTargetInfo(diagsEngine, targetOptions))
    , fm(fso)
    , headers(fm, diagsEngine, opts, target)
    , sm(diagsEngine, fm)
    , pp(diagsEngine, opts, target, sm, headers, ci)
    {
        // Configure warnings to be similar to what command-line `clang` outputs
        // (see tut03).
        // XXX: ove warning initialization to libDriver

        using namespace clang;
        diagsEngine.setDiagnosticMapping(diag::DIAG_START_ANALYSIS, diag::MAP_IGNORE, loc);
    }

    ~PPContext()
    {
        delete target;
    }

    llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> refs;
    llvm::raw_ostream& rawOstream;
    clang::DiagnosticOptions diagOpts;
    clang::DiagnosticConsumer* diagClient; // Owned by diags, do not free
    clang::DiagnosticsEngine diagsEngine;
    clang::Diagnostic diags;
    clang::LangOptions opts;
    clang::TargetOptions targetOptions;
    clang::TargetInfo* target;
    clang::FileSystemOptions fso;
    clang::FileManager fm;
    clang::HeaderSearch headers;
    clang::SourceManager sm;
    clang::SourceLocation loc;
    clang::CompilerInstance ci; // implements ModuleLoader
    clang::Preprocessor pp;
};



#endif //! _PP_CONTEXT_H

