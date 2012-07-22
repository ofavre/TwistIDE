//! Libs: -lLLVM-3.1svn -lclangFrontend -lclangParse -lclangDriver -lclangSerialization -lclangSema -lclangEdit -lclangAnalysis -lclangAST -lclangLex -lclangBasic

#include <iostream>
using namespace std;

#include <llvm/Config/config.h>
#include <llvm/Support/Host.h>

#include <clang/Basic/TargetInfo.h>
#include <clang/Basic/FileManager.h>
#include <clang/Driver/ArgList.h>
#include <clang/Lex/Preprocessor.h>
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


int main(int argc, char* argv[])
{
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " source.cpp [clang-options]" << endl;
    return EXIT_FAILURE;
  }

  TargetOptions targetOptions;
  targetOptions.Triple = getDefaultTargetTriple();

  CompilerInstance ci;
  ci.createDiagnostics(argc - 1, argv + 2);
  ci.setTarget(TargetInfo::CreateTargetInfo(ci.getDiagnostics(), targetOptions));
  ci.createFileManager();
  ci.createSourceManager(ci.getFileManager());
  ci.createPreprocessor();

  ci.getDiagnostics().setDiagnosticMapping(diag::DIAG_START_ANALYSIS, diag::MAP_IGNORE, SourceLocation());

  // Add input file
  const FileEntry* File = ci.getFileManager().getFile(argv[1]);
  if (!File) {
    cerr << "Failed to open \'" << argv[1] << "\'";
    return EXIT_FAILURE;
  }
  ci.getSourceManager().createMainFileID(File);
  ci.getPreprocessor().EnterMainSourceFile();
  ci.getDiagnosticClient().BeginSourceFile(ci.getLangOpts(), &ci.getPreprocessor());

  // Parse it
  Token Tok;
  do {
    ci.getPreprocessor().Lex(Tok);
    if (ci.getDiagnostics().hasErrorOccurred())
      break;
    ci.getPreprocessor().DumpToken(Tok);
    cerr << endl;
  } while (Tok.isNot(tok::eof));

  ci.getDiagnosticClient().EndSourceFile();

  return EXIT_SUCCESS;
}
