project(notes)

set(notes_SRCS notes.cpp)

include_directories( ../..
                    ../../.. )

kde4_add_ui_files(notes_SRCS config.ui)

kde4_add_plugin(basket_applet_notes ${notes_SRCS})

target_link_libraries(basket_applet_notes basketcommon ${KDE4_PLASMA_LIBS} ${KDE4_KIO_LIBS})

install(
    TARGETS basket_applet_notes
    DESTINATION ${PLUGIN_INSTALL_DIR}
    )

install(
    FILES basket-applet-notes.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
    )

install(
    FILES notes.svgz
    DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/default/widgets/)
