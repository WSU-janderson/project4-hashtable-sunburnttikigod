cmake_minimum_required(VERSION 4.0)
project(hashtable)

set(CMAKE_CXX_STANDARD 20)

add_executable(HashTableDebug
        HashTableDebug.cpp
        HashTable.cpp
        HashTable.h
)

add_executable(HashTableTests
        HashTableTests.cpp
        HashTable.cpp
        HashTable.h
)

# Make SequenceDebug the default startup target
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HashTableDebug)