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

Direction directionFromSignalMode(const Glib::ustring &modeString)
{
  auto modeStringUpper = modeString.uppercase();

  if(modeStringUpper == "IN")
  {
    return DIR_IN;
  }
  else if(modeStringUpper == "OUT")
  {
    return DIR_OUT;
  }
  else if(modeStringUpper == "INOUT")
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
private:
  VHDLFile::Mode m_mode;
  std::map<const Glib::ustring, VHDLEntity *> m_entityMap;

  VHDLArchitecture *m_pCurrentArchitecture;
  VHDLFile *m_pCurrentFile;

public:
  VhdlConstructingVisitor(VHDLFile::Mode mode):
    vhdlBaseVisitor(),
    m_mode(mode),
    m_pCurrentArchitecture(nullptr),
    m_pCurrentFile(nullptr)
  {
  }

private:
  typedef std::pair<Glib::ustring, Glib::ustring> stringpair;

  /* Return any parsed node as literal text */
  Glib::ustring getCurrentFragment(antlr4::ParserRuleContext *ctx)
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
    auto pVHDLFile = std::make_unique<VHDLFile>(m_mode);
    m_pCurrentFile = pVHDLFile.get();
    for(auto &el: ctx->design_unit())
    {
      visit(el);
    }
    return std::move(pVHDLFile);
  }

  virtual antlrcpp::Any visitContext_clause(vhdlParser::Context_clauseContext *ctx) override {
    if(ctx->context_item().size() > 0)
    {
      auto text = getCurrentFragment(ctx);
      g_assert(text != "");
      return std::make_unique<VHDLFragment>(text);
    }
    else
    {
      return std::unique_ptr<VHDLFragment>(nullptr);
    }
  }

  virtual antlrcpp::Any visitDesign_unit(vhdlParser::Design_unitContext *ctx) override {

    /* Since only one context clause can be stored in a VHDLFile object, only
     * set it if it is a non-empty string */
    std::unique_ptr<VHDLFragment> pContext = std::move(visit(ctx->context_clause()));
    if(pContext)
    {
      m_pCurrentFile->setContext(std::move(pContext));
    }

    auto library_unit = visit(ctx->library_unit());

    if(library_unit.is<std::unique_ptr<VHDLEntity>>())
    {
      if(m_pCurrentFile->getEntity())
      {
        throw antlr4::RuntimeException("Only one entity is supported per VHDL file");
      }
      m_pCurrentFile->setEntity(std::move(library_unit));
    }
    else if(library_unit.is<std::unique_ptr<VHDLArchitecture>>())
    {
      m_pCurrentFile->addArchitecture(library_unit);
    }
    else
    {
      m_pCurrentFile->addOtherFragment(std::make_unique<VHDLFragment>(getCurrentFragment(ctx)));
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
        return std::make_unique<VHDLFragment>(getCurrentFragment(ctx));
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
      return std::make_unique<VHDLFragment>(getCurrentFragment(ctx));
    }
  }

  virtual antlrcpp::Any visitEntity_declaration(vhdlParser::Entity_declarationContext *ctx) override {
    auto name = visit(ctx->identifier(0)).as<Glib::ustring>();
    auto pEntity = std::make_unique<VHDLEntity>(name);

    auto pGenericsClauseCtx = ctx->entity_header()->generic_clause();
    if(pGenericsClauseCtx)
    {
      pEntity->init_addGenerics(std::move(visit(pGenericsClauseCtx)));
    }

    auto pPortClauseCtx = ctx->entity_header()->port_clause();
    if(pPortClauseCtx)
    {
      auto ports = std::move(visit(pPortClauseCtx).as<std::vector<std::unique_ptr<VHDLPort>>>());

      for(auto &pPort: ports)
      {
        std::cout << "Adding port " << pPort->getName() << " to entity " << name << std::endl;
        pEntity->init_addPort(std::move(pPort));
      }
    }

    if(ctx->entity_declarative_part()->entity_declarative_item().size() > 0)
    {
      pEntity->init_setDeclarativePart(std::make_unique<VHDLFragment>(getCurrentFragment(ctx->entity_declarative_part())));
    }

    m_entityMap[name] = pEntity.get();

    return std::move(pEntity);
  }

  virtual antlrcpp::Any visitEntity_header(vhdlParser::Entity_headerContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitArchitecture_body(vhdlParser::Architecture_bodyContext *ctx) override {
    auto archName = visit(ctx->identifier(0)).as<Glib::ustring>();
    auto entityName = visit(ctx->identifier(1)).as<Glib::ustring>();
    auto pArch = std::make_unique<VHDLArchitecture>(archName);
    pArch->setEntity(m_entityMap.at(entityName));

    m_pCurrentArchitecture = pArch.get();
    visit(ctx->architecture_declarative_part());
    visit(ctx->architecture_statement_part());
    return std::move(pArch);
  }

  virtual antlrcpp::Any visitArchitecture_declarative_part(vhdlParser::Architecture_declarative_partContext *ctx) override {
    for(auto &el: ctx->block_declarative_item())
    {
      auto value = visit(el);
      if(value.is<std::unique_ptr<VHDLComponent>>())
      {
        m_pCurrentArchitecture->init_addComponent(std::move(value));
      }
      else if(value.is<std::vector<std::unique_ptr<VHDLSignal>>>())
      {
        std::vector<std::unique_ptr<VHDLSignal>> signals = value;
        for(auto &pSignal: signals)
        {
          m_pCurrentArchitecture->init_addSignal(std::move(pSignal));
        }
      }
      else if(value.is<std::unique_ptr<VHDLFragment>>())
      {
        std::cout << "Visited fragment: [" << value.as<std::unique_ptr<VHDLFragment>>()->getText() << "]" << std::endl;
        assert(false);
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
    Glib::ustring instanceName = visit(ctx->label_colon()->identifier());
    Glib::ustring componentName = visit(ctx->instantiated_unit()->name());

    auto pInstance = std::make_unique<VHDLInstance>(instanceName, m_pCurrentArchitecture->findComponentByName(componentName));
    auto pRawInstance = pInstance.get();
    m_pCurrentArchitecture->init_addInstance(std::move(pInstance));

    std::vector<stringpair> port_maps = visit(ctx->port_map_aspect()->association_list());
    for(auto port_map: port_maps)
    {
      auto pPort = pRawInstance->getComponent()->findPortByName(port_map.first);

      auto signalNameUpper = port_map.second.uppercase();

      if(signalNameUpper != "OPEN")
      {
        auto pSignal = m_pCurrentArchitecture->findSignalByName(port_map.second);
        std::cout << "connecting signal " << port_map.second << " to port " << port_map.first << std::endl;
        pRawInstance->connectSignalToPort(pSignal, pPort);
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
    Glib::ustring formalPart;
    if(ctx->formal_part())
    {
      formalPart = visit(ctx->formal_part()).as<Glib::ustring>();
    }
    else
    {
      formalPart = "";
    }
    Glib::ustring actualPart = visit(ctx->actual_part()).as<Glib::ustring>();
    return stringpair(formalPart, actualPart);
  }

  virtual antlrcpp::Any visitFormal_part(vhdlParser::Formal_partContext *ctx) override {
    return Glib::ustring(ctx->getText());
  }

  virtual antlrcpp::Any visitActual_part(vhdlParser::Actual_partContext *ctx) override {
    return Glib::ustring(ctx->getText());
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
      return std::make_unique<VHDLFragment>(getCurrentFragment(ctx));
    }
  }

  virtual antlrcpp::Any visitSubtype_indication(vhdlParser::Subtype_indicationContext *ctx) override {
    return VHDLType(getCurrentFragment(ctx));
  }

  virtual antlrcpp::Any visitSignal_declaration(vhdlParser::Signal_declarationContext *ctx) override {
    std::vector<std::unique_ptr<VHDLSignal>> signals;

    for(auto &identctx: ctx->identifier_list()->identifier())
    {
      auto pSignal = std::make_unique<VHDLSignal>(visit(identctx).as<Glib::ustring>());

      signals.push_back(std::move(pSignal));
    }
    auto type = visit(ctx->subtype_indication());
    Glib::ustring defaultValue;
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
    return std::move(signals);
  }

  virtual antlrcpp::Any visitGeneric_clause(vhdlParser::Generic_clauseContext *ctx) override {
    return std::make_unique<VHDLFragment>(getCurrentFragment(ctx));
  }

  virtual antlrcpp::Any visitComponent_declaration(vhdlParser::Component_declarationContext *ctx) override {
    Glib::ustring entityName = visit(ctx->identifier(0));
    auto pComponent = std::make_unique<VHDLComponent>(entityName);

    if(ctx->generic_clause())
    {
      pComponent->init_addGenerics(std::move(visit(ctx->generic_clause())));
    }

    if(ctx->port_clause())
    {
      auto ports = std::move(visit(ctx->port_clause()).as<std::vector<std::unique_ptr<VHDLPort>>>());

      for(auto &pPort: ports)
      {
        std::cout << "Adding port " << pPort->getName() << " to component " << entityName << std::endl;
        pComponent->init_addPort(std::move(pPort));
      }
    }

    pComponent->init_done();

    return std::move(pComponent);
  }

  virtual antlrcpp::Any visitPort_clause(vhdlParser::Port_clauseContext *ctx) override {
    std::vector<std::unique_ptr<VHDLPort>> result;

    auto portCtxs = ctx->port_list()->interface_port_list()->interface_port_declaration();

    for(auto &portCtx: portCtxs)
    {
      auto ports = std::move(visit(portCtx).as<std::vector<std::unique_ptr<VHDLPort>>>());
      result.insert(std::end(result),
                    std::make_move_iterator(std::begin(ports)),
                    std::make_move_iterator(std::end(ports)));
    }
    return std::move(result);
  }

  virtual antlrcpp::Any visitInterface_port_declaration(vhdlParser::Interface_port_declarationContext *ctx) override {
    std::vector<std::unique_ptr<VHDLPort>> ports;

    for(auto &identctx: ctx->identifier_list()->identifier())
    {
      auto pPort = std::make_unique<VHDLPort>(visit(identctx).as<Glib::ustring>());
      Direction dir = visit(ctx->signal_mode());
      if(dir == DIR_INVALID)
      {
        throw antlr4::RuntimeException("Parsing of signal modes other than IN, OUT and INOUT is not implemented");
      }

      pPort->setDirection(dir);

      ports.push_back(std::move(pPort));
    }

    auto type = visit(ctx->subtype_indication());
    for(auto &pPort: ports)
    {
      pPort->setType(type);
      // TODO: also parse and store expression
    }

    return std::move(ports);
  }

  virtual antlrcpp::Any visitSignal_mode(vhdlParser::Signal_modeContext *ctx) override {
    return directionFromSignalMode(ctx->getText());
  }

  virtual antlrcpp::Any visitIdentifier(vhdlParser::IdentifierContext *ctx) override {
    return Glib::ustring(ctx->getText());
  }

  virtual antlrcpp::Any visitName(vhdlParser::NameContext *ctx) override {
    return Glib::ustring(ctx->getText());
  }
};


std::unique_ptr<VHDLFile> parseVHDL(std::istream &stream, VHDLFile::Mode mode)
{
  antlr4::ANTLRInputStream input(stream);
  vhdlLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  vhdlParser parser(&tokens);
  antlr4::tree::ParseTree *tree = parser.design_file();

  VhdlConstructingVisitor visitor(mode);
  return visitor.visit(tree);
}

