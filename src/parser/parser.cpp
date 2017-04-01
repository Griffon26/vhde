/* vim: ts=2:sw=2:expandtab
 *
 * VHDE - The VHDL Diagram Editor
 * Copyright (C) 2017  Maurice van der Pot <griffon26@kfk4ever.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "antlr4-runtime.h"
#include "vhdlLexer.h"
#include "vhdlParser.h"
#include "vhdlBaseVisitor.h"

#include "vhdl_architecture.h"

class vhdlConstructor: public vhdlBaseVisitor
{
  virtual antlrcpp::Any visitDesign_file(vhdlParser::Design_fileContext *ctx) override
  {
    return new VHDLArchitecture("myarch");
  }
};


void parseVHDL(std::istream &stream)
{
  antlr4::ANTLRInputStream input(stream);
  vhdlLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  vhdlParser parser(&tokens);
  antlr4::tree::ParseTree *tree = parser.design_file();

  auto visitor = vhdlConstructor();
  auto result = visitor.visit(tree);
  if(result.isNull())
  {
    std::cout << "visitor output is null" << std::endl;
  }

  auto pArch = result.as<VHDLArchitecture *>();

  std::cout << "visitor output: [" << pArch << "]" << std::endl;

  std::cout << "name of arch is: [" << pArch->getName() << "]" << std::endl;

  std::cout << tree->toStringTree(&parser) << std::endl;

  delete pArch;
}

