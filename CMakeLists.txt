cmake_minimum_required (VERSION 2.8)

project (gara_tra_i_labirinti)

set (ALLEGRO_VERSION "5.0.10")

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -iquote ${PROJECT_SOURCE_DIR}/")

find_package (Allegro5)
find_package (Allegro5ACodec)
find_package (Allegro5Audio)
find_package (Allegro5Font)
find_package (Allegro5Image)
find_package (Allegro5Main)
find_package (Allegro5Primitives)

if (NOT ALLEGRO5_FOUND OR NOT ALLEGRO5_ACODEC_FOUND OR NOT ALLEGRO5_AUDIO_FOUND OR NOT ALLEGRO5_FONT_FOUND OR NOT ALLEGRO5_IMAGE_FOUND OR NOT ALLEGRO5_PRIMITIVES_FOUND)
    message(FATAL_ERROR "Per la compilazione è necessaria la libreria grafica Allegro 5")
else (NOT ALLEGRO5_FOUND OR NOT ALLEGRO5_ACODEC_FOUND OR NOT ALLEGRO5_AUDIO_FOUND OR NOT ALLEGRO5_FONT_FOUND OR NOT ALLEGRO5_IMAGE_FOUND OR NOT ALLEGRO5_PRIMITIVES_FOUND)
    include_directories (
                            ${ALLEGRO5_INCLUDE_DIR}
                            ${ALLEGRO5_ACODEC_INCLUDE_DIR} 
                            ${ALLEGRO5_AUDIO_INCLUDE_DIR} 
                            ${ALLEGRO5_FONT_INCLUDE_DIR}
                            ${ALLEGRO5_IMAGE_INCLUDE_DIR}
                            ${ALLEGRO5_MAIN_INCLUDE_DIR}
                            ${ALLEGRO5_PRIMITIVES_INCLUDE_DIR})
endif (NOT ALLEGRO5_FOUND OR NOT ALLEGRO5_ACODEC_FOUND OR NOT ALLEGRO5_AUDIO_FOUND OR NOT ALLEGRO5_FONT_FOUND OR NOT ALLEGRO5_IMAGE_FOUND OR NOT ALLEGRO5_PRIMITIVES_FOUND)

set (SOURCES
    main.c
    types.c
    config/config.c
    game/ai.c
    game/cell.c
    game/character.c
    game/events.c
    game/game.c
    game/info.c
    game/intro.c
    game/level.c
    game/map.c
    game/powerup.c
    game/animations.c
    io/keyboard.c
    main/audio.c
    main/drawing.c
    main/graphics.c
    main/image.c
    main/output.c
    main/tiling.c
    main/timer.c
    misc/geometry.c
    misc/random.c
    main/fs.c
    parser/buffer_parser.c
    parser/file_parser.c
    parser/parser.c
    pathfinding/astar.c
    pathfinding/bfs.c
    pathfinding/dijkstra.c
    pathfinding/path.c
    std/hashtable.c
    std/list.c
    std/priority_queue.c
)

SET (RESOURCES
    assets/config.cfg
    assets/powerups.cfg
    assets/enemies.bmp
    assets/user.bmp
    assets/heart.bmp
    assets/bonus.bmp
    assets/font.bmp
    assets/logo.bmp
    assets/audio/crash.wav
    assets/audio/freezing.wav
    assets/audio/gameover.wav
    assets/audio/super.wav
    assets/audio/intro.mod
    assets/audio/speed.wav
    assets/audio/teleport.wav
    assets/audio/wall.wav
    assets/audio/won.wav
    assets/audio/intro.mod
    assets/audio/levels/A.mod
    assets/audio/levels/B.mod
    assets/audio/levels/C.mod
    assets/audio/levels/D.mod
    assets/audio/levels/E.mod
    assets/audio/levels/F.mod
    assets/audio/levels/G.mod
    assets/levels/backgrounds.bmp
    assets/levels/backgrounds_1.bmp
    assets/levels/walls.bmp
    assets/levels/walls_1.bmp
    assets/levels/A/A.cfg
    assets/levels/A/map.cfg
    assets/levels/B/B.cfg
    assets/levels/C/C.cfg
    assets/levels/D/D.cfg
    assets/levels/E/E.cfg
    assets/levels/F/F.cfg
    assets/levels/G/G.cfg
    )

set (LIBS
    m)

set (ALLEGRO_LIBS
    ${ALLEGRO5_LIBRARIES}
    ${ALLEGRO5_ACODEC_LIBRARIES}
    ${ALLEGRO5_AUDIO_LIBRARIES}
    ${ALLEGRO5_FONT_LIBRARIES}
    ${ALLEGRO5_IMAGE_LIBRARIES}
    ${ALLEGRO5_MAIN_LIBRARIES}
    ${ALLEGRO5_PRIMITIVES_LIBRARIES})

include_directories ("${CMAKE_MODULE_PATH}")

if (WIN32)

    add_definitions (-D_GNU_SOURCE=1)

    set (LIBS 
        ${LIBS} 
        gdiplus.lib
        uuid.lib
        kernel32.lib
        winmm.lib
        psapi.lib
        opengl32.lib
        glu32.lib
        user32.lib
        comdlg32.lib
        gdi32.lib
        shell32.lib
        ole32.lib
        advapi32.lib
        ws2_32.lib
        shlwapi.lib
        stdc++)

    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc -Wl,--subsystem,console")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -Wl,--subsystem,console")

endif (WIN32)

if (APPLE)

    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")

    add_executable (Gara MACOSX_BUNDLE ${SOURCES} ${RESOURCES})

    set (MACOSX_BUNDLE_GUI_IDENTIFIER "Gara tra i labirinti")

    foreach (_file ${RESOURCES})
        GET_FILENAME_COMPONENT(_file_dir ${_file} PATH)
        SET_SOURCE_FILES_PROPERTIES( ${_file} PROPERTIES 
                       MACOSX_PACKAGE_LOCATION 
                       "Resources/${_file_dir}" )
    endforeach (_file)

else (NOT APPLE)
   
    add_executable (Gara WIN32 ${SOURCES})

    file (COPY assets DESTINATION ${CMAKE_BINARY_DIR})

endif (APPLE)

target_link_libraries (Gara 
                        ${ALLEGRO5_LIBRARIES}
                        ${ALLEGRO5_ACODEC_LIBRARIES}
                        ${ALLEGRO5_AUDIO_LIBRARIES}
                        ${ALLEGRO5_FONT_LIBRARIES}
                        ${ALLEGRO5_IMAGE_LIBRARIES}
                        ${ALLEGRO5_MAIN_LIBRARIES}
                        ${ALLEGRO5_PRIMITIVES_LIBRARIES}
                        ${LIBS})


