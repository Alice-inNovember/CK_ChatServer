#NAME------------------------------------------------------
NAME		=	ChatServer
#CMDS------------------------------------------------------
CXX			=	c++
RM			=	rm -rf
#FLAGS-----------------------------------------------------
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98
#FILES-----------------------------------------------------
SRCS		=	main.cpp
OBJS		=	$(SRCS:.cpp=.o)
#COLOR-----------------------------------------------------
C_OFF		=	"\033[0m"
C_RED		=	"\033[1;31m"
C_GRN		=	"\033[1;32m"
C_BLE		=	"\033[1;34m"
C_PLE		=	"\033[1;35m"
C_CYN		=	"\033[1;36m"
INFO		=
#PRINTER---------------------------------------------------
define P_STAT
	$(eval INFO = $(shell echo $(1)$(C_OFF)))
	$(info $(INFO))
endef
#KEY-------------------------------------------------------
all : $(NAME)
#NAME------------------------------------------------------
$(NAME) : $(OBJS)
	$(call P_STAT,$(C_BLE)Compiling)

	@$(CXX) $(CXXFLAGS) -o $@ $^
	$(call P_STAT,$(C_PLE)$(NAME)"   "✅)

	$(call P_STAT,$(C_GRN)Done!)
#OBJS------------------------------------------------------
%.o : %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@ 
#CLEAN-----------------------------------------------------
clean :
	$(call P_STAT,$(C_RED)Cleaning)

	@$(RM) $(OBJS)
	$(call P_STAT,$(C_CYN)Obj"          "✅)

	$(call P_STAT,$(C_GRN)Done!)
	$(info )
#FCLEAN----------------------------------------------------
fclean : clean
	$(call P_STAT,$(C_RED)Removing)

	@$(RM) $(NAME)
	$(call P_STAT,$(C_PLE)$(NAME)"   "✅)

	$(call P_STAT,$(C_GRN)Done!)
	$(info )
#RE--------------------------------------------------------
re :
	@$(MAKE) fclean
	@$(MAKE) all

.PHONY : all clean fclean re