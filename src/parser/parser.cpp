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

Direction directionFromSignalMode(std::string modeString)
{
  std::transform(std::begin(modeString), std::end(modeString), std::begin(modeString), ::toupper);

  if(modeString == "IN")
  {
    return DIR_IN;
  }
  else if(modeString == "OUT")
  {
    return DIR_OUT;
  }
  else if(modeString == "INOUT")
  {
    return DIR_INOUT;
  }
  else
  {
    return DIR_INVALID;
  }
}

class vhdlConstructor: public vhdlBaseVisitor
{
public:
  std::map<std::string, VHDLEntity *> m_entityStore;
  std::map<std::string, VHDLArchitecture *> m_architectureStore;
  std::map<std::string, std::vector<VHDLComponent *>> m_entityLessComponents;

  VHDLArchitecture *m_pCurrentArchitecture;

private:
  typedef std::pair<std::string, std::string> stringpair;

  virtual antlrcpp::Any visitDesign_file(vhdlParser::Design_fileContext *ctx) override {
    for(auto &el: ctx->design_unit())
    {
      visit(el);
    }
    return nullptr;
  }

  virtual antlrcpp::Any visitDesign_unit(vhdlParser::Design_unitContext *ctx) override {
    return visit(ctx->library_unit());
  }

  virtual antlrcpp::Any visitLibrary_unit(vhdlParser::Library_unitContext *ctx) override {
    if(ctx->primary_unit())
    {
      return visit(ctx->primary_unit());
    }
    else
    {
      assert(ctx->secondary_unit());
      return visit(ctx->secondary_unit());
    }
  }

  virtual antlrcpp::Any visitPrimary_unit(vhdlParser::Primary_unitContext *ctx) override {
    if(ctx->entity_declaration())
    {
      return visit(ctx->entity_declaration());
    }
    else
    {
      throw antlr4::RuntimeException("Parsing of primary units other than entity declarations is not implemented");
    }
  }

  virtual antlrcpp::Any visitSecondary_unit(vhdlParser::Secondary_unitContext *ctx) override {
    assert(ctx->architecture_body());
    return visit(ctx->architecture_body());
  }

  virtual antlrcpp::Any visitEntity_declaration(vhdlParser::Entity_declarationContext *ctx) override {
    auto name = visit(ctx->identifier(0)).as<std::string>();
    m_entityStore[name] = new VHDLEntity(name);
    return nullptr;
  }

