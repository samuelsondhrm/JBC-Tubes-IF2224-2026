cmake_minimum_required(VERSION 3.16)
project(ArionCompiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic)

enable_testing()


add_library(arion_lexer_lib STATIC
    src/ms1-lexer/token/token.cpp
    src/ms1-lexer/lexer/symbolTable.cpp
    src/ms1-lexer/lexer/lexer.cpp
    src/ms1-lexer/dfa/dfa.cpp
    src/ms1-lexer/util/file.cpp
)
target_include_directories(arion_lexer_lib PUBLIC src/ms1-lexer)


add_executable(arion-lexer src/ms1-lexer/main.cpp)
target_link_libraries(arion-lexer PRIVATE arion_lexer_lib)


add_library(arion_parser_lib STATIC
    src/ms2-parser/ParseNode.cpp
    src/ms2-parser/TreePrinter.cpp
    src/ms2-parser/Parser.cpp
    src/ms2-parser/parser_toplevel.cpp
    src/ms2-parser/parser_declarations.cpp
    src/ms2-parser/parser_statements.cpp
    src/ms2-parser/parser_expressions.cpp
    src/ms2-parser/parser_subprograms.cpp
)
target_include_directories(arion_parser_lib PUBLIC src/ms2-parser)
target_link_libraries(arion_parser_lib PUBLIC arion_lexer_lib)

add_executable(arion-parser src/ms2-parser/main.cpp)
target_link_libraries(arion-parser PRIVATE arion_parser_lib)


function(add_diff_test name exe input expected)
    add_test(
        NAME ${name}
        COMMAND ${CMAKE_COMMAND}
            -D EXE=$<TARGET_FILE:${exe}>
            -D INPUT=${input}
            -D EXPECTED=${expected}
            -D OUTPUT=${CMAKE_CURRENT_BINARY_DIR}/${name}.out
            -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/RunDiffTest.cmake
    )
endfunction()


set(MS1_OUT ${CMAKE_CURRENT_SOURCE_DIR}/test/milestone-1/outputs)
set(MS1_IN  ${CMAKE_CURRENT_SOURCE_DIR}/test/milestone-1)

add_diff_test(ms1-valid          arion-lexer ${MS1_IN}/test-valid.txt             ${MS1_OUT}/output-valid.txt)
add_diff_test(ms1-invalid         arion-lexer ${MS1_IN}/test-invalid.txt          ${MS1_OUT}/output-invalid.txt)
add_diff_test(ms1-edge            arion-lexer ${MS1_IN}/test-edgecases.txt       ${MS1_OUT}/output-edgecases.txt)
add_diff_test(ms1-caseinsensitive arion-lexer ${MS1_IN}/test-caseinsensitive.txt ${MS1_OUT}/output-caseinsensitive.txt)
add_diff_test(ms1-literaltypes    arion-lexer ${MS1_IN}/test-literaltypes.txt     ${MS1_OUT}/output-literaltypes.txt)
add_diff_test(ms1-eql             arion-lexer ${MS1_IN}/test-eql.txt              ${MS1_OUT}/output-eql.txt)
add_diff_test(ms1-kwidboundary    arion-lexer ${MS1_IN}/test-kwidboundary.txt    ${MS1_OUT}/output-kwidboundary.txt)
add_diff_test(ms1-commentambig   arion-lexer ${MS1_IN}/test-commentambiguity.txt  ${MS1_OUT}/output-commentambiguity.txt)


set(MS2_OUT ${CMAKE_CURRENT_SOURCE_DIR}/test/milestone-2/outputs)
set(MS2_IN  ${CMAKE_CURRENT_SOURCE_DIR}/test/milestone-2)

add_diff_test(ms2-01-minimal     arion-parser ${MS2_IN}/test-01-minimal.txt     ${MS2_OUT}/output-01-minimal.txt)
add_diff_test(ms2-02-vars        arion-parser ${MS2_IN}/test-02-vars.txt        ${MS2_OUT}/output-02-vars.txt)
add_diff_test(ms2-03-const       arion-parser ${MS2_IN}/test-03-const.txt       ${MS2_OUT}/output-03-const.txt)
add_diff_test(ms2-04-types       arion-parser ${MS2_IN}/test-04-types.txt       ${MS2_OUT}/output-04-types.txt)
add_diff_test(ms2-05-assign      arion-parser ${MS2_IN}/test-05-assign.txt      ${MS2_OUT}/output-05-assign.txt)
add_diff_test(ms2-06-ifelse      arion-parser ${MS2_IN}/test-06-ifelse.txt      ${MS2_OUT}/output-06-ifelse.txt)
add_diff_test(ms2-07-loops       arion-parser ${MS2_IN}/test-07-loops.txt       ${MS2_OUT}/output-07-loops.txt)
add_diff_test(ms2-08-procedure   arion-parser ${MS2_IN}/test-08-procedure.txt   ${MS2_OUT}/output-08-procedure.txt)
add_diff_test(ms2-09-function    arion-parser ${MS2_IN}/test-09-function.txt    ${MS2_OUT}/output-09-function.txt)
add_diff_test(ms2-10-expressions arion-parser ${MS2_IN}/test-10-expressions.txt ${MS2_OUT}/output-10-expressions.txt)
add_diff_test(ms2-11-arrrecord   arion-parser ${MS2_IN}/test-11-arrrecord.txt   ${MS2_OUT}/output-11-arrrecord.txt)
add_diff_test(ms2-12-case        arion-parser ${MS2_IN}/test-12-case.txt        ${MS2_OUT}/output-12-case.txt)
add_diff_test(ms2-13-proccall    arion-parser ${MS2_IN}/test-13-proccall.txt    ${MS2_OUT}/output-13-proccall.txt)
add_diff_test(ms2-14-complex     arion-parser ${MS2_IN}/test-14-complex.txt     ${MS2_OUT}/output-14-complex.txt)
add_diff_test(ms2-15-alldecl     arion-parser ${MS2_IN}/test-15-alldecl.txt     ${MS2_OUT}/output-15-alldecl.txt)