add_executable(game
    main.cpp
    circle.cpp
    grid.cpp
    glad.c
)

target_link_libraries(game
PUBLIC
    glfw3
)