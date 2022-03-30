add_executable(game
    main.cpp
    circle.cpp
    glad.c
)

target_link_libraries(game
PUBLIC
    glfw3
)