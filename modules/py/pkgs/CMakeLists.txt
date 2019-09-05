cmake_minimum_required(VERSION 3.12)
if(${ENABLE_PYTHON})

    set(QNLP_PYTHON_PKGS    QNLP;
                            CircuitPrinter;
    )

    project(qnlp_python)

    find_package(Python COMPONENTS Interpreter Development)
    # Install the bound module into the QNLP Python environment
    configure_file(setup.py ${CMAKE_CURRENT_BINARY_DIR}/setup.py )

    foreach(PKG IN LISTS QNLP_PYTHON_PKGS)
       file(COPY ${PKG} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
    endforeach()
    
    add_custom_target(PipInstallerPkgs ALL DEPENDS PipInstaller)
    add_custom_command(TARGET PipInstallerPkgs
        # Run after all other rules within the target have been executed
        POST_BUILD
        COMMAND pip install .
        COMMENT "Installing Python packages into QNLP environment"
        VERBATIM
    )
endif()
