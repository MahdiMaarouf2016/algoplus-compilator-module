#---------------------------------------------------------------------------------
#
#        ALGO+ CUSTOM COMPILER PROJECT, PROJECT REQUIRED BY ALGO+ 2016-2017
#                           SATURDAY ‎11 AUGOST ‎2018, ‏‎12:27:18
#
#               NOTICE : THIS PROJECT MUST COMPILED BY C++98 COMPILER
#
#---------------------------------------------------------------------------------

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += qt
//QT += xml

SOURCES += main.cpp \
    codesgenerator.cpp \
    compilateur.cpp \
    compilationerror.cpp \
    function.cpp \
    lexicalanalyser.cpp \
    lexicalerror.cpp \
    mainscope.cpp \
    procedure.cpp \
    scope.cpp \
    symanticanalyser.cpp \
    syntaxicanalyser.cpp \
    syntaxicerror.cpp \
    symanticerror.cpp\
    warning.cpp \
    token.cpp \
    stream.cpp \
    debuger.cpp \
    stdstring.cpp \
    Char.cpp

HEADERS += \
    codesgenerator.h \
    compilateur.h \
    compilationerror.h \
    function.h \
    lexicalanalyser.h \
    lexicalerror.h \
    mainscope.h \
    procedure.h \
    scope.h \
    symanticanalyser.h \
    syntaxicanalyser.h \
    syntaxicerror.h \
    symanticerror.h\
    warning.h \
    token.h \
    headerAlgo.h \
    coreAlgo.h \
    stream. cpp \
    stream.h \
    stdstring.h \
    debuger.h \
    Char.h

OTHER_FILES += \
    XmlAlgConfig.inc\
    testScenario.txt


#-------------------------------------- NOTES --------------------------------------------
# toutes conversions de type demandé par ce projet ont ete terminé le 21/09/2018 à 02:28
#-----------------------------------------------------------------------------------------
