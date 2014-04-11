
# Minimum Version of CMake Required
cmake_minimum_required(VERSION 2.8)

# Global variables -------------------------------------------------------------
set( global_cxx_flags			"${global_cxx_flags}			-Wall"				)
set( global_cxx_flags_debug		"${global_cxx_flags_debug}		-g -DDEBUG"			)
set( global_cxx_flags_release	"${global_cxx_flags_release}	-s -O2 -DRELEASE"	)

set( root_dir	${CMAKE_CURRENT_SOURCE_DIR}	)
set( src_in_dir	${root_dir}/src				)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY	${bin_out_dir}	)
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${lib_out_dir}	)

set( CMAKE_MODULE_PATH	${CMAKE_CURRENT_SOURCE_DIR}/cmake	)

# My Little Investigations -----------------------------------------------------

set( PROJECT mli )

# Definitions
add_definitions( -DTIXML_USE_TICPP 			)
add_definitions( -D__STDC_CONSTANT_MACROS	)

if( CMAKE_BUILD_TYPE MATCHES "Debug" )
	add_definitions( -DENABLE_DEBUG_MODE	)
	add_definitions( -DMLI_DEBUG			)
endif()

# Game -------------------------------------------------------------------------

# Project properties
project(${PROJECT}-game)

# Dependencies
find_package( SDL2			REQUIRED	)
find_package( SDL2_image	REQUIRED	)
find_package( SDL2_ttf		REQUIRED	)
find_package( SDL2_mixer	REQUIRED	)
find_package( FFMPEG 		REQUIRED	)
find_package( Crypto++		REQUIRED	)

# G++ Flags
set( CMAKE_CXX_FLAGS			${global_cxx_flags}			)
set( CMAKE_CXX_FLAGS_DEBUG		${global_cxx_flags_debug}	)
set( CMAKE_CXX_FLAGS_RELEASE	${global_cxx_flags_release}	)

