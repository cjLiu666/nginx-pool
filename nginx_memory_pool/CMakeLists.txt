cmake_minimum_required(VERSION 3.0.0)
project(nginx_mem_pool)
add_executable (nginx_mem_pool test.cc ngx_palloc.cc)