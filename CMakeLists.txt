cmake_minimum_required(VERSION 3.25)
project(CARTAGO VERSION 1.0 LANGUAGES CXX)

# Estanadar
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Fuente 
# Via: Glob carga todos los .cpp de manera automatica
# sin listarlos via manual
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Ejecutable
add_executable(CARTAGO ${SOURCES})


# Includes (Directorio para .hpp)
target_include_directories(CARTAGO PRIVATE include)

# Opciones de compilacion
target_compile_options(CARTAGO PRIVATE 
    $<$<CONFIG:Release>:
        /O2
        /MT
        /GL
    >
    $<$<CONFIG:Debug>:
        /Od
        /Zi
        /W4
    >    
)

target_link_options(CARTAGO PRIVATE
    $<$<CONFIG:Release>:
        /OPT:REF
        /OPT:ICF
        /LTCG   
    >
)