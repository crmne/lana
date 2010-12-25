# file(COPY . DESTINATION ${PROJECT_BINARY_DIR} PATTERN "*.rb" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE) # CMake 2.8 only
file(GLOB scripts RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*.rb)
foreach(script ${scripts})
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${script} ${PROJECT_BINARY_DIR}/${script})
endforeach(script)
