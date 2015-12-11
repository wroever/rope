macro (unit_test name)
    add_test(${name}_test
    ${CMAKE_CURRENT_BINARY_DIR}/${name}_test)
    add_executable(${name}_test ${name}.cpp)
    target_link_libraries(${name}_test ${name} UnitTest++)
endmacro (unit_test)

unit_test(proj)
