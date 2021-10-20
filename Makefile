OS_NAME = $(shell uname)
ifeq ($(OS_NAME),Linux)
	ECHOc = echo -e
else
	ECHOc = echo
endif

Green=\x1b[32m
Cyan=\x1b[36m
Red=\x1b[31m
End=\x1b[0m

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
		Vec3.cpp \
		Texture.cpp \
		appWindow.cpp \
		Chunk.cpp

OBJ = $(addprefix obj/,$(FILES:.cpp=.o))

#linkage
LIBS = -lglfw -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -lGL -lpthread -ldl `pkg-config --libs glew` -lm

UNAME = $(shell uname -s)
ifneq (, $(findstring MINGW, $(UNAME)))
	LIBS = -lglfw3 -lgdi32 -lglew32 -lopengl32
	NAME = humangl.exe
endif

CXXFLAGS = -std=gnu++11 -Wall -Wextra -g

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $^ -o $@ $(LIBS)
	@$(ECHOc) "Compilation of $(Cyan)$(NAME)$(End) :    $(Green)Done$(End)"

obj/%.o:src/%.cpp includes/*.h $(test -f src/%.hpp)
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -c $< -o $@ -Iincludes -Ilibft
	@$(ECHOc) "Compilation of $(Cyan)$@$(End) :    $(Green)Done$(End)"

clean :
	rm -rf obj
	@$(ECHOc) "$(Red)Deletion of object file $@$(End) :    $(Green)Done$(End)"

fclean : clean
	rm -rf $(NAME)
	@$(ECHOc) "$(Red)Deletion of binary $@$(End) :    $(Green)Done$(End)"

re : fclean all

