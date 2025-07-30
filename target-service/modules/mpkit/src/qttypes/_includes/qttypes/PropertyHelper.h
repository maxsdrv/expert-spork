#pragma once

#include <QObject>

// clang-format off
/// Defines 'member' property and corresponding signal
#define MEMBER_PROPERTY(TYPE, NAME, DEFAULT)                   \
    Q_PROPERTY(TYPE NAME MEMBER m_##NAME NOTIFY NAME##Changed) \
public:                                                        \
    Q_SIGNAL void NAME##Changed(TYPE value);                   \
private:                                                       \
    TYPE m_##NAME{DEFAULT};

/// Defines readonly member property
#define READONLY_MEMBER_PROPERTY(TYPE, NAME, DEFAULT) \
    Q_PROPERTY(TYPE NAME MEMBER m_##NAME CONSTANT)    \
private:                                              \
    TYPE m_##NAME{DEFAULT};

/// Declare property getter/setter/notifier
#define DECLARE_PROPERTY(TYPE, NAME, SETTER)                          \
    Q_PROPERTY(TYPE NAME READ NAME WRITE SETTER NOTIFY NAME##Changed) \
public:                                                               \
    Q_SIGNAL void NAME##Changed(TYPE value);
// clang-format on
