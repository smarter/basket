project(context-vertical-containment)

include_directories( ../../..
                     ../..
                     ..)

set(context_SRCS 
	columnscontainment.cpp
	)

kde4_add_plugin(basket_containment_columns ${context_SRCS})

target_link_libraries(basket_containment_columns basketcommon ${KDE4_PLASMA_LIBS} ${KDE4_KIO_LIBS})

install(TARGETS basket_containment_columns DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES basket-containment-columns.desktop DESTINATION ${SERVICES_INSTALL_DIR})
