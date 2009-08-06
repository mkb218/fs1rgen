#include "XmlParser.hpp"

auto_ptr<fs1rgen::Model> fs1rgen::XmlParser::getModel()  {
  return auto_ptr<Model>(new Model(m_model));
}
