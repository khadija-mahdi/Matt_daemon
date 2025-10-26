# Unified Makefile for Bonus/ project and client subproject
NAME = Matt_daemon
CLIENT_NAME = Ben_AFK
CXX = g++
CXXFLAGS = -Wall -Wextra -g

# Libraries (libcurl needed by SmtpEmail)
LDLIBS = -lcurl

# Main (Bonus) sources (exclude client/ folder)
MAIN_SRCS := $(shell find . -maxdepth 1 -name '*.cpp' -not -path './client/*')
MAIN_OBJS := $(patsubst %.cpp,objects/main/%.o,$(notdir $(MAIN_SRCS)))

# Client sources: prefer sources in client/; add selected parent sources only if not present in client/
CLIENT_LOCAL_SRCS := $(shell find client -maxdepth 1 -name '*.cpp' 2>/dev/null)
CLIENT_PARENT_CANDIDATES := helper.cpp server.cpp tintin_reporter.cpp SmtpEmail.cpp LogArchiver.cpp
CLIENT_PARENT_SRCS := $(foreach f,$(CLIENT_PARENT_CANDIDATES),$(if $(wildcard client/$(f)),,$(f)))
CLIENT_SRCS := $(CLIENT_LOCAL_SRCS) $(CLIENT_PARENT_SRCS)
CLIENT_OBJS := $(patsubst %.cpp,objects/client/%.o,$(notdir $(CLIENT_SRCS)))

.PHONY: all client clean fclean re

all: $(NAME)

# Build main objects
objects/main/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME): $(MAIN_OBJS)
	@echo "Compiling $(NAME)..."
	$(CXX) $(CXXFLAGS) -o $(NAME) $(MAIN_OBJS) $(LDLIBS)
	@echo "$(NAME) compiled."

client: $(CLIENT_NAME)

# Build client objects; compile from client/ if present, otherwise from top-level
objects/client/%.o: 
	@mkdir -p $(dir $@)
	@if [ -f client/$*.cpp ]; then \
		$(CXX) $(CXXFLAGS) -c client/$*.cpp -o $@; \
	else \
		$(CXX) $(CXXFLAGS) -c $*.cpp -o $@; \
	fi

$(CLIENT_NAME): $(CLIENT_OBJS)
	@echo "Compiling $(CLIENT_NAME)..."
	$(CXX) $(CXXFLAGS) -o $(CLIENT_NAME) $(CLIENT_OBJS) $(LDLIBS)
	@echo "$(CLIENT_NAME) compiled."

clean:
	@rm -rf objects

fclean: clean
	@rm -f $(NAME) $(CLIENT_NAME)

re: fclean all client