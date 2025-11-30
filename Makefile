# DualSense DLL Makefile for NMAKE (MSVC++)

CC = cl.exe
LINK = link.exe

# Compiler flags
CFLAGS = /nologo /W3 /O2 /MD /EHsc /std:c++17 /DUNICODE /D_UNICODE /DDUALSENSE_EXPORTS /D_CRT_SECURE_NO_WARNINGS
INCLUDES = /Iinclude /Isrc

# Linker flags
LDFLAGS = /DLL /NOLOGO /INCREMENTAL:NO
LIBS = hid.lib setupapi.lib

# Source files
SRC = \
	src\api\dualsense_api.cpp \
	src\api\device_manager.cpp \
	src\hid\windows_hid.cpp \
	src\protocol\output_composer.cpp \
	src\dllmain.cpp

# Object files
OBJ = \
	src\api\dualsense_api.obj \
	src\api\device_manager.obj \
	src\hid\windows_hid.obj \
	src\protocol\output_composer.obj \
	src\dllmain.obj

# Output directory
OUTDIR = bin

# Targets
TARGET = $(OUTDIR)\dualsense.dll
IMPLIB = $(OUTDIR)\dualsense.lib

all: $(OUTDIR) $(TARGET) example

$(OUTDIR):
	@if not exist $(OUTDIR) mkdir $(OUTDIR)

$(TARGET): $(OBJ)
	$(LINK) $(LDFLAGS) /OUT:$(TARGET) /IMPLIB:$(IMPLIB) $(OBJ) $(LIBS)
	@echo.
	@echo Build complete! DLL: $(TARGET)
	@echo.

.cpp.obj:
	$(CC) $(CFLAGS) $(INCLUDES) /c $< /Fo$@

example: $(TARGET)
	@echo.
	@echo Building basic_test example...
	@cd examples\basic_test && nmake /NOLOGO
	@echo.

clean:
	@if exist src\api\*.obj del /Q src\api\*.obj
	@if exist src\hid\*.obj del /Q src\hid\*.obj
	@if exist src\protocol\*.obj del /Q src\protocol\*.obj
	@if exist src\*.obj del /Q src\*.obj
	@if exist $(OUTDIR)\*.dll del /Q $(OUTDIR)\*.dll
	@if exist $(OUTDIR)\*.lib del /Q $(OUTDIR)\*.lib
	@if exist $(OUTDIR)\*.exp del /Q $(OUTDIR)\*.exp
	@if exist $(OUTDIR)\*.pdb del /Q $(OUTDIR)\*.pdb
	@if exist $(OUTDIR) rmdir /Q $(OUTDIR)
	@cd examples\basic_test && nmake /NOLOGO clean
	@echo Cleaned all build artifacts

.PHONY: all clean example
