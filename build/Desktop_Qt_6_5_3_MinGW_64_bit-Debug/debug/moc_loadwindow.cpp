/****************************************************************************
** Meta object code from reading C++ file 'loadwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../loadwindow.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'loadwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSLoadWindowENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSLoadWindowENDCLASS = QtMocHelpers::stringData(
    "LoadWindow",
    "onReturnToMain",
    "",
    "showVolumeControl",
    "selectAudio",
    "beginplay",
    "stopplay",
    "continueplay"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSLoadWindowENDCLASS_t {
    uint offsetsAndSizes[16];
    char stringdata0[11];
    char stringdata1[15];
    char stringdata2[1];
    char stringdata3[18];
    char stringdata4[12];
    char stringdata5[10];
    char stringdata6[9];
    char stringdata7[13];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSLoadWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSLoadWindowENDCLASS_t qt_meta_stringdata_CLASSLoadWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 10),  // "LoadWindow"
        QT_MOC_LITERAL(11, 14),  // "onReturnToMain"
        QT_MOC_LITERAL(26, 0),  // ""
        QT_MOC_LITERAL(27, 17),  // "showVolumeControl"
        QT_MOC_LITERAL(45, 11),  // "selectAudio"
        QT_MOC_LITERAL(57, 9),  // "beginplay"
        QT_MOC_LITERAL(67, 8),  // "stopplay"
        QT_MOC_LITERAL(76, 12)   // "continueplay"
    },
    "LoadWindow",
    "onReturnToMain",
    "",
    "showVolumeControl",
    "selectAudio",
    "beginplay",
    "stopplay",
    "continueplay"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSLoadWindowENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   50,    2, 0x08,    1 /* Private */,
       3,    0,   51,    2, 0x08,    2 /* Private */,
       4,    0,   52,    2, 0x08,    3 /* Private */,
       5,    0,   53,    2, 0x08,    4 /* Private */,
       6,    0,   54,    2, 0x08,    5 /* Private */,
       7,    0,   55,    2, 0x08,    6 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject LoadWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSLoadWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSLoadWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSLoadWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<LoadWindow, std::true_type>,
        // method 'onReturnToMain'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showVolumeControl'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'selectAudio'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'beginplay'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'stopplay'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'continueplay'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void LoadWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LoadWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onReturnToMain(); break;
        case 1: _t->showVolumeControl(); break;
        case 2: _t->selectAudio(); break;
        case 3: _t->beginplay(); break;
        case 4: _t->stopplay(); break;
        case 5: _t->continueplay(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *LoadWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LoadWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSLoadWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int LoadWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
