//! Libs: -lLLVM-3.1svn -lclangFrontend -lclangParse -lclangDriver -lclangSerialization -lclangSema -lclangEdit -lclangAnalysis -lclangAST -lclangLex -lclangBasic

#include <iostream>
using namespace std;

#include <llvm/Config/config.h>
#include <llvm/Support/Host.h>

#include <clang/Basic/TargetInfo.h>
#include <clang/Basic/FileManager.h>
#include <clang/Driver/ArgList.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/DiagnosticOptions.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Frontend/Utils.h>
using namespace clang;
using clang::driver::InputArgList;
using clang::CompilerInstance;
using clang::DiagnosticOptions;
using clang::TextDiagnosticPrinter;
using clang::DiagnosticConsumer;
using clang::DiagnosticsEngine;
using clang::TargetInfo;
using llvm::sys::getDefaultTargetTriple;
using clang::PrintPreprocessedAction;


static const char *getInputKindName(InputKind Kind);


int main(int argc, char* argv[])
{
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " [clang-options] source.cpp" << endl;
    cerr << "Any clang option is usable, source files may be listed among in no particular order." << endl;
    return EXIT_FAILURE;
  }

  CompilerInstance ci;
  ci.createDiagnostics(argc, argv + 1);
  CompilerInvocation::CreateFromArgs(ci.getInvocation(), argv + 1, argv + argc, ci.getDiagnostics());

  vector<FrontendInputFile>& inputs = ci.getFrontendOpts().Inputs;
  cout << "Input files: (" << inputs.size() << ")" << endl;
  for (vector<FrontendInputFile>::const_iterator it = inputs.begin(), end = inputs.end() ; it != end ; ++it)
    cout << " - type:" << getInputKindName(it->Kind) << "\tfrom:" << (it->IsSystem ? "system" : "user") << "\tname:\"" << it->File << "\"" << endl;

  PrintPreprocessedAction printPpAction;
  ci.ExecuteAction(printPpAction);

  return EXIT_SUCCESS;
}

static const char *getInputKindName(InputKind Kind) {
  switch (Kind) {
  case IK_None:              break;
  case IK_AST:               return "ast";
  case IK_Asm:               return "assembler-with-cpp";
  case IK_C:                 return "c";
  case IK_CXX:               return "c++";
  case IK_LLVM_IR:           return "ir";
  case IK_ObjC:              return "objective-c";
  case IK_ObjCXX:            return "objective-c++";
  case IK_OpenCL:            return "cl";
  case IK_CUDA:              return "cuda";
  case IK_PreprocessedC:     return "cpp-output";
  case IK_PreprocessedCXX:   return "c++-cpp-output";
  case IK_PreprocessedObjC:  return "objective-c-cpp-output";
  case IK_PreprocessedObjCXX:return "objective-c++-cpp-output";
  }

  llvm_unreachable("Unexpected language kind!");
}
