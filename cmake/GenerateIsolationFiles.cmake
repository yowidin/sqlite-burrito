include(CMakeParseArguments)

function(generate_isolation_files)
   set(option_args)

   set(single_value_args
      # Path to the generate-isolation-files script
      GENERATOR_PATH

      # Path to the SQLite3 include file
      INCLUDE_FILE

      # Output path for the include files
      INCLUDE_OUTPUT_PATH

      # Output path for the source files
      SOURCE_OUTPUT_PATH
   )

   set(multi_value_args)

   cmake_parse_arguments(GIF "${option_args}" "${single_value_args}" "${multi_value_args}" ${ARGN})

   list(LENGTH GIF_UNPARSED_ARGUMENTS _arg_len)
   if(NOT ${_arg_len} EQUAL 0)
      message(FATAL_ERROR "generation_isolation_files received unexpected arguments:\n${GIF_UNPARSED_ARGUMENTS}\nFull argument list:\n${ARGV}")
   endif()

   # Check generator availability
   if(NOT GIF_GENERATOR_PATH)
      message(FATAL_ERROR "Path to the generate-isolation-files script is required")
   endif()

   # Prepare the generator command
   set(GIF_GENERATOR_COMMAND
      python3 ${GIF_GENERATOR_PATH}
      -i ${GIF_INCLUDE_FILE}
      --output-include-file=${GIF_INCLUDE_OUTPUT_PATH}
      --output-src-file=${GIF_SOURCE_OUTPUT_PATH}
   )

   if(GIF_DRY_RUN)
      set(GIF_GENERATOR_COMMAND ${GIF_GENERATOR_COMMAND} --dry-run)
   endif()

   if(GIF_WRITE_HEADERS)
      set(GIF_GENERATOR_COMMAND ${GIF_GENERATOR_COMMAND} --write-headers)
   endif()

   if(GIF_WRITE_SOURCES)
      set(GIF_GENERATOR_COMMAND ${GIF_GENERATOR_COMMAND} --write-sources)
   endif()

   message(STATUS "Generating isolation files from: ${GIF_INCLUDE_FILE}")

   # Execute the compiler command
   execute_process(
      COMMAND ${GIF_GENERATOR_COMMAND}
      RESULT_VARIABLE GENERATOR_RESULT_CODE
      OUTPUT_VARIABLE GENERATOR_STDOUT
      ERROR_VARIABLE GENERATOR_STDERR
      OUTPUT_STRIP_TRAILING_WHITESPACE
   )
   if(NOT ${GENERATOR_RESULT_CODE} EQUAL "0")
      message(FATAL_ERROR "[${GENERATOR_RESULT_CODE}] Isolation files generator failed:\n${GENERATOR_STDOUT}\n${GENERATOR_STDERR}\n")
   endif()
endfunction()
