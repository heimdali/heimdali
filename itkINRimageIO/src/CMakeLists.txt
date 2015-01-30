set(ITKIOINR_SRC
itkINRImageIOFactory.cxx
itkINRImageIO.cxx
)

add_library(ITKIOINR ${ITKIOINR_SRC})

target_link_libraries(ITKIOINR
${ITKINR_LIBRARIES}
${ITKIOImageBase_LIBRARIES}
${TEST_DEPENDS_LIBRARIES}
${ITKTestKernel_LIBRARIES}
${ITKINR_LIBRARIES}
)

itk_module_target(ITKIOINR)
