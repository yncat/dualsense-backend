# Basic Test Makefile for NMAKE

CC = cl.exe
LINK = link.exe

CFLAGS = /nologo /W3 /O2 /MD /EHsc /std:c++17
INCLUDES = /I..\..\include
LDFLAGS = /NOLOGO
LIBS = ..\..\bin\dualsense.lib

OUTDIR = ..\..\bin
TARGET = $(OUTDIR)\basic_test.exe
SRC = main.cpp
OBJ = main.obj

all: $(TARGET)

$(TARGET): $(OBJ)
	$(LINK) $(LDFLAGS) /OUT:$(TARGET) $(OBJ) $(LIBS)
	@echo.
	@echo Build complete! Executable: $(TARGET)
	@echo.

.cpp.obj:
	$(CC) $(CFLAGS) $(INCLUDES) /c $< /Fo$@

clean:
	@if exist $(OBJ) del /Q $(OBJ)
	@echo Cleaned build artifacts

run: $(TARGET)
	@echo.
	@echo Running $(TARGET)...
	@echo.
	@cd ..\..\bin && basic_test.exe

.PHONY: all clean run
