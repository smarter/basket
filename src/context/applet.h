#ifndef BASKET_APPLET_H
#define BASKET_APPLET_H

#include "basket_export.h"

#include <Plasma/Applet>

class BASKET_EXPORT Applet : public Plasma::Applet
{
    Q_OBJECT
    public:
        explicit Applet( QObject* parent, const QVariantList& args = QVariantList() );
};

/**
 * Register an applet when it is contained in a loadable module
 * Copied from Amarok Applet class
 */
#define K_EXPORT_BASKET_APPLET(libname, classname) \
K_PLUGIN_FACTORY(factory, registerPlugin<classname>();) \
K_EXPORT_PLUGIN(factory("basket_applet_" #libname))\
K_EXPORT_PLUGIN_VERSION(PLASMA_VERSION)
#endif
