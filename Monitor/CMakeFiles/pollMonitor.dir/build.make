# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/SF_PollServer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/SF_PollServer

# Include any dependencies generated for this target.
include Monitor/CMakeFiles/pollMonitor.dir/depend.make

# Include the progress variables for this target.
include Monitor/CMakeFiles/pollMonitor.dir/progress.make

# Include the compile flags for this target's objects.
include Monitor/CMakeFiles/pollMonitor.dir/flags.make

Monitor/CMakeFiles/pollMonitor.dir/pollMonitor.cpp.o: Monitor/CMakeFiles/pollMonitor.dir/flags.make
Monitor/CMakeFiles/pollMonitor.dir/pollMonitor.cpp.o: Monitor/pollMonitor.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/SF_PollServer/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object Monitor/CMakeFiles/pollMonitor.dir/pollMonitor.cpp.o"
	cd /home/SF_PollServer/Monitor && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/pollMonitor.dir/pollMonitor.cpp.o -c /home/SF_PollServer/Monitor/pollMonitor.cpp

Monitor/CMakeFiles/pollMonitor.dir/pollMonitor.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/pollMonitor.dir/pollMonitor.cpp.i"
	cd /home/SF_PollServer/Monitor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/SF_PollServer/Monitor/pollMonitor.cpp > CMakeFiles/pollMonitor.dir/pollMonitor.cpp.i

Monitor/CMakeFiles/pollMonitor.dir/pollMonitor.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/pollMonitor.dir/pollMonitor.cpp.s"
	cd /home/SF_PollServer/Monitor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/SF_PollServer/Monitor/pollMonitor.cpp -o CMakeFiles/pollMonitor.dir/pollMonitor.cpp.s

# Object files for target pollMonitor
pollMonitor_OBJECTS = \
"CMakeFiles/pollMonitor.dir/pollMonitor.cpp.o"

# External object files for target pollMonitor
pollMonitor_EXTERNAL_OBJECTS =

pollMonitor: Monitor/CMakeFiles/pollMonitor.dir/pollMonitor.cpp.o
pollMonitor: Monitor/CMakeFiles/pollMonitor.dir/build.make
pollMonitor: /usr/lib64/libbase.a
pollMonitor: Monitor/CMakeFiles/pollMonitor.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/SF_PollServer/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../pollMonitor"
	cd /home/SF_PollServer/Monitor && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/pollMonitor.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Monitor/CMakeFiles/pollMonitor.dir/build: pollMonitor

.PHONY : Monitor/CMakeFiles/pollMonitor.dir/build

Monitor/CMakeFiles/pollMonitor.dir/clean:
	cd /home/SF_PollServer/Monitor && $(CMAKE_COMMAND) -P CMakeFiles/pollMonitor.dir/cmake_clean.cmake
.PHONY : Monitor/CMakeFiles/pollMonitor.dir/clean

Monitor/CMakeFiles/pollMonitor.dir/depend:
	cd /home/SF_PollServer && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/SF_PollServer /home/SF_PollServer/Monitor /home/SF_PollServer /home/SF_PollServer/Monitor /home/SF_PollServer/Monitor/CMakeFiles/pollMonitor.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Monitor/CMakeFiles/pollMonitor.dir/depend