  virtual antlrcpp::Any visitEntity_header(vhdlParser::Entity_headerContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitArchitecture_body(vhdlParser::Architecture_bodyContext *ctx) override {
    auto archName = visit(ctx->identifier(0)).as<std::string>();
    auto entityName = visit(ctx->identifier(1)).as<std::string>();
    auto pArch = new VHDLArchitecture(archName);
    pArch->setEntity(m_entityStore.at(entityName));
    m_architectureStore[archName] = pArch;

    m_pCurrentArchitecture = pArch;
    visit(ctx->architecture_declarative_part());
    visit(ctx->architecture_statement_part());
    return nullptr;
  }

  virtual antlrcpp::Any visitArchitecture_declarative_part(vhdlParser::Architecture_declarative_partContext *ctx) override {
    for(auto &el: ctx->block_declarative_item())
    {
      auto value = visit(el);
      if(value.is<VHDLComponent *>())
      {
        m_pCurrentArchitecture->init_addComponent(value);
      }
      else if(value.is<std::vector<VHDLSignal *>>())
      {
        std::vector<VHDLSignal *> signals = value;
        for(auto pSignal: signals)
        {
          m_pCurrentArchitecture->init_addSignal(pSignal);
        }
      }
      else
      {
        assert(false);
      }
    }
    return nullptr;
  }

  virtual antlrcpp::Any visitArchitecture_statement_part(vhdlParser::Architecture_statement_partContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitArchitecture_statement(vhdlParser::Architecture_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitComponent_instantiation_statement(vhdlParser::Component_instantiation_statementContext *ctx) override {
    std::string instanceName = visit(ctx->label_colon()->identifier());
    std::string componentName = visit(ctx->instantiated_unit()->name());

    auto pInstance = new VHDLInstance(instanceName, m_pCurrentArchitecture->findComponentByName(componentName));
    m_pCurrentArchitecture->init_addInstance(pInstance);

    std::vector<stringpair> port_maps = visit(ctx->port_map_aspect()->association_list());
    for(auto port_map: port_maps)
    {
      auto pPort = pInstance->getComponent()->findPortByName(port_map.first);

      auto signalNameUpper = port_map.second;
      std::transform(std::begin(signalNameUpper), std::end(signalNameUpper), std::begin(signalNameUpper), ::toupper);

      if(signalNameUpper != "OPEN")
      {
        auto pSignal = m_pCurrentArchitecture->findSignalByName(port_map.second);
        std::cout << "connecting signal " << port_map.second << " to port " << port_map.first << std::endl;
        pInstance->connectSignalToPort(pSignal, pPort);
      }
    }

    return nullptr;
  }

  virtual antlrcpp::Any visitPort_map_aspect(vhdlParser::Port_map_aspectContext *ctx) override {
    return visit(ctx->association_list());
  }

  virtual antlrcpp::Any visitAssociation_list(vhdlParser::Association_listContext *ctx) override {

    std::vector<stringpair> list;

#if 1
    for(auto &el: ctx->association_element())
    {
      list.push_back(visit(el).as<stringpair>());
    }
#else
    #warning "figure out why this loop won't work"
    std::transform(std::begin(ctx->association_element()),
                   std::end(ctx->association_element()),
                   std::back_inserter(list),
                   [this](vhdlParser::Association_elementContext *elctx) { return this->visit(elctx).as<stringpair>(); });
#endif

    return list;
  }

  virtual antlrcpp::Any visitAssociation_element(vhdlParser::Association_elementContext *ctx) override {
    return stringpair(visit(ctx->formal_part()).as<std::string>(), visit(ctx->actual_part()).as<std::string>());
  }

  virtual antlrcpp::Any visitFormal_part(vhdlParser::Formal_partContext *ctx) override {
    return ctx->getText();
  }

  virtual antlrcpp::Any visitActual_part(vhdlParser::Actual_partContext *ctx) override {
    return ctx->getText();
  }

  virtual antlrcpp::Any visitBlock_declarative_item(vhdlParser::Block_declarative_itemContext *ctx) override {
    if(ctx->component_declaration())
    {
      return visit(ctx->component_declaration());
    }
    else if(ctx->signal_declaration())
    {
      return visit(ctx->signal_declaration());
    }
    else
    {
        throw antlr4::RuntimeException("Parsing of architecture declarative parts other than component declarations and signal declarations is not implemented");
    }
  }

  virtual antlrcpp::Any visitSignal_declaration(vhdlParser::Signal_declarationContext *ctx) override {
    std::vector<VHDLSignal *> signals;

    for(auto &identctx: ctx->identifier_list()->identifier())
    {
      auto pSignal = new VHDLSignal(visit(identctx).as<std::string>());
      // TODO: also parse and store subtype_indication

      signals.push_back(pSignal);
    }
    return signals;
  }

  virtual antlrcpp::Any visitComponent_declaration(vhdlParser::Component_declarationContext *ctx) override {
    std::string entityName = visit(ctx->identifier(0));
    auto pComponent = new VHDLComponent(entityName);

    // Remember it so we can resolve all the entity references once we know all entities
    m_entityLessComponents[entityName].push_back(pComponent);

    std::vector<VHDLPort *> ports = visit(ctx->port_clause());

    for(auto pPort: ports)
    {
      std::cout << "Adding port " << pPort->getName() << " to component " << entityName << std::endl;
      pComponent->init_addPort(pPort);
    }

    pComponent->init_done();

    return pComponent;
  }

  virtual antlrcpp::Any visitPort_clause(vhdlParser::Port_clauseContext *ctx) override {
    std::vector<VHDLPort *> result;
    auto portCtxs = ctx->port_list()->interface_port_list()->interface_port_declaration();

    for(auto &portCtx: portCtxs)
    {
      auto ports = visit(portCtx).as<std::vector<VHDLPort *>>();
      result.insert(std::end(result), std::begin(ports), std::end(ports));
    }
    return result;
  }

  virtual antlrcpp::Any visitInterface_port_declaration(vhdlParser::Interface_port_declarationContext *ctx) override {
    std::vector<VHDLPort *> ports;

    for(auto &identctx: ctx->identifier_list()->identifier())
    {
      auto pPort = new VHDLPort(visit(identctx).as<std::string>());
      Direction dir = visit(ctx->signal_mode());
      if(dir == DIR_INVALID)
      {
        throw antlr4::RuntimeException("Parsing of signal modes other than IN, OUT and INOUT is not implemented");
      }

      pPort->setDirection(dir);
      // TODO: also parse and store subtype_indication

      ports.push_back(pPort);
    }
    return ports;
  }

  virtual antlrcpp::Any visitSignal_mode(vhdlParser::Signal_modeContext *ctx) override {
    return directionFromSignalMode(ctx->getText());
  }

  virtual antlrcpp::Any visitIdentifier(vhdlParser::IdentifierContext *ctx) override {
    return ctx->getText();
  }

  virtual antlrcpp::Any visitName(vhdlParser::NameContext *ctx) override {
    return ctx->getText();
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

  for (auto& kv: visitor.m_entityStore)
  {
    std::cout << "Entity " << kv.first << " is at " << kv.second << std::endl;
  }

  for (auto& kv: visitor.m_architectureStore)
  {
    std::cout << "Architecture " << kv.first << " of entity " << kv.second->getEntity()->getName() << " is at " << kv.second << std::endl;
  }

  std::cout << tree->toStringTree(&parser) << std::endl;
}

