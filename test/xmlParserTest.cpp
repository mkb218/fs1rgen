#include "xmlParser.hpp"
#include <iostream>
#include <fstream>
#include <string>

#define BUFSIZE 1024*1024

using namespace::std;

int main() {
  ifstream input("xmlParserTest/goodinput.xml", ifstream::in);
  string xmlData;
  char buffer[BUFSIZE];
  while (input.good()) {
    input.getline(buffer, BUFSIZE);
    xmlData.append(buffer);
  }

  try {
    XmlParser parser;
    parser.parse(xmlData);
  } catch (exception & e) {
    cerr << "good input failed: " << e.what << endl;
    return 0;
  }
}
