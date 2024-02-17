SRC = src/main.cpp src/Server.cpp src/Client.cpp src/Channel.cpp 
# src/Bot.cpp src/BtcPrice.cpp
#  src/join.cpp

OBJ_DIR = obj

OBJ = $(patsubst %, $(OBJ_DIR)/%, $(notdir $(SRC:.cpp=.o)))

INC_DIR = inc

FLGS = -Wall -Wextra -Werror -std=c++98

NAME = ircserv

all : $(NAME)

$(NAME) : $(OBJ)
	c++ $(FLGS) $^ -o $@

$(OBJ_DIR)/%.o : src/%.cpp $(INC_DIR)
	mkdir -p $(OBJ_DIR)
	c++ $(FLGS) -I$(INC_DIR) -c $< -o $@

clean :
	rm -rf $(OBJ_DIR)

fclean : clean
	rm -rf $(NAME)

re  : clean fclean all

.PHONY : all clean fclean re

.SILENT : $(NAME) $(OBJ) clean fclean re all