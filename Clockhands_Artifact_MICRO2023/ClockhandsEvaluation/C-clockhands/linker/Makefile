bin := linker
.PHONY : all clean

all : $(bin)
clean :
	rm -f $(bin)

$(bin) : % : %.cpp
	g++ -std=c++17 -Wall -Wextra $< -o $@
