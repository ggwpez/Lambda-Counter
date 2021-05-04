TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt c++11

SOURCES += \
	main.cpp

# CONFIG -= exceptions # somehow this wont disabl exceptions
QMAKE_LIBS += -lgmpxx -lgmp -lpthread
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
	memoize/tuple_hash.hpp \
	multiarray.hpp

debug {
	DEFINES += DEBUG
}
release {
	DEFINES -= DEBUG
	DEFINES += NOEXCEPT
	QMAKE_CXXFLAGS += -fno-exceptions -fno-rtti
	QMAKE_LFLAGS = -Wl,-O2
}

QMAKE_CXXFLAGS += -std=c++17
