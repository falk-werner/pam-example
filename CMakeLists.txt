cmake_minimum_required (VERSION 3.10)
project(pam-example)

link_libraries(pam)
link_libraries(pam_misc)

add_executable(auth-interactive src/app/auth-interactive.c)

