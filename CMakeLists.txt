cmake_minimum_required(VERSION 3.6)

# プロジェクト名
project(sfinder-cpp)

# 出力先
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# サブディレクトリの登録
add_subdirectory(googletest)
add_subdirectory(src)
add_subdirectory(test)

# ターゲットの設定
set_target_properties(sample PROPERTIES FOLDER library)
set_target_properties(test PROPERTIES FOLDER executable)
