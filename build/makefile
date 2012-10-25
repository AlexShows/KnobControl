#define macros
EXEC_NAME = KnobControl.exe
DIR_SRC = ..\src
DIR_INCLUDE = ..\inc
DIR_LIB = ..\lib
DIR_DEBUG_BIN = ..\debug
DIR_RELEASE_BIN = ..\release
DIR_INTERMEDIATE = ..\intermediate
DIR_BUILD = ..\build

SRC_FILES = \
	$(DIR_SRC)\KnobControl.cpp \
	$(DIR_SRC)\HIDComm.cpp
	
# knobcontrol (release version)
knobcontrol : $(SRC_FILES)
	cl /EHsc /Fe$(DIR_RELEASE_BIN)\$(EXEC_NAME) /I$(DIR_INCLUDE) $(SRC_FILES) $(DIR_LIB)\hid.lib setupapi.lib
	copy *.obj $(DIR_INTERMEDIATE)
	del *.obj

# knobcontrol (debug version)
knobcontrol_debug : $(SRC_FILES)
	cl /EHsc /Zi /Fe$(DIR_DEBUG_BIN)\$(EXEC_NAME) /I$(DIR_INCLUDE) $(SRC_FILES) $(DIR_LIB)\hid.lib setupapi.lib
	copy *.obj $(DIR_INTERMEDIATE)
	del *.obj
	copy *.pdb $(DIR_INTERMEDIATE)
	del *.pdb

# create release output directories
create_release_dirs :
 @if not exist $(DIR_RELEASE_BIN) mkdir $(DIR_RELEASE_BIN)
 @if not exist $(DIR_INTERMEDIATE) mkdir $(DIR_INTERMEDIATE)

# create debug output directories
create_debug_dirs :
 @if not exist $(DIR_DEBUG_BIN) mkdir $(DIR_DEBUG_BIN)
 @if not exist $(DIR_INTERMEDIATE) mkdir $(DIR_INTERMEDIATE)
 
# delete output directories
clean :
 @if exist $(DIR_DEBUG_BIN) rmdir /S /Q $(DIR_DEBUG_BIN)
 @if exist $(DIR_RELEASE_BIN) rmdir /S /Q $(DIR_RELEASE_BIN)
 @if exist $(DIR_INTERMEDIATE) rmdir /S /Q $(DIR_INTERMEDIATE)

# clean, create directories and build release version
all : clean create_release_dirs knobcontrol
release : clean create_release_dirs knobcontrol

# clean, create directories and build a debug version
debug : clean create_debug_dirs knobcontrol_debug
