NAME        = ft_ping
SRCS        = main.c \
							ping_utils.c \
							ping_init.c \
							ping_packet.c

OBJDIR      = build
OBJS        = $(patsubst %.c, $(OBJDIR)/%.o, $(SRCS))
DEPS        = $(patsubst %.c, $(OBJDIR)/%.d, $(SRCS))

CC 	        = gcc
CFLAGS    = -Wall -Wextra -Werror -MMD -MP
LDFLAGS		= -lm

all:    $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS)-o $(NAME)

-include $(DEPS)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CXX) $(CFLAGS) -c $< -o $@

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
