CC = g++
CFLAGS = $(shell xml2-config --cflags)

SDLLIB = $(shell sdl-config --libs)


LIBS = $(SDLLIB)  -lgameoflife
NAME = brians

OBJECTS = \
	./brians.o 
	

ALL: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJECTS) $(LIBS)
	
TEST: ALL
	./$(NAME)

CLEAN:
	rm $(OBJECTS) 

CLEAN2: CLEAN
	rm $(NAME)
NEW:
	make CLEAN -i
	make ALL      
       
%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
