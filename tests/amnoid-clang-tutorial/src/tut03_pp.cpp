//! Libs: -lLLVM-3.1svn -lclangFrontend -lclangParse -lclangDriver -lclangSerialization -lclangSema -lclangEdit -lclangAnalysis -lclangAST -lclangLex -lclangBasic

#include <iostream>
using namespace std;

#include "clang/Frontend/Utils.h"
#include "PPContext.h"
using namespace clang;


int main(int argc, char* argv[])
{
  if (argc != 2) {
    cerr << "No filename given" << endl;
    return EXIT_FAILURE;
  }

  // Create Preprocessor object
  PPContext context;

  // Add header search directories
  PreprocessorOptions ppOpts;
  HeaderSearchOptions hdrOpts ("/usr/");
  hdrOpts.Verbose = true;
  FrontendOptions feOpts;
  clang::InitializePreprocessor(context.pp, ppOpts, hdrOpts, feOpts);
  // This is somehow still insufficient to make the preprocessor recognise <stdio.h>...

  // Add input file
  const FileEntry* File = context.fm.getFile(argv[1]);
  if (!File) {
    cerr << "Failed to open \'" << argv[1] << "\'";
    return EXIT_FAILURE;
  }
  context.sm.createMainFileID(File);
  context.pp.EnterMainSourceFile();
  context.diagClient->BeginSourceFile(context.opts, &context.pp);

  // Parse it
  Token Tok;
  do {
    context.pp.Lex(Tok);
    if (context.diagsEngine.hasErrorOccurred())
      break;
    context.pp.DumpToken(Tok);
    cerr << endl;
  } while (Tok.isNot(tok::eof));

  context.diagClient->EndSourceFile();

  return EXIT_SUCCESS;
}
