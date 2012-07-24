//! Libs: -lLLVM-3.1svn -lclangFrontend -lclangParse -lclangDriver -lclangSerialization -lclangSema -lclangEdit -lclangAnalysis -lclangAST -lclangLex -lclangBasic

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
#include <clang/Lex/Preprocessor.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/Utils.h>
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
using clang::driver::ActionList;
using clang::driver::Action;
using clang::driver::InputAction;


static llvm::sys::Path GetExecutablePath(const char *Argv0, bool CanonicalPrefixes);


int main(int argc, char* argv[])
{
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " [clang-options] source.cpp" << endl;
    cerr << "Any clang option is usable, source files may be listed among in no particular order." << endl;
    return EXIT_FAILURE;
  }

  CompilerInstance ci;
  ci.createDiagnostics(argc, argv + 1);

  Path path = GetExecutablePath(argv[0], true);
  Driver driver (path.str(), getDefaultTargetTriple(), "a.out", true, ci.getDiagnostics());
  driver.CCCIsCPP = true; // only preprocess
  OwningPtr<Compilation> compil (driver.BuildCompilation(llvm::ArrayRef<const char*>(argv, argc)));
  if (!compil) {
    cerr << "Could not build a Compilation!" << endl;
    return EXIT_FAILURE;
  }

  driver.PrintActions(*compil);

  // The following list some unknown arguments (like -###) as inputs...
  //
  // {
  //   const InputArgList& inputs = compil->getInputArgs();
  //   cout << "Input files: (" << inputs.getNumInputArgStrings() << ")" << endl;
  //   for (unsigned i = 0, n = inputs.getNumInputArgStrings() ; i < n ; ++i)
  //     cout << " - " << inputs.getArgString(i) << endl;
  // }
  //
  // {
  //   const DerivedArgList& inputs = compil->getArgs();
  //   cout << "Input files: (" << inputs.getNumInputArgStrings() << ")" << endl;
  //   for (unsigned i = 0, n = inputs.getNumInputArgStrings() ; i < n ; ++i)
  //     cout << " - " << inputs.getArgString(i) << endl;
  // }

  {
    const ActionList& actions = compil->getActions();
    cout << "Actions: (" << actions.size() << ")" << endl;
    for (ActionList::const_iterator it = actions.begin(), end = actions.end() ; it != end ; ++it) {
      const Action& a = *(*it);
      cout << " - " << a.getClassName() << " inputs(" << a.getInputs().size() << ") -> " << clang::driver::types::getTypeName(a.getType()) << endl;
      for (ActionList::const_iterator it2 = a.begin(), end2 = a.end() ; it2 != end2 ; ++it2) {
        const Action& a2 = *(*it2);
        const InputAction* ia = dyn_cast<const InputAction>(&a2);
        if (ia) {
          cout << "    - " << a2.getClassName() << " {";
          const Arg& inputs = ia->getInputArg();
          for (unsigned i = 0, n = inputs.getNumValues() ; i < n ; ++i) {
            cout << "\"" << inputs.getValue(compil->getArgs(), i) << "\"";
            if (i+1 < n) cout << ", ";
          }
          cout << "} -> " << clang::driver::types::getTypeName(a2.getType()) << endl;
        } else
          cout << "    - " << a2.getClassName() << " inputs(" << a2.getInputs().size() << ") -> " << clang::driver::types::getTypeName(a2.getType()) << endl;
      }
    }
  }

  cout << endl;

  //!\\ The following doesn't quite work
  //!\\ Tip: Use -### to print commands instead of executing them.
  //!\\ ExecuteCompilation exec()s the this same program, with extra arguments.
  //!\\ We should take advantage of the parsing done, and do the preprocess ourselves.

  int Res = 0;
  const Command *FailingCommand = 0;
  Res = driver.ExecuteCompilation(*compil, FailingCommand);

  // If result status is < 0, then the driver command signalled an error.
  // In this case, generate additional diagnostic information if possible.
  if (Res < 0)
    driver.generateCompilationDiagnostics(*compil, FailingCommand);

  return EXIT_SUCCESS;
}

/// Turns argv[0] into the executable path
llvm::sys::Path GetExecutablePath(const char *Argv0, bool CanonicalPrefixes)
{
  if (!CanonicalPrefixes)
    return llvm::sys::Path(Argv0);

  // symbol just needs to be some symbol in the binary
  void *symbol = (void*) (intptr_t) GetExecutablePath; // can't take ::main() address, use the current function instead
  return llvm::sys::Path::GetMainExecutable(Argv0, symbol);
}
