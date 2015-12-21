# Copyright 2013 - 2015 Ivan Limar
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


TARGET = robots-trik-v62-russian-c-generator

include(../../../../../../global.pri)

QT += widgets network

TEMPLATE = lib
CONFIG += plugin

DESTDIR = $$DESTDIR/plugins/tools/kitPlugins/

includes(\
                plugins/robots/generators/trik/trikGeneratorBase \
                plugins/robots/generators/generatorBase \
                plugins/robots/common/kitBase \
                plugins/robots/common/trikKit \
                plugins/robots/utils \
                qrtext \
)

links(qrkernel qslog qrutils qrrepo qscintilla2 robots-trik-generator-base robots-generator-base robots-trik-kit \
                robots-kit-base qrgui-preferences-dialog robots-utils \
)

RESOURCES += \
    $$PWD/templates.qrc \
    $$PWD/trikRussianCGeneratorLibrary.qrc

HEADERS += \
    $$PWD/src/trikRussianCControlFlowValidator.h \
    $$PWD/src/trikRussianCMasterGenerator.h \
    $$PWD/trikV62RussianCGeneratorPlugin.h \
    $$PWD/include/trikRussianCGeneratorLibrary/trikRussianCGeneratorPluginBase.h

SOURCES += \
    $$PWD/src/trikRussianCControlFlowValidator.cpp \
    $$PWD/src/trikRussianCGeneratorPluginBase.cpp \
    $$PWD/src/trikRussianCMasterGenerator.cpp \
    $$PWD/trikV62RussianCGeneratorPlugin.cpp
