find_package(Doxygen)
set(MD_SOURCES 01_intro.md 02_devguide.md)
set(IMAGE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/images")
set(DEST_IMAGE_PATH "${CMAKE_BINARY_DIR}/${IMAGE_PATH}")
set(PROJECT_LOGO "${CMAKE_CURRENT_SOURCE_DIR}/images/epri-logo.jpg")
set(IMAGES "${IMAGE_PATH}/stlink-usart.png"
    "${IMAGE_PATH}/image1.png"
    "${IMAGE_PATH}/image2.png"
    "${IMAGE_PATH}/image3.png"
    "${IMAGE_PATH}/image4.png"
    "${IMAGE_PATH}/image5.png"
    "${IMAGE_PATH}/image6.png"
    "${IMAGE_PATH}/image7.png"
    "${IMAGE_PATH}/image8.png"
    "${IMAGE_PATH}/image9.png"
    "${IMAGE_PATH}/image10.png"
    "${IMAGE_PATH}/image11.png"
    "${IMAGE_PATH}/image12.png"
    "${IMAGE_PATH}/image13.png"
    "${IMAGE_PATH}/image14.png"
    "${IMAGE_PATH}/image15.png"
    "${IMAGE_PATH}/image16.png"
) 

set(PLANTUML_JAR_PATH "/usr/share/java/plantuml.jar")
if(DOXYGEN_FOUND)
    configure_file(
        "doxygen.conf.in"
        "doxygen.conf"
        @ONLY
    )
    add_custom_target(
        doc
        "${DOXYGEN_EXECUTABLE}"
        "${CMAKE_CURRENT_BINARY_DIR}/doxygen.conf"
        OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/html/index.html"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        DEPENDS "${MD_SOURCES}"
        DEPENDS "${PROJECT_LOGO}"
        DEPENDS "${IMAGES}"
        COMMENT "Generating code documentation..." VERBATIM
    )
    add_custom_target(
        pdf
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/latex/refman.pdf"
    )
    add_custom_command(
        OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/latex/refman.pdf"
        COMMAND "make"
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/html/index.html"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/latex"
        COMMENT "Generating PDF format code documentation..." VERBATIM
    )

endif(DOXYGEN_FOUND)
