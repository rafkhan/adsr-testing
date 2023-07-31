# Project Name
TARGET = adsr-experiment

# Flags
LDFLAGS += -u _printf_float

# Sources
CPP_SOURCES = adsr-experiment.cpp
CPP_SOURCES += OkayAdsr.cpp

# Library Locations
LIBDAISY_DIR = ../DaisyExamples/libDaisy/
DAISYSP_DIR = ../DaisyExamples/DaisySP/

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