# Sources files list
file( GLOB_RECURSE ${PROJECT_NAME}_src
	src/Animation.cpp
	src/Animation.h
	src/AnimationSound.cpp
	src/AnimationSound.h
	src/CaseContent/Area.cpp
	src/CaseContent/Area.h
	src/CaseContent/Conversation.cpp
	src/CaseContent/Conversation.h
	src/CaseContent/Crowd.cpp
	src/CaseContent/Crowd.h
	src/CaseContent/Dialog.cpp
	src/CaseContent/Dialog.h
	src/CaseContent/Encounter.cpp
	src/CaseContent/Encounter.h
	src/CaseContent/FieldCharacter.cpp
	src/CaseContent/FieldCharacter.h
	src/CaseContent/FieldCutscene.cpp
	src/CaseContent/FieldCutscene.h
	src/CaseContent/ForegroundElement.cpp
	src/CaseContent/ForegroundElement.h
	src/CaseContent/Location.cpp
	src/CaseContent/Location.h
	src/CaseContent/Notification.cpp
	src/CaseContent/Notification.h
	src/CaseContent/ZoomedView.cpp
	src/CaseContent/ZoomedView.h
	src/CaseInformation/AnimationManager.cpp
	src/CaseInformation/AnimationManager.h
	src/CaseInformation/AudioManager.cpp
	src/CaseInformation/AudioManager.h
	src/CaseInformation/Case.cpp
	src/CaseInformation/Case.h
	src/CaseInformation/CommonCaseResources.cpp
	src/CaseInformation/CommonCaseResources.h
	src/CaseInformation/ContentManager.cpp
	src/CaseInformation/ContentManager.h
	src/CaseInformation/DialogCharacterManager.cpp
	src/CaseInformation/DialogCharacterManager.h
	src/CaseInformation/DialogCutsceneManager.cpp
	src/CaseInformation/DialogCutsceneManager.h
	src/CaseInformation/EvidenceManager.cpp
	src/CaseInformation/EvidenceManager.h
	src/CaseInformation/FieldCharacterManager.cpp
	src/CaseInformation/FieldCharacterManager.h
	src/CaseInformation/FieldCutsceneManager.cpp
	src/CaseInformation/FieldCutsceneManager.h
	src/CaseInformation/FlagManager.cpp
	src/CaseInformation/FlagManager.h
	src/CaseInformation/FontManager.cpp
	src/CaseInformation/FontManager.h
	src/CaseInformation/PartnerManager.cpp
	src/CaseInformation/PartnerManager.h
	src/CaseInformation/SpriteManager.cpp
	src/CaseInformation/SpriteManager.h
	src/Collisions.cpp
	src/Collisions.h
	src/Color.cpp
	src/Color.h
	src/Condition.cpp
	src/Condition.h
	src/EasingFunctions.cpp
	src/EasingFunctions.h
	src/Events/ButtonArrayEventProvider.h
	src/Events/ButtonEventProvider.h
	src/Events/CaseParsingEventProvider.h
	src/Events/DialogEventProvider.h
	src/Events/EventProviders.cpp
	src/Events/EventProviders.h
	src/Events/EvidenceSelectorEventProvider.h
	src/Events/ImageButtonEventProvider.h
	src/Events/LocationEventProvider.h
	src/Events/PromptOverlayEventProvider.h
	src/Events/SelectorEventProvider.h
	src/FileFunctions.cpp
	src/FileFunctions.h
	src/Font.cpp
	src/Font.h
	src/Game.cpp
	src/Game.h
	src/HeightMap.cpp
	src/HeightMap.h
	src/Image.cpp
	src/Image.h
	src/Interfaces.cpp
	src/Interfaces.h
	src/Line.cpp
	src/Line.h
	src/MouseHelper.cpp
	src/MouseHelper.h
	src/Polygon.cpp
	src/Polygon.h
	src/PositionalSound.h
	src/Rectangle.cpp
	src/Rectangle.h
	src/ResourceLoader.cpp
	src/ResourceLoader.h
	src/Screens/GameScreen.cpp
	src/Screens/GameScreen.h
	src/Screens/LogoScreen.cpp
	src/Screens/LogoScreen.h
	src/Screens/OptionsScreen.cpp
	src/Screens/OptionsScreen.h
	src/Screens/Screen.h
	src/Screens/SelectionScreen.cpp
	src/Screens/SelectionScreen.h
	src/Screens/TitleScreen.cpp
	src/Screens/TitleScreen.h
	src/Sprite.cpp
	src/Sprite.h
	src/State.cpp
	src/State.h
	src/TextInputHelper.cpp
	src/TextInputHelper.h
	src/TransitionRequest.h
	src/UserInterface/Arrow.cpp
	src/UserInterface/Arrow.h
	src/UserInterface/ButtonArray.cpp
	src/UserInterface/ButtonArray.h
	src/UserInterface/EvidenceDescription.cpp
	src/UserInterface/EvidenceDescription.h
	src/UserInterface/EvidenceSelector.cpp
	src/UserInterface/EvidenceSelector.h
	src/UserInterface/ImageButton.cpp
	src/UserInterface/ImageButton.h
	src/UserInterface/PartnerInformation.cpp
	src/UserInterface/PartnerInformation.h
	src/UserInterface/PromptOverlay.cpp
	src/UserInterface/PromptOverlay.h
	src/UserInterface/Selector.cpp
	src/UserInterface/Selector.h
	src/UserInterface/SkipArrow.cpp
	src/UserInterface/SkipArrow.h
	src/UserInterface/Slider.cpp
	src/UserInterface/Slider.h
	src/UserInterface/Tab.cpp
	src/UserInterface/Tab.h
	src/Utils.cpp
	src/Utils.h
	src/Vector2.cpp
	src/Vector2.h
	src/Version.cpp
	src/Version.h
	src/Video.cpp
	src/Video.h
	src/XmlReader.cpp
	src/XmlReader.h
	src/XmlWriter.cpp
	src/XmlWriter.h
	src/enums.cpp
	src/enums.h
	src/globals.cpp
	src/globals.h
	src/main.cpp
	src/miniz.c
	src/miniz.h
	src/mli_audio.cpp
	src/mli_audio.h
	src/ticpp/ticpp.cpp
	src/ticpp/ticpp.h
	src/ticpp/ticpprc.h
	src/ticpp/tinystr.cpp
	src/ticpp/tinystr.h
	src/ticpp/tinyxml.cpp
	src/ticpp/tinyxml.h
	src/ticpp/tinyxmlerror.cpp
	src/ticpp/tinyxmlparser.cpp
)

# Directories for lib openbar-utils link
include_directories	(
	${SDL2_INCLUDE_DIR}
	${SDL2_IMAGE_INCLUDE_DIRS}
	${SDLMIXER_INCLUDE_DIR}
	#${FFMPEG_INCLUDE_DIR}
	${CRYPTO++_INCLUDE_DIR}
)
link_directories	(
	${FFMPEG_LIBRARY_DIR}
)

# Compile as an executable and link lib openbar-utils
add_executable( ${PROJECT_NAME} ${${PROJECT_NAME}_src} )
target_link_libraries( ${PROJECT_NAME}
	${SDL2_LIBRARY}
	${SDL2_IMAGE_LIBRARIES}
	${SDL2TTF_LIBRARY}
	${SDLMIXER_LIBRARY}
	${FFMPEG_LIBRARIES}
	${CRYPTO++_LIBRARIES}
)
set_target_properties( ${PROJECT_NAME} PROPERTIES
	COMPILE_DEFINITIONS GAME_EXECUTABLE
)

# Launcher ---------------------------------------------------------------------

# Project properties
project(${PROJECT}-launcher)

