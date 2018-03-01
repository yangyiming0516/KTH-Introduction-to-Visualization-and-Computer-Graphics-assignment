
add_custom_target(glfw-docs ALL ${DOXYGEN_EXECUTABLE}
                  WORKING_DIRECTORY ${GLFW_BINARY_DIR}/docs
                  COMMENT "Generating HTML documentation" VERBATIM)

