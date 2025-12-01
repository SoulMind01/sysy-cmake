#pragma once
#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <fstream>

#include "../src/lib/httplib.h"

const std::unordered_set<std::string> allowed_ips = {
    "10.0.0.44",
    "10.0.0.217",
    "10.0.0.94"};

bool is_allowed_origin(const std::string &origin);

void handle_result(const httplib::Request &req, httplib::Response &res);

void start_server();

std::string read_file(const std::string &path);

void run_compiler(
    std::string mode = "-koopa",
    std::string input_file_path = "./debug/hello.cpp",
    std::string output_file_path = "./debug/hello.koopa");