# Warning! This code was automatically generated for projects
# without default 'CMakeLists.txt' file.

set(app_sources
	"main.cpp"
    "rmt1.cpp"
    "../scenario/scenario.cpp"
    "../scenario/scenario1.cpp"
    "../scenario/scenario2.cpp"
)

set(includes
	"."
    "../scenario"
    "../include"
)

set(reqs
    nvs_flash
)

set(files 

)


idf_component_register(SRCS ${app_sources} INCLUDE_DIRS ${includes} REQUIRES ${reqs} EMBED_FILES ${files})
