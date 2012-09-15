//! Libs: -lLLVM-3.1svn -lclangARCMigrate -lclangStaticAnalyzerFrontend -lclangStaticAnalyzerCheckers -lclangStaticAnalyzerCore -lclangIndex -lclangCodeGen -lclangRewrite -lclangFrontend -lclangParse -lclangDriver -lclangSerialization -lclangSema -lclangEdit -lclangAnalysis -lclangAST -lclangLex -lclangBasic -lclangFrontendTool

#include <iostream>
using namespace std;

#include <llvm/Config/config.h>
#include <llvm/Support/Host.h>

#include <clang/Basic/FileManager.h>
#include <clang/Driver/ArgList.h>
#include <clang/Driver/Arg.h>
#include <clang/Driver/Driver.h>
#include <clang/Driver/Compilation.h>
#include <clang/Driver/Action.h>
#include <clang/Driver/Job.h>
#include <clang/Driver/Tool.h>
#include <clang/Driver/ToolChain.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/Utils.h>
#include <clang/FrontendTool/Utils.h>
#include <clang/Frontend/FrontendDiagnostic.h>
#include <clang/Frontend/ASTUnit.h>
using namespace clang;
using llvm::dyn_cast;
using clang::driver::Arg;
using clang::driver::InputArgList;
using clang::driver::DerivedArgList;
using clang::CompilerInstance;
using llvm::sys::getDefaultTargetTriple;
using llvm::sys::Path;
using llvm::ArrayRef;
using clang::driver::Driver;
using clang::driver::Compilation;
using clang::driver::Command;
using clang::driver::ArgStringList;
using clang::driver::ActionList;
using clang::driver::Action;
using clang::driver::InputAction;
using clang::driver::PreprocessJobAction;
using clang::driver::Job;
using clang::driver::JobAction;
using clang::driver::JobList;
using clang::driver::Tool;
using clang::driver::ToolChain;
using clang::ParsedSourceLocation;
using clang::ASTUnit;

int main(int argc, const char* argv[])
{
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " [clang-options] source.cpp:LINE:COL" << endl;
    cerr << "Outputs code-completion proposals for the given source file location." << endl;
    cerr << "Any clang option is usable, source files may be listed among in no particular order, but only the first reference will be completed." << endl;
    return EXIT_FAILURE;
  }

  string completionLocationStr;
  char* tweaked_input = NULL;
  for (int i = 0 ; i < argc ; ++i) {
    const char* first = NULL;
    if ((first = strchr(argv[i], ':'))) {
      const char* second = NULL;
      if ((second = strchr(first + 1, ':'))) {
        if (first + 1 + strspn(first + 1, "0123456789") == second
            && strspn(second + 1, "0123456789") == strlen(second + 1)) {
          completionLocationStr = argv[i];
          ptrdiff_t len = first - argv[i];
          tweaked_input = new char [len + 1];
          strncpy(tweaked_input, argv[i], len);
          tweaked_input[len] = '\0';
          argv[i] = tweaked_input;
          break;
        }
      }
    }
  }

  if (completionLocationStr.empty()) {
    cerr << "Please specify the line and column where to perform code completion." << endl;
    cerr << "Use the `--help' option for more information." << endl;
    return 1;
  }

  // Add the ResourceDir as an include,
  // because the ResourceFilesPath arg in ASTUnit
  // does not work as expected.
  int argc2 = argc + 2;
  const char* argv2[argc2];
  for (int i = 0 ; i < argc ; ++i)
    argv2[i] = argv[i];
  argv2[argc2-1-1] = "-I";
  argv2[argc2  -1] = "/usr/local/lib/clang/3.1/include";

  DiagnosticOptions diagOpts;
  llvm::IntrusiveRefCntPtr<DiagnosticsEngine> diags (
    CompilerInstance::createDiagnostics(diagOpts, argc, argv)
  );
  OwningPtr<ASTUnit> astUnit (ASTUnit::LoadFromCommandLine(
      argv2 + 1,
      argv2 + argc2,
      diags,
      "", // StringRef ResourceFilesPath
      false, // bool OnlyLocalDecls
      false, // bool CaptureDiagnostics
      NULL, // RemappedFile *RemappedFiles
      0, // unsigned NumRemappedFiles
      true, // bool RemappedFilesKeepOriginalName
      false, // bool PrecompilePreamble
      clang::TU_Complete, // TranslationUnitKind TUKind
      false, // bool CacheCodeCompletionResults
      false // bool AllowPCHWithCompilerErrors
  ));

  if (!astUnit)
    return 1;

  // Prepare for code completion
  FileSystemOptions fsOpts;
  LangOptions langOpts;
  IntrusiveRefCntPtr<FileManager> fileMgr (new FileManager(fsOpts)); // IntrusiveRefCntPtr is needed otherwise we get double free inside CompilerInstance dtor.
  IntrusiveRefCntPtr<SourceManager> sourceMgr (new SourceManager (*diags, *fileMgr)); // idem: IntrusiveRefCntPtr needed
  PrintingCodeCompleteConsumer completionConsumer (
      true, // IncludeMacros
      true, // IncludeCodePatterns
      true, // IncludeGlobals
      llvm::outs()
  );
  SmallVector<StoredDiagnostic, 4> StoredDiagnostics;
  SmallVector<const llvm::MemoryBuffer*, 4> OwnedBuffers;
  ParsedSourceLocation parsedLoc = ParsedSourceLocation::FromString(completionLocationStr);

  // Perform code completion
  astUnit->CodeComplete(parsedLoc.FileName, parsedLoc.Line, parsedLoc.Column,
      NULL, // RemappedFiles
      0, // NumRemappedFiles
      true, // IncludeMacros
      true, // IncludeCodePatterns
      completionConsumer,
      *diags,
      langOpts,
      *sourceMgr,
      *fileMgr,
      StoredDiagnostics,
      OwnedBuffers
  );

  return 0;
}
