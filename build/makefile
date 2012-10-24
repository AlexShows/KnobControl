#define macros
EXEC_NAME = KnobControl.exe
DIR_SRC = ..\src
DIR_INCLUDE = ..\inc
DIR_LIB = ..\lib
DIR_BIN = ..\bin
DIR_INTERMEDIATE = ..\intermediate
DIR_BUILD = ..\build

SRC_FILES = \
	$(DIR_SRC)\KnobControl.cpp
	
#description block
$(EXEC_NAME) : $(SRC_FILES)
	cl /EHsc /Zi /Fe$(DIR_BIN)\$(EXECUTABLE_NAME) /I$(DIR_INCLUDE) $(SRC_FILES) $(DIR_LIB)\hid.lib setupapi.lib
	copy *.obj $(DIR_INTERMEDIATE)
	del *.obj

# build application
knobcontrol: $(EXEC_NAME)

# create output directories
create_dirs:
 @if not exist $(DIR_BIN) mkdir $(DIR_BIN)
 @if not exist $(DIR_INTERMEDIATE) mkdir $(DIR_INTERMEDIATE)

# delete output directories
clean:
 @if exist $(DIR_BIN) rmdir /S /Q $(DIR_BIN)
 @if exist $(DIR_INTERMEDIATE) rmdir /S /Q $(DIR_INTERMEDIATE)

# create directories and build application
all: clean create_dirs knobcontrol
