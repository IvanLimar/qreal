# Copyright 2013 - 2015 A. N. Terekhov
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


include(../../../../../../../global.pri)

TARGET = russianCInterpreter
CONFIG   += console

copyToDestdir($$PWD/run.sh)

TEMPLATE = app

HEADERS += \
          $$PWD/Defs.h \
          $$PWD/global_vars.h
SOURCES += \
          $$PWD/codes.c \
          $$PWD/import.c \
          $$PWD/main.c \
          $$PWD/scaner.c
