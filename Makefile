COMP = g++-5.3.0
#put all your object files here
OBJS = main.o
#The executabel filename DON'T CHANGE
EXEC = ex2
CPP_COMP_FLAG = -std=gnu++14 -g -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

$(EXEC): $(OBJS)
	$(COMP) $(OBJS) -o $@
#a rule for building a simple c++ source file
#use g++ -MM main.cpp to see dependencies

main.o: main.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

clean:
	rm -f $(OBJS) $(EXEC)