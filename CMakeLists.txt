cmake_minimum_required(VERSION 3.16)
project(MarkdownToHTML VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable automatic compilation of Qt files
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

# Manual Qt6 configuration to bypass version compatibility issues
set(Qt6_DIR "C:/Qt/6.11.1/mingw_64/lib/cmake/Qt6")
set(CMAKE_PREFIX_PATH "C:/Qt/6.11.1/mingw_64" "C:/Qt/6.11.1/mingw_64/lib/cmake")

# Find Qt6 packages
find_package(Qt6 COMPONENTS Core Gui Widgets PrintSupport REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Define source files
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/MarkdownParser.cpp
    src/HTMLGenerator.cpp
    src/FileManager.cpp
)

# Define header files
set(HEADERS
    include/MainWindow.h
    include/MarkdownParser.h
    include/HTMLGenerator.h
    include/FileManager.h
)

# Define UI files
set(UIS
    ui/MainWindow.ui
)

# Source groups for IDEs
source_group("Header Files" FILES ${HEADERS})
source_group("Source Files" FILES ${SOURCES})
source_group("Form Files" FILES ${UIS})

# Add the executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UIS}
)

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::PrintSupport
)

# Set runtime output directories
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
