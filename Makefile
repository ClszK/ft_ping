NAME        = ft_ping
SRCS        = main.c \
							ping_utils.c \
							ping_init.c \
							ping_packet.c

OBJDIR      = build
OBJS        = $(patsubst %.c, $(OBJDIR)/%.o, $(SRCS))
DEPS        = $(patsubst %.c, $(OBJDIR)/%.d, $(SRCS))

CXX         = cc
CXXFLAGS    = -Wall -Wextra -Werror -MMD -MP

all:    $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

-include $(DEPS)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -rf $(NAME)

re: 
	make fclean
	make all

.PHONY: all clean fclean re
