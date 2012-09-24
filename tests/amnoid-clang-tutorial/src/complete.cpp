//! Libs: -lLLVM-3.1svn -lclangARCMigrate -lclangStaticAnalyzerFrontend -lclangStaticAnalyzerCheckers -lclangStaticAnalyzerCore -lclangIndex -lclangCodeGen -lclangRewrite -lclangFrontend -lclangParse -lclangDriver -lclangSerialization -lclangSema -lclangEdit -lclangAnalysis -lclangAST -lclangLex -lclangBasic -lclangFrontendTool
//! Libs: -lreadline

#include <cstdio>
#include <readline/readline.h>
#include <readline/history.h>

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



class Completer {
  public:
    Completer();
    bool load_file(string path);
    bool complete(string path, long line, long col);
  private:
    FileSystemOptions fsOpts;
    LangOptions langOpts;
    DiagnosticOptions diagOpts;
    llvm::IntrusiveRefCntPtr<DiagnosticsEngine> diags;
    IntrusiveRefCntPtr<FileManager> fileMgr; // IntrusiveRefCntPtr is needed otherwise we get double free inside CompilerInstance dtor (from ASTUnit).
    IntrusiveRefCntPtr<SourceManager> sourceMgr; // idem: IntrusiveRefCntPtr needed
    PrintingCodeCompleteConsumer completionConsumer;
};

int main(int argc, const char* argv[]);
void print_usage(const char* argv0);
void enter_interactive(Completer* completer = NULL);



int main(int argc, const char* argv[])
{
  if (argc == 2) {

    if (strcmp("-h", argv[1]) == 0 || strcmp("--help", argv[1]) == 0) {

      print_usage(argv[0]);
      return EXIT_SUCCESS;

    } else {

      const char* str = argv[1];
      const char* colon = strchr(str, ':');
      string file (str, colon - str);
      str = colon + 1;
      colon = strchr(str, ':');
      long int line = strtol(str, NULL, 10);
      str = colon + 1;
      colon = strchr(str, '\0');
      long int col = strtol(str, NULL, 10);

      Completer completer;
      completer.load_file(file);
      completer.complete(file, line, col);

    }

  } else if (argc > 2) {

    print_usage(argv[0]);
    return EXIT_FAILURE;

  } else {

    enter_interactive();

  }

  return EXIT_SUCCESS;
}

void print_usage(const char* argv0)
{
  cerr << "Usage: " << argv0 << " [clang-options] source.cpp:LINE:COL" << endl;
  cerr << "Outputs code-completion proposals for the given source file location." << endl;
  cerr << "Any clang option is usable, source files may be listed among in no particular order, but only the first reference will be completed." << endl;
}

void enter_interactive(Completer* _completer)
{
  Completer* completer = NULL;
  if (_completer != NULL)
    completer = _completer;
  else
    completer = new Completer();

  bool quit = false;

  while (!quit)
  {
    string input;
    char * str = readline("> ");
    if (str == NULL) {
      cout << endl;
      quit = true;
      continue;
    }
    input.assign(str);
    // Trim
    input.erase(0, input.find_first_not_of(" \t"));
    input.erase(input.find_last_not_of(" \t"));
    // Check for empty lines
    if (input.empty()) {
      continue;
    }
    // Add the original line (with blanks) to the history
    add_history(str);
    free(str);
    str = NULL;

    enum Action { Noop, LoadFile, Complete, Help, Error };
    Action action = Noop;
    string file;
    long line = -1;
    long col = -1;

    size_t colon;
    colon = input.find(":");
    if (colon != string::npos) {
      file = input.substr(0, colon);
      size_t colon2 = input.find(":", colon+1);
      if (colon2 != string::npos) {
        line = strtol(input.substr(colon+1, colon2).c_str(), NULL, 10);
        col = strtol(input.substr(colon2+1).c_str(), NULL, 10);
        action = Complete;
      } else {
        action = Error;
      }
    } else if (input == "/?" || input == ".h" || input == ".help" || input == "help") {
      action = Help;
    } else if (input.size() != 0) {
      action = LoadFile;
    }

    switch (action) {
      case LoadFile: {
        bool rtn;
        rtn = completer->load_file(file);
        if (!rtn) {
          cout << "An error occured" << endl;
        }
      } break;
      case Complete: {
        bool rtn;
        rtn = completer->complete(file, line, col);
        if (!rtn) {
          cout << "An error occured" << endl;
        }
      } break;
      case Help: {
        cout << "Available commands:" << endl;
        cout << "  /?, .h, .help, help     Prints this help." << endl;
        cout << "  {file}                  Loads and parses the given file." << endl;
        cout << "  {file}:{line}:{column}  Code complete at the given location of the given file." << endl;
        cout << "                          Loads the file if not already loaded." << endl;
      } break;
      case Error: {
        cout << "Unrecognized command. Type /? for help." << endl;
      } break;
      default: // fall-through
      case Noop: {
      } break;
    }
  }

  if (_completer == NULL)
    delete completer;
}

Completer::Completer()
: diags(CompilerInstance::createDiagnostics(diagOpts, 0, NULL))
, fileMgr(new FileManager(fsOpts))
, sourceMgr(new SourceManager(*diags, *fileMgr))
, completionConsumer(
    true, // IncludeMacros
    true, // IncludeCodePatterns
    true, // IncludeGlobals
    llvm::outs()
  )
{
}

bool Completer::load_file(string path)
{
  (void)path;
  cout << "Not implemented" << endl;
  return true;
}

bool Completer::complete(string path, long line, long col)
{
  // Add the ResourceDir as an include,
  // because the ResourceFilesPath arg in ASTUnit
  // does not work as expected.
  const int argc = 3;
  const char* argv[argc];
  argv[0] = "-I";
  argv[1] = "/usr/local/lib/clang/3.1/include";
  argv[2] = path.c_str();

  OwningPtr<ASTUnit> astUnit (ASTUnit::LoadFromCommandLine(
      argv,
      argv + argc,
      diags,
      "", // StringRef ResourceFilesPath
      false, // bool OnlyLocalDecls
      true, // bool CaptureDiagnostics
      NULL, // RemappedFile *RemappedFiles
      0, // unsigned NumRemappedFiles
      true, // bool RemappedFilesKeepOriginalName
      false, // bool PrecompilePreamble
      clang::TU_Complete, // TranslationUnitKind TUKind
      false, // bool CacheCodeCompletionResults
      false // bool AllowPCHWithCompilerErrors
  ));

  if (!astUnit)
    return false;

  // Prepare for code completion
  SmallVector<StoredDiagnostic, 4> StoredDiagnostics;
  SmallVector<const llvm::MemoryBuffer*, 4> OwnedBuffers;

  // Perform code completion
  astUnit->CodeComplete(path, line, col,
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

  return true;
}
