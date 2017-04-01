#include "antlr4-runtime.h"
#include "vhdlLexer.h"
#include "vhdlParser.h"

void parseVHDL(std::istream &stream)
{
  ANTLRInputStream input(stream);
  vhdlLexer lexer(&input);
  CommonTokenStream tokens(lexer);
  vhdlParser parser(&tokens);
  tree::ParseTree *tree = parser.main();

  std::cout << tree->toStringTree(&parser) << std::endl;
}

