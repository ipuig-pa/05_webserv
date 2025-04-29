NAME = webserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++17 -Iinc -Iinc/conf

SRC_DIR = src
OBJ_DIR = obj

VPATH =	$(SRC_DIR):\
		$(SRC_DIR)/conf

SOURCES =	ConfReader.cpp \
			ConfParser.cpp \
			ServerConf.cpp \
			LocationConf.cpp \
			MultiServer.cpp \
			Client.cpp \
			Socket.cpp \
			RequestHandler.cpp \
			HttpReqParser.cpp \
			HttpRequest.cpp \
			HttpResponse.cpp \
			Header.cpp \
			Status.cpp \
			CgiChecker.cpp \
			FileUtils.cpp \
			main.cpp

OBJECTS = $(SOURCES:%.cpp=$(OBJ_DIR)/%.o)
#$(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJECTS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJECTS)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -Iinc -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

# NAME = webserv

# SRCS = src/main.cpp
# OBJS = $(SRCS:.cpp=.o)

# CXX = c++
# CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Iinc

# all: $(NAME)

# $(NAME): $(OBJS)
# 	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

# clean:
# 	rm -f $(OBJS)

# fclean: clean
# 	rm -f $(NAME)

# re: fclean all