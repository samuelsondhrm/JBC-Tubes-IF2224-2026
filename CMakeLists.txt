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

add_library(arion_semantic_lib STATIC
    src/ms3-semantic/builder/ASTBuilder.cpp
    src/ms3-semantic/symtable/SymbolTable.cpp
    src/ms3-semantic/semantic/SemanticAnalyzer.cpp
    src/ms3-semantic/semantic/visit_declarations.cpp
    src/ms3-semantic/semantic/visit_statements.cpp
    src/ms3-semantic/semantic/visit_expressions.cpp
    src/ms3-semantic/type/TypeChecker.cpp
    src/ms3-semantic/printer/DecoratedASTPrinter.cpp
)
target_include_directories(arion_semantic_lib PUBLIC src/ms3-semantic)
target_link_libraries(arion_semantic_lib PUBLIC arion_parser_lib)

add_executable(arion-semantic src/ms3-semantic/main.cpp)
target_link_libraries(arion-semantic PRIVATE arion_semantic_lib)

add_library(arion_interpreter_lib STATIC
    src/ms4-interpreter/icg/ICGenerator.cpp
    src/ms4-interpreter/icg/ICGen_Expr.cpp
    src/ms4-interpreter/icg/ICGen_Statements.cpp
    src/ms4-interpreter/icg/ICGen_Subprograms.cpp
    src/ms4-interpreter/interpreter/StackMachine.cpp
    src/ms4-interpreter/interpreter/interpreter.cpp
)
target_include_directories(arion_interpreter_lib PUBLIC src/ms4-interpreter/interpreter src/ms4-interpreter/icg)
target_link_libraries(arion_interpreter_lib PUBLIC arion_semantic_lib)

add_executable(arion-interpreter src/ms4-interpreter/main.cpp)
target_link_libraries(arion-interpreter PRIVATE arion_interpreter_lib)



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
add_diff_test(ms2-16-comments    arion-parser ${MS2_IN}/test-16-comments.txt    ${MS2_OUT}/output-16-comments.txt)


set(MS2_ERR ${CMAKE_CURRENT_SOURCE_DIR}/test/milestone-2/expected-errors)

function(add_error_test name exe input expected_errors)
    add_test(
        NAME ${name}
        COMMAND ${CMAKE_COMMAND}
            -D EXE=$<TARGET_FILE:${exe}>
            -D INPUT=${input}
            -D EXPECTED_ERRORS=${expected_errors}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/RunErrorTest.cmake
    )
endfunction()

add_error_test(ms2-err-17 arion-parser ${MS2_IN}/test-17-err-missing-semi.txt ${MS2_ERR}/err-17-missing-semi.txt)
add_error_test(ms2-err-18 arion-parser ${MS2_IN}/test-18-err-wrong-token.txt  ${MS2_ERR}/err-18-wrong-token.txt)
add_error_test(ms2-err-19 arion-parser ${MS2_IN}/test-19-err-multiple.txt     ${MS2_ERR}/err-19-multiple.txt)
add_error_test(ms2-err-20 arion-parser ${MS2_IN}/test-20-err-misspelled.txt   ${MS2_ERR}/err-20-misspelled.txt)
add_error_test(ms2-err-21 arion-parser ${MS2_IN}/test-21-err-bad-for.txt       ${MS2_ERR}/err-21-bad-for.txt)



set(MS3_OUT ${CMAKE_CURRENT_SOURCE_DIR}/test/milestone-3/outputs)
set(MS3_IN  ${CMAKE_CURRENT_SOURCE_DIR}/test/milestone-3)
set(MS3_ERR ${CMAKE_CURRENT_SOURCE_DIR}/test/milestone-3/expected-errors)

add_diff_test(ms3-01minimal        arion-semantic ${MS3_IN}/test-01-minimal.txt      ${MS3_OUT}/output-test-01-minimal.txt)
add_diff_test(ms3-02alltypes        arion-semantic ${MS3_IN}/test-02-alltypes.txt     ${MS3_OUT}/output-test-02-alltypes.txt)
add_diff_test(ms3-03procfunc       arion-semantic ${MS3_IN}/test-03-procfunc.txt     ${MS3_OUT}/output-test-03-procfunc.txt)
add_diff_test(ms3-04controlflow     arion-semantic ${MS3_IN}/test-04-controlflow.txt ${MS3_OUT}/output-test-04-controlflow.txt)
add_diff_test(ms3-05nestedscope     arion-semantic ${MS3_IN}/test-05-nested-scope.txt ${MS3_OUT}/output-test-05-nested-scope.txt)
add_diff_test(ms3-06assigncompat    arion-semantic ${MS3_IN}/test-06-assign-compat.txt ${MS3_OUT}/output-test-06-assign-compat.txt)
add_error_test(ms3-err-07 arion-semantic ${MS3_IN}/test-07-err-redeclare.txt        ${MS3_ERR}/err-07-err-redeclare.txt)
add_error_test(ms3-err-08 arion-semantic ${MS3_IN}/test-08-err-undeclared.txt       ${MS3_ERR}/err-08-err-undeclared.txt)
add_error_test(ms3-err-09 arion-semantic ${MS3_IN}/test-09-err-assign-const.txt     ${MS3_ERR}/err-09-err-assign-const.txt)
add_error_test(ms3-err-10 arion-semantic ${MS3_IN}/test-10-err-type-mismatch.txt   ${MS3_ERR}/err-10-err-type-mismatch.txt)
add_error_test(ms3-err-11 arion-semantic ${MS3_IN}/test-11-err-nonbool-cond.txt    ${MS3_ERR}/err-11-err-nonbool-cond.txt)
add_error_test(ms3-err-12 arion-semantic ${MS3_IN}/test-12-err-nonarray-index.txt   ${MS3_ERR}/err-12-err-nonarray-index.txt)
add_error_test(ms3-err-13 arion-semantic ${MS3_IN}/test-13-err-nonrecord-access.txt ${MS3_ERR}/err-13-err-nonrecord-access.txt)
add_error_test(ms3-err-14 arion-semantic ${MS3_IN}/test-14-err-wrong-args.txt      ${MS3_ERR}/err-14-err-wrong-args.txt)
add_error_test(ms3-err-15 arion-semantic ${MS3_IN}/test-15-err-for-nonint.txt       ${MS3_ERR}/err-15-err-for-nonint.txt)
add_error_test(ms3-err-16 arion-semantic ${MS3_IN}/test-16-err-case-type.txt        ${MS3_ERR}/err-16-err-case-type.txt)
