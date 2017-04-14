#file(WRITE "${CMAKE_SOURCE_DIR}/QtCreatorDeployment.txt" "cosmos\n")

#set(program_name programs/tests/test_device_i2c)
#file(RELATIVE_PATH relativeDir ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR}/${program_name})
#add_deployment_binary(programs/tests/test_device_i2c "bin")

#set(program_name programs/agents/agent_exec)
#file(RELATIVE_PATH relativeDir ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR}/${program_name})
#add_deployment_file(${relativeDir} "bin")

#set(program_name programs/agents/agent_cpu)
#file(RELATIVE_PATH relativeDir ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR}/${program_name})
#add_deployment_file(${relativeDir} "bin")

#set(program_name programs/general/command_generator)
#file(RELATIVE_PATH relativeDir ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR}/${program_name})
#add_deployment_file(${relativeDir} "bin")

#file(RELATIVE_PATH relativeDir ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR}/programs/agents)
#add_deployment_directory(${relativeDir} "")

#add_deployment_file(scripts/setupCosmosCore.sh "")
