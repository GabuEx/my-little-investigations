TEMPLATE = app
TARGET = MLI
INCLUDEPATH += .

# Input
HEADERS += osx/ApplicationSupportBridge.h \
           osx/NSFileManagerDirectoryLocations.h \
           src/Animation.h \
           src/AnimationSound.h \
           src/Collisions.h \
           src/Color.h \
           src/Condition.h \
           src/EasingFunctions.h \
           src/enums.h \
           src/FileFunctions.h \
           src/Game.h \
           src/globals.h \
           src/HeightMap.h \
           src/Image.h \
           src/Interfaces.h \
           src/KeyboardHelper.h \
           src/Line.h \
           src/miniz.h \
           src/mli_audio.h \
           src/MLIFont.h \
           src/MouseHelper.h \
           src/Polygon.h \
           src/PositionalSound.h \
           src/Rectangle.h \
           src/resource.h \
           src/ResourceLoader.h \
           src/Sprite.h \
           src/State.h \
           src/TextInputHelper.h \
           src/TransitionRequest.h \
           src/Utils.h \
           src/Vector2.h \
           src/Version.h \
           src/Video.h \
           src/XmlReader.h \
           src/XmlWriter.h \
           src/CaseContent/Area.h \
           src/CaseContent/Conversation.h \
           src/CaseContent/Crowd.h \
           src/CaseContent/Dialog.h \
           src/CaseContent/Encounter.h \
           src/CaseContent/FieldCharacter.h \
           src/CaseContent/FieldCutscene.h \
           src/CaseContent/ForegroundElement.h \
           src/CaseContent/Location.h \
           src/CaseContent/Notification.h \
           src/CaseContent/ZoomedView.h \
           src/CaseInformation/AnimationManager.h \
           src/CaseInformation/AudioManager.h \
           src/CaseInformation/Case.h \
           src/CaseInformation/CommonCaseResources.h \
           src/CaseInformation/ContentManager.h \
           src/CaseInformation/DialogCharacterManager.h \
           src/CaseInformation/DialogCutsceneManager.h \
           src/CaseInformation/EvidenceManager.h \
           src/CaseInformation/FieldCharacterManager.h \
           src/CaseInformation/FieldCutsceneManager.h \
           src/CaseInformation/FlagManager.h \
           src/CaseInformation/FontManager.h \
           src/CaseInformation/PartnerManager.h \
           src/CaseInformation/SpriteManager.h \
           src/Events/ButtonArrayEventProvider.h \
           src/Events/ButtonEventProvider.h \
           src/Events/CaseParsingEventProvider.h \
           src/Events/DialogEventProvider.h \
           src/Events/EventProviders.h \
           src/Events/EvidenceSelectorEventProvider.h \
           src/Events/ImageButtonEventProvider.h \
           src/Events/LocationEventProvider.h \
           src/Events/PromptOverlayEventProvider.h \
           src/Events/SelectorEventProvider.h \
           src/Screens/CheckForUpdatesScreen.h \
           src/Screens/GameScreen.h \
           src/Screens/LogoScreen.h \
           src/Screens/MLIScreen.h \
           src/Screens/OptionsScreen.h \
           src/Screens/SelectionScreen.h \
           src/Screens/TitleScreen.h \
           src/UserInterface/Arrow.h \
           src/UserInterface/ButtonArray.h \
           src/UserInterface/EvidenceDescription.h \
           src/UserInterface/EvidenceSelector.h \
           src/UserInterface/ImageButton.h \
           src/UserInterface/PartnerInformation.h \
           src/UserInterface/PromptOverlay.h \
           src/UserInterface/Selector.h \
           src/UserInterface/SkipArrow.h \
           src/UserInterface/Slider.h \
           src/UserInterface/Tab.h \
    src/utf8cpp/utf8/checked.h \
    src/utf8cpp/utf8/core.h \
    src/utf8cpp/utf8/unchecked.h \
    src/utf8cpp/utf8.h \
    src/MLIException.h \


