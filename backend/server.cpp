#include "server.h"

httplib::Server svr;

bool is_allowed_ip(const std::string &origin)
{
  return allowed_ips.find(origin) != allowed_ips.end();
}

void start_server()
{
  httplib::Server svr;
  svr.Get("/result", handle_result);
  svr.listen("0.0.0.0", 8080);
}

void run_compiler(
    std::string mode,
    std::string input_file_path,
    std::string output_file_path)
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

void handle_result(const httplib::Request &req, httplib::Response &res)
{
  run_compiler();
  std::string body = read_file("./debug/instructions.txt");
  res.set_content(body, "text/plain; charset=utf-8");
  printf("Remote addr: %s\n", req.remote_addr.c_str());
  if (is_allowed_ip(req.remote_addr))
  {
    res.set_header("Access-Control-Allow-Origin", req.get_header_value("Origin"));
  }
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