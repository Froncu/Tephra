include(${CMAKE_CURRENT_LIST_DIR}/tephraTargets.cmake)

function (tephra_enable TARGET_NAME)
   if (NOT TARGET ${TARGET_NAME})
      message(FATAL_ERROR "Unknown target '${TARGET_NAME}'")
   endif ()

   set_property(TARGET ${TARGET_NAME} PROPERTY EXPORT_COMPILE_COMMANDS ON)
   add_custom_command(TARGET ${TARGET_NAME} PRE_LINK
      COMMAND tephra::tephra WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Running Tephra reflection" VERBATIM)
endfunction ()