#pragma once
#include <string>
#include <vector>
extern int CONFIG_INDENTATION_WIDTH;
class CodeDumper
{
private:
  std::vector<std::string> lines;
  std::string current_indentation = "";

public:
  CodeDumper()
  {
    lines.push_back(""); // Start with an empty line
  }
  void IncIndent()
  {
    for (int i = 0; i < CONFIG_INDENTATION_WIDTH; i++)
      current_indentation.push_back(' ');
  }
  void DecIndent()
  {
    for (int i = 0; !current_indentation.empty() && i < CONFIG_INDENTATION_WIDTH; i++)
      current_indentation.pop_back();
  }
  void NewLine()
  {
    lines.push_back(current_indentation);
  }
  void Append(const std::string &str)
  {
    lines.back() += str;
  }
  std::string Dump()
  {
    std::string res;
    for (const auto &line : lines)
    {
      res += line + "\n";
    }
    return res;
  }
};