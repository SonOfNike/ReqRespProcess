cmake_minimum_required(VERSION 3.10.0)
project(ReqRespProcess VERSION 0.1.0 LANGUAGES C CXX)

find_package(OpenSSL REQUIRED)

# Path to the websocketpp library
set(WEBSOCKETPP_PATH /home/git_repos/websocketpp)

add_executable(ReqRespProcess main.cpp
                         MDConnector.cpp
                         ShmemManager.cpp
                         RespProcessor.cpp
                         ../Utils/SymbolIDManager.cpp
                         ../Utils/simdjson/simdjson.cpp)

target_link_libraries(ReqRespProcess PRIVATE OpenSSL::SSL OpenSSL::Crypto)
target_include_directories(ReqRespProcess PUBLIC
        ${WEBSOCKETPP_PATH}
    )

include(CTest)
enable_testing()

set_property(TARGET ReqRespProcess PROPERTY CXX_STANDARD 17)