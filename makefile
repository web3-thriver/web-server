SRC = main.cpp src/ConfigFile.cpp src/Location.cpp src/Server.cpp src/parseConfig.cpp\
		src/parseConfigUtils.cpp src/serverUtils.cpp src/RequestUtils.cpp src/setRequest.cpp\
		src/sendResponse.cpp src/methods.cpp


HEADERS = headers/ConfigFile.hpp headers/Location.hpp headers/Server.hpp headers/parseConfig.hpp headers/ServerUtils.hpp

NAME = websrv

PATH_OBJ = obj/

CC = c++

FLAGS = -Wall -Wextra -Werror -g -std=c++98

OBJ = $(addprefix $(PATH_OBJ), $(SRC:.cpp=.o))

all: $(NAME)

$(NAME): $(OBJ) $(HEADERS)
	$(CC) -g $(FLAGS) -o $(NAME) $(SRC)

$(OBJ) : $(PATH_OBJ)%.o: %.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	${CC} ${FLAGS} -c $< -o $@

clean:
	rm -rf $(PATH_OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all