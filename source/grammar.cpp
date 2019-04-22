#include <lars/parser/grammar.h>
#include <lars/parser/interpreter.h>
#include <lars/iterators.h>

using namespace lars::peg;

namespace {
  
  /**  alternative to `std::get` that works on iOS < 11 */
  template <class T, class V> const T &pget(const V &v){
    if (auto r = std::get_if<T>(&v)) {
      return *r;
    } else {
      throw std::runtime_error("corrupted grammar node");
    }
  }
  
}

std::ostream & lars::peg::operator<<(std::ostream &stream, const GrammarNode &node){
  using Symbol = lars::peg::GrammarNode::Symbol;
  
  switch (node.symbol) {
    case GrammarNode::Symbol::WORD: {
      stream << "'" << pget<std::string>(node.data) << "'";
      break;
    }
      
    case GrammarNode::Symbol::ANY: {
      stream << ".";
      break;
    }
    case Symbol::RANGE:{
      auto &v = pget<std::array<lars::peg::Letter, 2>>(node.data);
      stream << "[" << v[0] << "-" << v[1] << "]";
      break;
    }
      
    case Symbol::SEQUENCE:{
      const auto &data = pget<std::vector<GrammarNode::Shared>>(node.data);
      stream << "(";
      for (auto [i,n]: lars::enumerate(data)) {
        stream << *n << (i + 1 == data.size() ? "" : " ");
      }
      stream << ")";
      break;
    }
      
    case Symbol::CHOICE:{
      stream << "(";
      const auto &data = pget<std::vector<GrammarNode::Shared>>(node.data);
      for (auto [i,n]: lars::enumerate(data)) {
        stream << *n << (i + 1 == data.size() ? "" : " | ");
      }
      stream << ")";
      break;
    }
      
    case Symbol::ZERO_OR_MORE:{
      const auto &data = pget<GrammarNode::Shared>(node.data);
      stream << *data << "*";
      break;
    }
      
    case Symbol::ONE_OR_MORE:{
      const auto &data = pget<GrammarNode::Shared>(node.data);
      stream << *data << "+";
      break;
    }
      
    case GrammarNode::Symbol::OPTIONAL: {
      stream << *pget<GrammarNode::Shared>(node.data) << "?";
      break;
    }
      
    case GrammarNode::Symbol::ALSO: {
      stream << "&" << *pget<GrammarNode::Shared>(node.data);
      break;
    }
      
    case GrammarNode::Symbol::NOT: {
      stream << "!" << *pget<GrammarNode::Shared>(node.data);
      break;
    }
      
    case GrammarNode::Symbol::EMPTY: {
      stream << "''";
      break;
    }
      
    case GrammarNode::Symbol::ERROR: {
      stream << "[]";
      break;
    }
      
    case GrammarNode::Symbol::RULE: {
      auto rule = pget<std::shared_ptr<Rule>>(node.data);
      stream << rule->name;
      break;
    }
            
    case GrammarNode::Symbol::WEAK_RULE: {
      if (auto rule = pget<std::weak_ptr<Rule>>(node.data).lock()) {
        stream << rule->name;
      } else {
        stream << "<DeletedRule>";
      }
      break;
    }
      
    case GrammarNode::Symbol::END_OF_FILE: {
      stream << "<EOF>";
      break;
    }
      
    case GrammarNode::Symbol::FILTER: {
      stream << "<Filter>";
      break;
    }
  }
  
  return stream;
}
