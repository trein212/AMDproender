set(SOURCES
    aov.h
    basic.h
    camera.h
    input_maps.h
    internal.h
    light.h
    main.cpp
    material.h
    test_scenes.h
    uberv2.h)

add_executable(BaikalTest ${SOURCES})
target_compile_features(BaikalTest PRIVATE cxx_std_14)
target_include_directories(BaikalTest PRIVATE .)
target_link_libraries(BaikalTest PRIVATE Baikal BaikalIO GTest)
set_target_properties(BaikalTest
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${Baikal_SOURCE_DIR}/BaikalTest)
target_compile_definitions(BaikalTest PRIVATE _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING=1)

add_custom_target(BaikalTestImagesDir)

set(BAIKALTEST_REFERENCEIMAGES_DIR ReferenceImages)
FILE(TO_NATIVE_PATH ${Baikal_SOURCE_DIR}/BaikalTest/${BAIKALTEST_REFERENCEIMAGES_DIR} BAIKALTEST_REFERENCEIMAGES_SRC)
FILE(TO_NATIVE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BAIKALTEST_REFERENCEIMAGES_DIR} BAIKALTEST_REFERENCEIMAGES_DST)

set(BAIKALTEST_OUTPUTIMAGES_DIR OutputImages)
FILE(TO_NATIVE_PATH ${Baikal_SOURCE_DIR}/BaikalTest/${BAIKALTEST_OUTPUTIMAGES_DIR} BAIKALTEST_OUTPUTIMAGES_SRC)
FILE(TO_NATIVE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BAIKALTEST_OUTPUTIMAGES_DIR} BAIKALTEST_OUTPUTIMAGES_DST)

if (WIN32)
    add_custom_command(TARGET BaikalTestImagesDir POST_BUILD
        COMMAND IF NOT EXIST ${BAIKALTEST_REFERENCEIMAGES_DST} mklink /D ${BAIKALTEST_REFERENCEIMAGES_DST} ${BAIKALTEST_REFERENCEIMAGES_SRC}
        COMMAND IF NOT EXIST ${BAIKALTEST_OUTPUTIMAGES_DST} mklink /D ${BAIKALTEST_OUTPUTIMAGES_DST} ${BAIKALTEST_OUTPUTIMAGES_SRC}
        DEPENDS ${BAIKALTEST_REFERENCEIMAGES_DST} ${BAIKALTEST_OUTPUTIMAGES_DST}
    )
else ()
    add_custom_command(TARGET BaikalTestImagesDir POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${BAIKALTEST_REFERENCEIMAGES_SRC} ${BAIKALTEST_REFERENCEIMAGES_DST}
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${BAIKALTEST_OUTPUTIMAGES_SRC} ${BAIKALTEST_OUTPUTIMAGES_DST}
        DEPENDS ${BAIKALTEST_REFERENCEIMAGES_DST} ${BAIKALTEST_OUTPUTIMAGES_DST}
    )
endif ()

add_dependencies(BaikalTest ResourcesDir BaikalKernelsDir BaikalTestImagesDir)

if (WIN32)
    add_custom_command(TARGET BaikalTest POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            ${BAIKAL_TESTS_DLLS}
            "$<TARGET_FILE_DIR:BaikalTest>"
    )
endif ()

