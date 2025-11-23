#pragma once
#include <string>
#include <vector>
extern int CONFIG_INDENTATION_WIDTH;
class CodeDumper // Dumps code with indentation management
{
private:
  std::string lines;
  std::string current_indentation = "";

public:
  void IncIndent() // Increase indentation
  {
    for (int i = 0; i < CONFIG_INDENTATION_WIDTH; i++)
      current_indentation.push_back(' ');
  }
  void DecIndent() // Decrease indentation
  {
    for (int i = 0; !current_indentation.empty() && i < CONFIG_INDENTATION_WIDTH; i++)
      current_indentation.pop_back();
  }
  void NewLine() // Add a new line with current indentation
  {
    lines.push_back('\n');
    lines += current_indentation;
  }
  void Append(const std::string &str) // Append a string to the current line
  {
    lines += str;
  }
  std::string Dump() const // Get the dumped code
  {
    return lines;
  }
  void OpenScope() // Open a new scope with increased indentation
  {
    NewLine();
    Append("{");
    IncIndent();
    NewLine();
  }
  void CloseScope() // Close the current scope with decreased indentation
  {
    DecIndent();
    NewLine();
    Append("}");
    NewLine();
  }
};