#include "antlr4-runtime.h"
#include "vhdlLexer.h"
#include "vhdlParser.h"

void parseVHDL(std::istream &stream)
{
  antlr4::ANTLRInputStream input(stream);
  vhdlLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  vhdlParser parser(&tokens);
  antlr4::tree::ParseTree *tree = parser.design_file();

  std::cout << tree->toStringTree(&parser) << std::endl;
}

