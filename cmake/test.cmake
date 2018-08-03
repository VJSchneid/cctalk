set(TEST_TARGETS "" CACHE INTERNAL "TEST_TARGET")

function(add_test_executable target)
    set(TEST_TARGETS "${TEST_TARGETS};${target}" CACHE INTERNAL "TEST_TARGET")
endfunction()

function(add_all_test_targets)
    add_test_target()
    add_valgrind_test_target()
endfunction()

function(add_test_target)
    foreach(TARGET ${TEST_TARGETS})
        set(TEST_COMMANDS "${TEST_COMMANDS};COMMAND;$<TARGET_FILE:${TARGET}>")
    endforeach()

    add_custom_target(tests
        ${TEST_COMMANDS}
        DEPENDS ${TEST_TARGETS}
    )
endfunction()

function(add_valgrind_test_target)
    foreach(TARGET ${TEST_TARGETS})
        set(TEST_COMMANDS ${TEST_COMMANDS};COMMAND;valgrind --error-exitcode=1 --leak-check=full --track-origins=yes $<TARGET_FILE:${TARGET}>)
    endforeach()

    add_custom_target(valgrind-tests
        ${TEST_COMMANDS}
        DEPENDS ${TEST_TARGETS}
    )
endfunction()
