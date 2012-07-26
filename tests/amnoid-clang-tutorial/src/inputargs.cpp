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
#include <clang/Driver/Job.h>
#include <clang/Driver/Tool.h>
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
using clang::driver::Job;
using clang::driver::JobList;
using clang::driver::Tool;


static llvm::sys::Path GetExecutablePath(const char *Argv0, bool CanonicalPrefixes);
int ExecuteCompilation(const Driver* that, const Compilation &C,
                               const Command *&FailingCommand);
int ExecuteCommand(const Compilation* that, const Command &C,
                                const Command *&FailingCommand);
int ExecuteJob(const Compilation* that, const Job &J,
                            const Command *&FailingCommand);


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
  Res = ExecuteCompilation(&driver, *compil, FailingCommand);

  // If result status is < 0, then the driver command signalled an error.
  // In this case, generate additional diagnostic information if possible.
  if (Res < 0)
    driver.generateCompilationDiagnostics(*compil, FailingCommand);

  return EXIT_SUCCESS;
}


/// Copied from tools/driver/driver.cpp, where it was a static (local) function.
/// Turns argv[0] into the executable path.
llvm::sys::Path GetExecutablePath(const char *Argv0, bool CanonicalPrefixes)
{
  if (!CanonicalPrefixes)
    return llvm::sys::Path(Argv0);

  // symbol just needs to be some symbol in the binary
  void *symbol = (void*) (intptr_t) GetExecutablePath; // can't take ::main() address, use the current function instead
  return llvm::sys::Path::GetMainExecutable(Argv0, symbol);
}


// Some necessary includes for the following 3 functions
#include <clang/Driver/Options.h>
#include <clang/Driver/DriverDiagnostic.h>
#include <llvm/Support/Program.h>

/// Copied from Driver::ExecuteCompilation().
/// Modified to call the modified ExecuteJob().
int ExecuteCompilation(const Driver* that, const Compilation &C,
                               const Command *&FailingCommand) {
  // Just print if -### was present.
  if (C.getArgs().hasArg(clang::driver::options::OPT__HASH_HASH_HASH)) {
    C.PrintJob(llvm::errs(), C.getJobs(), "\n", true);
    return 0;
  }

  // If there were errors building the compilation, quit now.
  if (that->getDiags().hasErrorOccurred())
    return 1;

  int Res = ExecuteJob(&C, C.getJobs(), FailingCommand);

  // Remove temp files.
  C.CleanupFileList(C.getTempFiles());

  // If the command succeeded, we are done.
  if (Res == 0)
    return Res;

  // Otherwise, remove result files as well.
  if (!C.getArgs().hasArg(clang::driver::options::OPT_save_temps)) {
    C.CleanupFileList(C.getResultFiles(), true);

    // Failure result files are valid unless we crashed.
    if (Res < 0) {
      C.CleanupFileList(C.getFailureResultFiles(), true);
#ifdef _WIN32
      // Exit status should not be negative on Win32,
      // unless abnormal termination.
      Res = 1;
#endif
    }
  }

  // Print extra information about abnormal failures, if possible.
  //
  // This is ad-hoc, but we don't want to be excessively noisy. If the result
  // status was 1, assume the command failed normally. In particular, if it was
  // the compiler then assume it gave a reasonable error code. Failures in other
  // tools are less common, and they generally have worse diagnostics, so always
  // print the diagnostic there.
  const Tool &FailingTool = FailingCommand->getCreator();

  if (!FailingCommand->getCreator().hasGoodDiagnostics() || Res != 1) {
    // FIXME: See FIXME above regarding result code interpretation.
    if (Res < 0)
      that->Diag(clang::diag::err_drv_command_signalled)
        << FailingTool.getShortName();
    else
      that->Diag(clang::diag::err_drv_command_failed)
        << FailingTool.getShortName() << Res;
  }

  return Res;
}

/// Copied from Compilation::ExecuteCommand().
/// Changed to print command options, and not execute anything.
int ExecuteCommand(const Compilation* that, const Command &C,
                                const Command *&FailingCommand) {
  llvm::sys::Path Prog(C.getExecutable());
  const char **Argv = new const char*[C.getArguments().size() + 2];
  Argv[0] = C.getExecutable();
  std::copy(C.getArguments().begin(), C.getArguments().end(), Argv+1);
  Argv[C.getArguments().size() + 1] = 0;

  if ((that->getDriver().CCCEcho || that->getDriver().CCPrintOptions ||
       that->getArgs().hasArg(clang::driver::options::OPT_v)) && !that->getDriver().CCGenDiagnostics) {
    raw_ostream *OS = &llvm::errs();

    // Follow gcc implementation of CC_PRINT_OPTIONS; we could also cache the
    // output stream.
    if (that->getDriver().CCPrintOptions && that->getDriver().CCPrintOptionsFilename) {
      std::string Error;
      OS = new llvm::raw_fd_ostream(that->getDriver().CCPrintOptionsFilename,
                                    Error,
                                    llvm::raw_fd_ostream::F_Append);
      if (!Error.empty()) {
        that->getDriver().Diag(clang::diag::err_drv_cc_print_options_failure)
          << Error;
        FailingCommand = &C;
        delete OS;
        return 1;
      }
    }

    if (that->getDriver().CCPrintOptions)
      *OS << "[Logging clang options]";

    that->PrintJob(*OS, C, "\n", /*Quote=*/that->getDriver().CCPrintOptions);

    if (OS != &llvm::errs())
      delete OS;
  }

  for (const char** i = Argv ; *i ; ++i)
    cout << "\"" << *i << "\" ";
  cout << endl;

  std::string Error;
  int Res = 0;
  // Do not execute
  // Res = llvm::sys::Program::ExecuteAndWait(Prog, Argv,
  //                                    /*env*/0, /*that->Redirects (unfortunately private and inacessible)*/0,
  //                                    /*secondsToWait*/0, /*memoryLimit*/0,
  //                                    &Error);
  if (!Error.empty()) {
    assert(Res && "Error string set with 0 result code!");
    that->getDriver().Diag(clang::diag::err_drv_command_failure) << Error;
  }

  if (Res)
    FailingCommand = &C;

  delete[] Argv;
  return Res;
}

/// Copied from Compilation::ExecuteJob().
/// changed in order to call the modified ExecuteCommand().
int ExecuteJob(const Compilation* that, const Job &J,
                            const Command *&FailingCommand) {
  if (const Command *C = dyn_cast<Command>(&J)) {
    return ExecuteCommand(that, *C, FailingCommand);
  } else {
    const JobList *Jobs = cast<JobList>(&J);
    for (JobList::const_iterator
           it = Jobs->begin(), ie = Jobs->end(); it != ie; ++it) {
      if (int Res = ExecuteJob(that, **it, FailingCommand))
        return Res;
    }
    return 0;
  }
}