# Dependencies
find_package( SDL2			REQUIRED	)
find_package( Crypto++		REQUIRED	)
find_package( CURL			REQUIRED	)

# G++ Flags
set( CMAKE_CXX_FLAGS			${global_cxx_flags}			)
set( CMAKE_CXX_FLAGS_DEBUG		${global_cxx_flags_debug}	)
set( CMAKE_CXX_FLAGS_RELEASE	${global_cxx_flags_release}	)

# Sources files list
file( GLOB_RECURSE ${PROJECT_NAME}_src
	src/FileFunctions.cpp
	src/FileFunctions.h
	src/Utils.cpp
	src/Utils.h
	src/Version.cpp
	src/Version.h
	src/XmlReader.cpp
	src/XmlReader.h
	src/XmlWriter.cpp
	src/XmlWriter.h
	src/globals.cpp
	src/globals.h
	src/main.cpp
	src/ticpp/ticpp.cpp
	src/ticpp/ticpp.h
	src/ticpp/ticpprc.h
	src/ticpp/tinystr.cpp
	src/ticpp/tinystr.h
	src/ticpp/tinyxml.cpp
	src/ticpp/tinyxml.h
	src/ticpp/tinyxmlerror.cpp
	src/ticpp/tinyxmlparser.cpp
)

# Directories for lib openbar-utils link
include_directories	(
	${SDL2_INCLUDE_DIR}
	${CRYPTO++_INCLUDE_DIR}
	${CURL_INCLUDE_DIRS}
)

# Compile as an executable and link lib openbar-utils
add_executable( ${PROJECT_NAME} ${${PROJECT_NAME}_src} )
target_link_libraries( ${PROJECT_NAME}
	${SDL2_LIBRARY}
	${CRYPTO++_LIBRARIES}
	${CURL_LIBRARIES}
)
set_target_properties( ${PROJECT_NAME} PROPERTIES
	COMPILE_DEFINITIONS LAUNCHER
)

# Updater ----------------------------------------------------------------------

# Project properties
project(${PROJECT}-updater)

# Dependencies
find_package( SDL2			REQUIRED	)
find_package( SDL2_image	REQUIRED	)
find_package( SDL2_ttf		REQUIRED	)
find_package( Crypto++		REQUIRED	)
find_package( CURL			REQUIRED	)

# G++ Flags
set( CMAKE_CXX_FLAGS			${global_cxx_flags}			)
set( CMAKE_CXX_FLAGS_DEBUG		${global_cxx_flags_debug}	)
set( CMAKE_CXX_FLAGS_RELEASE	${global_cxx_flags_release}	)

# Sources files list
file( GLOB_RECURSE ${PROJECT_NAME}_src
	src/Color.cpp
	src/Color.h
	src/EasingFunctions.cpp
	src/EasingFunctions.h
	src/FileFunctions.cpp
	src/FileFunctions.h
	src/Font.cpp
	src/Font.h
	src/Game.cpp
	src/Game.h
	src/Image.cpp
	src/Image.h
	src/Rectangle.cpp
	src/Rectangle.h
	src/Screens/CheckForUpdatesScreen.cpp
	src/Screens/CheckForUpdatesScreen.h
	src/Screens/Screen.h
	src/State.h
	src/Utils.cpp
	src/Utils.h
	src/Vector2.cpp
	src/Vector2.h
	src/Version.cpp
	src/Version.h
	src/XmlReader.cpp
	src/XmlReader.h
	src/XmlWriter.cpp
	src/XmlWriter.h
	src/enums.cpp
	src/enums.h
	src/globals.cpp
	src/globals.h
	src/main.cpp
	src/ticpp/ticpp.cpp
	src/ticpp/ticpp.h
	src/ticpp/ticpprc.h
	src/ticpp/tinystr.cpp
	src/ticpp/tinystr.h
	src/ticpp/tinyxml.cpp
	src/ticpp/tinyxml.h
	src/ticpp/tinyxmlerror.cpp
	src/ticpp/tinyxmlparser.cpp
)

# Directories for lib openbar-utils link
include_directories	(
	${SDL2_INCLUDE_DIR}
	${SDL2_IMAGE_INCLUDE_DIRS}
	${CRYPTO++_INCLUDE_DIR}
	${CURL_INCLUDE_DIRS}
)

# Compile as an executable and link lib openbar-utils
add_executable( ${PROJECT_NAME} ${${PROJECT_NAME}_src} )
target_link_libraries( ${PROJECT_NAME}
	${SDL2_LIBRARY}
	${SDL2_IMAGE_LIBRARIES}
	${SDL2TTF_LIBRARY}
	${CRYPTO++_LIBRARIES}
	${CURL_LIBRARIES}
)
set_target_properties( ${PROJECT_NAME} PROPERTIES
	COMPILE_DEFINITIONS UPDATER
)

# ------------------------------------------------------------------------------

