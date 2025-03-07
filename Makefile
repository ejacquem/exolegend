NAME = banger
SRC = main.cpp Vector2D.cpp

all: ${NAME}

${NAME}:
	c++ ${SRC} -o ${NAME}

clean:
	rm ${NAME}