cmake_minimum_required (VERSION 3.10)
project(pam-example)

link_libraries(pam)
link_libraries(pam_misc)

add_subdirectory(auth/interactive)
add_subdirectory(auth/interactive-custom)
add_subdirectory(auth/non-interactive)

install(DIRECTORY etc/pam.d DESTINATION /etc)

