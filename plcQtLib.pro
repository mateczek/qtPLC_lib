#/*Ten plik jest częścią biblioteki plcQtLib.
#
#    plcQtLib jest wolnym oprogramowaniem; możesz go rozprowadzać dalej
#    i/lub modyfikować na warunkach Powszechnej Licencji Publicznej GNU,
#    wydanej przez Fundację Wolnego Oprogramowania - według wersji 2 tej
#    Licencji lub którejś z późniejszych wersji.
#
#    Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
#    użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
#    gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
#    ZASTOSOWAŃ. W celu uzyskania bliższych informacji sięgnij do
#    Powszechnej Licencji Publicznej GNU.
#
#    Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
#    Powszechnej Licencji Publicznej GNU (GNU General Public License);
#    jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
#    Place, Fifth Floor, Boston, MA  02110-1301  USA
#*/
#(C) mateczek marcof@interia.pl 2016
QT       += network
CONFIG+=c++11
QT       -= gui

TARGET = plcQtLib
TEMPLATE = lib
CONFIG += staticlib
win32{
    DEFINES += BCCWIN DAVE_LITTLE_ENDIAN
}
unix{
    DEFINES += LINUX DAVE_LITTLE_ENDIAN
}
SOURCES += plcqtlib.cpp \
    nodave.c

HEADERS += plcqtlib.h \
    log2.h \
    nodave.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
