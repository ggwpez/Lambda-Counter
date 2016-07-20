TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.cpp

# CONFIG -= exceptions # somehow this wont disabl exceptions
QMAKE_LIBS += -lgmpxx -lgmp
QMAKE_LFLAGS += -static

HEADERS += \
    mpz_wrapper.hpp \
    memoize/cache.hpp \
    memoize/dontcache.hpp \
    memoize/function_traits.hpp \
    memoize/template_conditional2.hpp \
    memoize/template_filter.hpp \
    memoize/template_for_all.hpp \
    memoize/template_predicates.hpp \
    memoize/tuple_hash.hpp

debug {
    DEFINES += DEBUG
}
release {
    DEFINES -= DEBUG
}

QMAKE_CFLAGS_RELEASE += -fno-exceptions
QMAKE_CXXFLAGS_RELEASE += -fno-exceptions

QMAKE_CXXFLAGS += -std=c++17
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

QMAKE_LFLAGS_RELEASE -= -O1
QMAKE_LFLAGS_RELEASE += -O3