SOURCES += src/Animation.cpp \
           src/AnimationSound.cpp \
           src/Collisions.cpp \
           src/Color.cpp \
           src/Condition.cpp \
           src/EasingFunctions.cpp \
           src/enums.cpp \
           src/FileFunctions.cpp \
           src/Game.cpp \
           src/globals.cpp \
           src/HeightMap.cpp \
           src/Image.cpp \
           src/Interfaces.cpp \
           src/KeyboardHelper.cpp \
           src/Line.cpp \
           src/main.cpp \
           src/miniz.c \
           src/mli_audio.cpp \
           src/MLIFont.cpp \
           src/MouseHelper.cpp \
           src/Polygon.cpp \
           src/Rectangle.cpp \
           src/ResourceLoader.cpp \
           src/Sprite.cpp \
           src/State.cpp \
           src/TextInputHelper.cpp \
           src/Utils.cpp \
           src/Vector2.cpp \
           src/Version.cpp \
           src/Video.cpp \
           src/XmlReader.cpp \
           src/XmlWriter.cpp \
           src/CaseContent/Area.cpp \
           src/CaseContent/Conversation.cpp \
           src/CaseContent/Crowd.cpp \
           src/CaseContent/Dialog.cpp \
           src/CaseContent/Encounter.cpp \
           src/CaseContent/FieldCharacter.cpp \
           src/CaseContent/FieldCutscene.cpp \
           src/CaseContent/ForegroundElement.cpp \
           src/CaseContent/Location.cpp \
           src/CaseContent/Notification.cpp \
           src/CaseContent/ZoomedView.cpp \
           src/CaseInformation/AnimationManager.cpp \
           src/CaseInformation/AudioManager.cpp \
           src/CaseInformation/Case.cpp \
           src/CaseInformation/CommonCaseResources.cpp \
           src/CaseInformation/ContentManager.cpp \
           src/CaseInformation/DialogCharacterManager.cpp \
           src/CaseInformation/DialogCutsceneManager.cpp \
           src/CaseInformation/EvidenceManager.cpp \
           src/CaseInformation/FieldCharacterManager.cpp \
           src/CaseInformation/FieldCutsceneManager.cpp \
           src/CaseInformation/FlagManager.cpp \
           src/CaseInformation/FontManager.cpp \
           src/CaseInformation/PartnerManager.cpp \
           src/CaseInformation/SpriteManager.cpp \
           src/Events/EventProviders.cpp \
           src/Screens/CheckForUpdatesScreen.cpp \
           src/Screens/GameScreen.cpp \
           src/Screens/LogoScreen.cpp \
           src/Screens/OptionsScreen.cpp \
           src/Screens/SelectionScreen.cpp \
           src/Screens/TitleScreen.cpp \
           src/UserInterface/Arrow.cpp \
           src/UserInterface/ButtonArray.cpp \
           src/UserInterface/EvidenceDescription.cpp \
           src/UserInterface/EvidenceSelector.cpp \
           src/UserInterface/ImageButton.cpp \
           src/UserInterface/PartnerInformation.cpp \
           src/UserInterface/PromptOverlay.cpp \
           src/UserInterface/Selector.cpp \
           src/UserInterface/SkipArrow.cpp \
           src/UserInterface/Slider.cpp \
           src/UserInterface/Tab.cpp \
    src/MLIException.cpp \
    src/tinyxml2/tinyxml2.cpp

LIBS += -m32 \
        -lmingw32 \
        -lshlwapi \
        -lSDL2main \
        -lSDL2 \
        -lSDL2_image \
        -lSDL2_ttf \
        -lSDL2_mixer \
        -lavcodec \
        -lavformat \
        -lavutil \
        -lswscale \
        -lcryptopp

DEFINES +=  \
    GAME_EXECUTABLE

DEFINES -= UNICODE

QMAKE_CXXFLAGS += \
    -m32 \
    -D__STDC_CONSTANT_MACROS \
    -D__STDC_LIMIT_MACROS \
    -D__WINDOWS \
    -DTIXML_USE_TICPP \
    -w

OTHER_FILES += \
    src/resource.rc
