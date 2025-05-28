/****************************************************************************
** Meta object code from reading C++ file 'scoreeditor.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../scoreeditor.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'scoreeditor.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSScoreEditorENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSScoreEditorENDCLASS = QtMocHelpers::stringData(
    "ScoreEditor",
    "exitRequested",
    "",
    "startluzhi",
    "savediyscore",
    "startmebutton_clicked",
    "startmetronome",
    "Note",
    "menote",
    "openEditMode"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSScoreEditorENDCLASS_t {
    uint offsetsAndSizes[20];
    char stringdata0[12];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[11];
    char stringdata4[13];
    char stringdata5[22];
    char stringdata6[15];
    char stringdata7[5];
    char stringdata8[7];
    char stringdata9[13];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSScoreEditorENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSScoreEditorENDCLASS_t qt_meta_stringdata_CLASSScoreEditorENDCLASS = {
    {
        QT_MOC_LITERAL(0, 11),  // "ScoreEditor"
        QT_MOC_LITERAL(12, 13),  // "exitRequested"
        QT_MOC_LITERAL(26, 0),  // ""
        QT_MOC_LITERAL(27, 10),  // "startluzhi"
        QT_MOC_LITERAL(38, 12),  // "savediyscore"
        QT_MOC_LITERAL(51, 21),  // "startmebutton_clicked"
        QT_MOC_LITERAL(73, 14),  // "startmetronome"
        QT_MOC_LITERAL(88, 4),  // "Note"
        QT_MOC_LITERAL(93, 6),  // "menote"
        QT_MOC_LITERAL(100, 12)   // "openEditMode"
    },
    "ScoreEditor",
    "exitRequested",
    "",
    "startluzhi",
    "savediyscore",
    "startmebutton_clicked",
    "startmetronome",
    "Note",
    "menote",
    "openEditMode"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSScoreEditorENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   50,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       3,    0,   51,    2, 0x08,    2 /* Private */,
       4,    0,   52,    2, 0x08,    3 /* Private */,
       5,    0,   53,    2, 0x08,    4 /* Private */,
       6,    1,   54,    2, 0x08,    5 /* Private */,
       9,    0,   57,    2, 0x08,    7 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject ScoreEditor::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSScoreEditorENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSScoreEditorENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSScoreEditorENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ScoreEditor, std::true_type>,
        // method 'exitRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'startluzhi'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'savediyscore'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'startmebutton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'startmetronome'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Note &, std::false_type>,
        // method 'openEditMode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void ScoreEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ScoreEditor *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->exitRequested(); break;
        case 1: _t->startluzhi(); break;
        case 2: _t->savediyscore(); break;
        case 3: _t->startmebutton_clicked(); break;
        case 4: _t->startmetronome((*reinterpret_cast< std::add_pointer_t<Note>>(_a[1]))); break;
        case 5: _t->openEditMode(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ScoreEditor::*)();
            if (_t _q_method = &ScoreEditor::exitRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *ScoreEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScoreEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSScoreEditorENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int ScoreEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void ScoreEditor::exitRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
