
NAME = ft_vox

FILES = main.cpp \
		VoxelGenerator.cpp \
		Utilities.cpp \
		Shader.cpp \
		Object.cpp \
		RectangularCuboid.cpp \
		Loop.cpp \
		World.cpp \
		Camera.cpp \
		Matrix.cpp \
		Vec3.cpp \
		Texture.cpp \
		appWindow.cpp

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

obj/%.o:src/%.cpp includes/*.h $(wildacrd $(src/%.hpp))
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -c $< -o $@ -Iincludes -Ilibft

clean :
	rm -rf obj

fclean : clean
	rm -rf $(NAME)

re : fclean all

