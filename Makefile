OS_NAME = $(shell uname)
ifeq ($(OS_NAME),Linux)
	ECHOc = echo -e
else
	ECHOc = echo
endif

NATH = nathan
CAMPUS = clusters.42paris.fr

ifeq ($(findstring $(NATH), $(shell hostname)), $(NATH))
	Green=\033[32m
	Cyan=\033[36m
	Red=\033[31m
	End=\033[0m
else ifeq ($(findstring $(NATH), $(shell hostname)),)
	Green=\033[32m
	Cyan=\033[36m
	Red=\033[31m
	End=\033[0m
else
	Green=\x1b[32m
	Cyan=\x1b[36m
	Red=\x1b[31m
	End=\x1b[0m
endif

NAME = ft_vox

FILES = main.cpp \
		VoxelGenerator.cpp \
		Shader.cpp \
		Object.cpp \
		RectangularCuboid.cpp \
		Loop.cpp \
		World.cpp \
		Camera.cpp \
		Matrix.cpp \
		Vec2.cpp \
		Vec3.cpp \
		Texture.cpp \
		Skybox.cpp \
		appWindow.cpp \
		Chunk.cpp

HEADERS = $(wildcard $(addprefix src/, $(FILES:.cpp=.hpp)))

OBJ = $(addprefix obj/,$(FILES:.cpp=.o))

#linkage
LIBS = -lglfw -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -lGL -lpthread -ldl `pkg-config --libs glew` -lm #-pg #-fsanitize=address

CXXFLAGS = -std=gnu++11 -Wall -Wextra -g #-pg #-fsanitize=address

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $^ -o $@ $(LIBS)
	@$(ECHOc) "Compilation of $(Cyan)$(NAME)$(End) :    $(Green)Done$(End)"

obj/%.o:src/%.cpp includes/*.h src/*.hpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -c $< -o $@ -Iincludes -Ilibft -I$(HOME)/.brew/include
	@$(ECHOc) "Compilation of $(Cyan)$@$(End) :    $(Green)Done$(End)"

clean :
	rm -rf obj
	@$(ECHOc) "$(Red)Deletion of object file $@$(End) :    $(Green)Done$(End)"

fclean : clean
	rm -rf $(NAME)
	@$(ECHOc) "$(Red)Deletion of binary $@$(End) :    $(Green)Done$(End)"

re : fclean all

