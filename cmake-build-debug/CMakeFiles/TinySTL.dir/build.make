# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2020.2.4\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2020.2.4\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\Clion\TinySTL

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\Clion\TinySTL\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/TinySTL.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/TinySTL.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/TinySTL.dir/flags.make

CMakeFiles/TinySTL.dir/main.cpp.obj: CMakeFiles/TinySTL.dir/flags.make
CMakeFiles/TinySTL.dir/main.cpp.obj: CMakeFiles/TinySTL.dir/includes_CXX.rsp
CMakeFiles/TinySTL.dir/main.cpp.obj: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\Clion\TinySTL\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/TinySTL.dir/main.cpp.obj"
	C:\PROGRA~1\MinGW\bin\G__~1.EXE  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\TinySTL.dir\main.cpp.obj -c D:\Clion\TinySTL\main.cpp

CMakeFiles/TinySTL.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TinySTL.dir/main.cpp.i"
	C:\PROGRA~1\MinGW\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\Clion\TinySTL\main.cpp > CMakeFiles\TinySTL.dir\main.cpp.i

CMakeFiles/TinySTL.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TinySTL.dir/main.cpp.s"
	C:\PROGRA~1\MinGW\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\Clion\TinySTL\main.cpp -o CMakeFiles\TinySTL.dir\main.cpp.s

CMakeFiles/TinySTL.dir/Detail/_alloc.cpp.obj: CMakeFiles/TinySTL.dir/flags.make
CMakeFiles/TinySTL.dir/Detail/_alloc.cpp.obj: CMakeFiles/TinySTL.dir/includes_CXX.rsp
CMakeFiles/TinySTL.dir/Detail/_alloc.cpp.obj: ../Detail/_alloc.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\Clion\TinySTL\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/TinySTL.dir/Detail/_alloc.cpp.obj"
	C:\PROGRA~1\MinGW\bin\G__~1.EXE  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\TinySTL.dir\Detail\_alloc.cpp.obj -c D:\Clion\TinySTL\Detail\_alloc.cpp

CMakeFiles/TinySTL.dir/Detail/_alloc.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TinySTL.dir/Detail/_alloc.cpp.i"
	C:\PROGRA~1\MinGW\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\Clion\TinySTL\Detail\_alloc.cpp > CMakeFiles\TinySTL.dir\Detail\_alloc.cpp.i

CMakeFiles/TinySTL.dir/Detail/_alloc.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TinySTL.dir/Detail/_alloc.cpp.s"
	C:\PROGRA~1\MinGW\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\Clion\TinySTL\Detail\_alloc.cpp -o CMakeFiles\TinySTL.dir\Detail\_alloc.cpp.s

# Object files for target TinySTL
TinySTL_OBJECTS = \
"CMakeFiles/TinySTL.dir/main.cpp.obj" \
"CMakeFiles/TinySTL.dir/Detail/_alloc.cpp.obj"

# External object files for target TinySTL
TinySTL_EXTERNAL_OBJECTS =

TinySTL.exe: CMakeFiles/TinySTL.dir/main.cpp.obj
TinySTL.exe: CMakeFiles/TinySTL.dir/Detail/_alloc.cpp.obj
TinySTL.exe: CMakeFiles/TinySTL.dir/build.make
TinySTL.exe: CMakeFiles/TinySTL.dir/linklibs.rsp
TinySTL.exe: CMakeFiles/TinySTL.dir/objects1.rsp
TinySTL.exe: CMakeFiles/TinySTL.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\Clion\TinySTL\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable TinySTL.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\TinySTL.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/TinySTL.dir/build: TinySTL.exe

.PHONY : CMakeFiles/TinySTL.dir/build

CMakeFiles/TinySTL.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\TinySTL.dir\cmake_clean.cmake
.PHONY : CMakeFiles/TinySTL.dir/clean

CMakeFiles/TinySTL.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\Clion\TinySTL D:\Clion\TinySTL D:\Clion\TinySTL\cmake-build-debug D:\Clion\TinySTL\cmake-build-debug D:\Clion\TinySTL\cmake-build-debug\CMakeFiles\TinySTL.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/TinySTL.dir/depend
