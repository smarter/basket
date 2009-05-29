set(free_SRCS
    desktop.cpp
    itemspace.cpp
    desktoplayout.cpp)

kde4_add_plugin(basket_containment_free ${free_SRCS})
if(WIN32)
target_link_libraries(basket_containment_free ${KDE4_PLASMA_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KFILE_LIBS})
else(WIN32)
target_link_libraries(basket_containment_free ${KDE4_PLASMA_LIBS} kworkspace ${KDE4_KIO_LIBS} ${KDE4_KFILE_LIBS} )
endif(WIN32)

install(TARGETS basket_containment_free DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES basket-containment-free.desktop DESTINATION ${SERVICES_INSTALL_DIR})
