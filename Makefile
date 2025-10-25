NAME = MattDaemon

SRCS = main.cpp
OBJ_D = obj

OBJS = $(addprefix $(OBJ_D)/,${SRCS:.cpp=.o}) 

CPP = c++ -std=c++98


all : ${NAME}

$(OBJ_D)/%.o: %.cpp | ${OBJ_D}
	@$(CPP) $(CFLAGS) -c $< -o $@ -g

${NAME} : ${OBJS}
	@$(CPP) ${OBJS} -o $(NAME)
	@echo  "\033[91m|  Compilation Done  |\033[00m"

${OBJ_D} :
	@mkdir ${OBJ_D}

clean:
	@rm -rf ${OBJ_D}
	@echo "\033[92m|  CLEANED  |\033[00m"

fclean: clean
	@rm -rf ${NAME}
	@echo "\033[92m|  EXECUTABLE REMOVED  |\033[00m"

re: fclean all