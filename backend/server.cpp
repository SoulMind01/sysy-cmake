#include "server.h"

bool is_allowed_origin(const std::string &origin)
{
  std::regex local_pattern(R"(http://localhost(:\d+)?$)");
  std::regex local_network_pattern(R"(http://10.0.0.44(:\d+)?$)");
  return std::regex_match(origin, local_pattern) || std::regex_match(origin, local_network_pattern);
}

void handle_result(const httplib::Request &req, httplib::Response &res)
{
  std::string body = read_file("./debug/hello.cpp");
  res.set_content(body, "text/plain; charset=utf-8");
  std::string origin = req.get_header_value("Origin");
  if (is_allowed_origin(origin))
  {
    printf("Origin: %s\n", origin.c_str());
    res.set_header("Access-Control-Allow-Origin", origin);
  }
}

void start_server()
{
  httplib::Server svr;
  svr.Get("/result", handle_result);
  svr.listen("0.0.0.0", 8080);
}

httplib::Server svr;
void run_compiler(
    std::string mode = "-koopa",
    std::string input_file_path = "./debug/hello.cpp",
    std::string output_file_path = "./debug/hello.koopa")
{
  std::string command = "./build/compiler " + mode + " " + input_file_path + " -o " + output_file_path;
  system(command.c_str());
}

std::string read_file(const std::string &path)
{
  std::ifstream ifs(path, std::ios::in | std::ios::binary);
  if (!ifs)
  {
    printf("Failed to open file: %s\n", path.c_str());
    assert(false);
  }
  std::ostringstream ss;
  ss << ifs.rdbuf();
  return ss.str();
}

int main()
{
  printf("Starting server on port 8080...\n");
  start_server();
  // httplib::Server svr;
  // svr.Get("/result", handle_result);
  // svr.listen("0.0.0.0", 8080);
  return 0;
}