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

#include "common.h"
#include "parse_vhdl.h"
#include "vhdl_architecture.h"
#include "vhdl_component.h"
#include "vhdl_file.h"
#include "vhdl_entity.h"
#include "vhdl_fragment.h"
#include "vhdl_instance.h"
#include "vhdl_port.h"
#include "vhdl_signal.h"

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

class VhdlConstructingVisitor: public vhdlBaseVisitor
{
public:
  VHDLFile::Mode m_mode;
  std::map<std::string, VHDLEntity *> m_entityMap;
  std::map<std::string, std::vector<VHDLComponent *>> m_entityLessComponents;

  VHDLArchitecture *m_pCurrentArchitecture;
  VHDLFile *m_pCurrentFile;

  VhdlConstructingVisitor(VHDLFile::Mode mode):
    vhdlBaseVisitor(),
    m_mode(mode)
  {
  }

private:
  typedef std::pair<std::string, std::string> stringpair;

  /* Return any parsed node as literal text */
  std::string getCurrentFragment(antlr4::ParserRuleContext *ctx)
  {
    auto start = ctx->getStart()->getStartIndex();
    auto stop = ctx->getStop()->getStopIndex();
    if(start < stop)
    {
      auto pStream = ctx->getStart()->getInputStream();
      return pStream->getText(antlr4::misc::Interval(start, stop));
    }
    else
    {
      return "";
    }
  }

  virtual antlrcpp::Any visitDesign_file(vhdlParser::Design_fileContext *ctx) override {
    VHDLFile *pVHDLFile = new VHDLFile(m_mode);
    m_pCurrentFile = pVHDLFile;
    for(auto &el: ctx->design_unit())
    {
      visit(el);
    }
    return pVHDLFile;
  }

  virtual antlrcpp::Any visitContext_clause(vhdlParser::Context_clauseContext *ctx) override {
    std::cout << "Visiting context clause...\n" << std::endl;
    if(ctx->context_item().size() > 0)
    {
      auto text = getCurrentFragment(ctx);
      g_assert(text != "");
      return new VHDLFragment(text);
    }
    else
    {
      return (VHDLFragment *)nullptr;
    }
  }

  virtual antlrcpp::Any visitDesign_unit(vhdlParser::Design_unitContext *ctx) override {

    /* Since only one context clause can be stored in a VHDLFile object, only
     * set it if it is a non-empty string */
    VHDLFragment *pContext = visit(ctx->context_clause());
    if(pContext)
    {
      m_pCurrentFile->setContext(pContext);
    }

    auto library_unit = visit(ctx->library_unit());

    if(library_unit.is<VHDLEntity *>())
    {
      if(m_pCurrentFile->getEntity())
      {
        throw antlr4::RuntimeException("Only one entity is supported per VHDL file");
      }
      m_pCurrentFile->setEntity(library_unit);
    }
    else if(library_unit.is<VHDLArchitecture *>())
    {
      m_pCurrentFile->addArchitecture(library_unit);
    }
    else
    {
      m_pCurrentFile->addOtherFragment(new VHDLFragment(getCurrentFragment(ctx)));
    }
    return nullptr;
  }

  virtual antlrcpp::Any visitLibrary_unit(vhdlParser::Library_unitContext *ctx) override {
    if(ctx->primary_unit())
    {
      return visit(ctx->primary_unit());
    }
    else
    {
      g_assert(ctx->secondary_unit());
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
      if(m_pCurrentFile->getMode() == VHDLFile::TEXT)
      {
        return new VHDLFragment(getCurrentFragment(ctx));
      }
      else
      {
        throw antlr4::RuntimeException("Parsing of primary units other than entity declarations is not implemented for files read in graphical mode");
      }
    }
  }

  virtual antlrcpp::Any visitSecondary_unit(vhdlParser::Secondary_unitContext *ctx) override {
    if(m_pCurrentFile->getMode() == VHDLFile::GRAPHICAL)
    {
      assert(ctx->architecture_body());
      return visit(ctx->architecture_body());
    }
    else
    {
      return new VHDLFragment(getCurrentFragment(ctx));
    }
  }

