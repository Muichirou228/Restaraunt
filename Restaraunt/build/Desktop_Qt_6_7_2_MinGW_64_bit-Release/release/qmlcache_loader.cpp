#include <QtQml/qqmlprivate.h>
#include <QtCore/qdir.h>
#include <QtCore/qurl.h>
#include <QtCore/qhash.h>
#include <QtCore/qstring.h>

namespace QmlCacheGeneratedCode {
namespace _0x5f_Restaraunt_main_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _0x5f__TablesScheme_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _0x5f__tables_booking_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}

}
namespace {
struct Registry {
    Registry();
    ~Registry();
    QHash<QString, const QQmlPrivate::CachedQmlUnit*> resourcePathToCachedUnit;
    static const QQmlPrivate::CachedQmlUnit *lookupCachedUnit(const QUrl &url);
};

Q_GLOBAL_STATIC(Registry, unitRegistry)


Registry::Registry() {
    resourcePathToCachedUnit.insert(QStringLiteral("/Restaraunt/main.qml"), &QmlCacheGeneratedCode::_0x5f_Restaraunt_main_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/TablesScheme.qml"), &QmlCacheGeneratedCode::_0x5f__TablesScheme_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/tables_booking.qml"), &QmlCacheGeneratedCode::_0x5f__tables_booking_qml::unit);
    QQmlPrivate::RegisterQmlUnitCacheHook registration;
    registration.structVersion = 0;
    registration.lookupCachedQmlUnit = &lookupCachedUnit;
    QQmlPrivate::qmlregister(QQmlPrivate::QmlUnitCacheHookRegistration, &registration);
}

Registry::~Registry() {
    QQmlPrivate::qmlunregister(QQmlPrivate::QmlUnitCacheHookRegistration, quintptr(&lookupCachedUnit));
}

const QQmlPrivate::CachedQmlUnit *Registry::lookupCachedUnit(const QUrl &url) {
    if (url.scheme() != QLatin1String("qrc"))
        return nullptr;
    QString resourcePath = QDir::cleanPath(url.path());
    if (resourcePath.isEmpty())
        return nullptr;
    if (!resourcePath.startsWith(QLatin1Char('/')))
        resourcePath.prepend(QLatin1Char('/'));
    return unitRegistry()->resourcePathToCachedUnit.value(resourcePath, nullptr);
}
}
int QT_MANGLE_NAMESPACE(qInitResources_qmake_resources)() {
    ::unitRegistry();
    Q_INIT_RESOURCE(build_Desktop_Qt_6_7_2_MinGW_64_bit_Release_release_qmake_resources_qmlcache);
    return 1;
}
Q_CONSTRUCTOR_FUNCTION(QT_MANGLE_NAMESPACE(qInitResources_qmake_resources))
int QT_MANGLE_NAMESPACE(qCleanupResources_qmake_resources)() {
    Q_CLEANUP_RESOURCE(build_Desktop_Qt_6_7_2_MinGW_64_bit_Release_release_qmake_resources_qmlcache);
    return 1;
}
int QT_MANGLE_NAMESPACE(qInitResources_qmake_qmake_immediate)() {
    ::unitRegistry();
    Q_INIT_RESOURCE(build_Desktop_Qt_6_7_2_MinGW_64_bit_Release_release_qmake_qmake_immediate_qmlcache);
    return 1;
}
Q_CONSTRUCTOR_FUNCTION(QT_MANGLE_NAMESPACE(qInitResources_qmake_qmake_immediate))
int QT_MANGLE_NAMESPACE(qCleanupResources_qmake_qmake_immediate)() {
    Q_CLEANUP_RESOURCE(build_Desktop_Qt_6_7_2_MinGW_64_bit_Release_release_qmake_qmake_immediate_qmlcache);
    return 1;
}
