#pragma once
#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <fstream>

#include "../src/lib/httplib.h"
bool is_allowed_origin(const std::string &origin);

void handle_result(const httplib::Request &req, httplib::Response &res);

void start_server();
std::string read_file(const std::string &path);