  virtual antlrcpp::Any visitEntity_declaration(vhdlParser::Entity_declarationContext *ctx) override {
    auto name = visit(ctx->identifier(0)).as<std::string>();
    auto pEntity = new VHDLEntity(name);

    auto pGenericsClauseCtx = ctx->entity_header()->generic_clause();
    if(pGenericsClauseCtx)
    {
      pEntity->init_addGenerics(visit(pGenericsClauseCtx));
    }

    auto pPortClauseCtx = ctx->entity_header()->port_clause();
    if(pPortClauseCtx)
    {
      std::vector<VHDLPort *> ports = visit(pPortClauseCtx);

      for(auto pPort: ports)
      {
        std::cout << "Adding port " << pPort->getName() << " to entity " << name << std::endl;
        pEntity->init_addPort(pPort);
      }
    }

    if(ctx->entity_declarative_part()->entity_declarative_item().size() > 0)
    {
      pEntity->init_setDeclarativePart(new VHDLFragment(getCurrentFragment(ctx->entity_declarative_part())));
    }

    m_entityMap[name] = pEntity;

    return pEntity;
  }

  virtual antlrcpp::Any visitEntity_header(vhdlParser::Entity_headerContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitArchitecture_body(vhdlParser::Architecture_bodyContext *ctx) override {
    auto archName = visit(ctx->identifier(0)).as<std::string>();
    auto entityName = visit(ctx->identifier(1)).as<std::string>();
    auto pArch = new VHDLArchitecture(archName);
    pArch->setEntity(m_entityMap.at(entityName));

    m_pCurrentArchitecture = pArch;
    visit(ctx->architecture_declarative_part());
    visit(ctx->architecture_statement_part());
    return pArch;
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
      else if(value.is<VHDLFragment *>())
      {
        std::cout << "Visited fragment: [" << value.as<VHDLFragment *>()->getText() << "]" << std::endl;
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
    std::string formalPart;
    if(ctx->formal_part())
    {
      formalPart = visit(ctx->formal_part()).as<std::string>();
    }
    else
    {
      formalPart = "";
    }
    std::string actualPart = visit(ctx->actual_part()).as<std::string>();
    return stringpair(formalPart, actualPart);
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
      /* Return any unknown VHDL as a VHDLFragment */
      return new VHDLFragment(getCurrentFragment(ctx));
    }
  }

  virtual antlrcpp::Any visitSubtype_indication(vhdlParser::Subtype_indicationContext *ctx) override {
    return VHDLType(getCurrentFragment(ctx));
  }

  virtual antlrcpp::Any visitSignal_declaration(vhdlParser::Signal_declarationContext *ctx) override {
    std::vector<VHDLSignal *> signals;

    for(auto &identctx: ctx->identifier_list()->identifier())
    {
      auto pSignal = new VHDLSignal(visit(identctx).as<std::string>());

      signals.push_back(pSignal);
    }
    auto type = visit(ctx->subtype_indication());
    std::string defaultValue;
    if(ctx->expression())
    {
      defaultValue = getCurrentFragment(ctx->expression());
    }
    for(auto &pSignal: signals)
    {
      pSignal->setType(type);
      pSignal->setDefaultValue(defaultValue);
      // TODO: also parse and store signal_kind
    }
    return signals;
  }

  virtual antlrcpp::Any visitGeneric_clause(vhdlParser::Generic_clauseContext *ctx) override {
    return new VHDLFragment(getCurrentFragment(ctx));
  }

  virtual antlrcpp::Any visitComponent_declaration(vhdlParser::Component_declarationContext *ctx) override {
    std::string entityName = visit(ctx->identifier(0));
    auto pComponent = new VHDLComponent(entityName);

    // Remember it so we can resolve all the entity references once we know all entities
    m_entityLessComponents[entityName].push_back(pComponent);

    if(ctx->generic_clause())
    {
      pComponent->init_addGenerics(visit(ctx->generic_clause()));
    }

    if(ctx->port_clause())
    {
      std::vector<VHDLPort *> ports = visit(ctx->port_clause());

      for(auto pPort: ports)
      {
        std::cout << "Adding port " << pPort->getName() << " to component " << entityName << std::endl;
        pComponent->init_addPort(pPort);
      }
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

      ports.push_back(pPort);
    }

    auto type = visit(ctx->subtype_indication());
    for(auto &pPort: ports)
    {
      pPort->setType(type);
      // TODO: also parse and store expression
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


VHDLFile *parseVHDL(std::istream &stream, VHDLFile::Mode mode)
{
  antlr4::ANTLRInputStream input(stream);
  vhdlLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  vhdlParser parser(&tokens);
  antlr4::tree::ParseTree *tree = parser.design_file();

  auto visitor = VhdlConstructingVisitor(mode);
  return visitor.visit(tree);
}

