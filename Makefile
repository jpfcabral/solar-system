TARGET=solar
CXX=c++
LD=gcc
OBJS=solar.c
conv:$(OBJS)
	$(LD) -g -o $(TARGET) $(OBJS) -lglut -lGL -lGLU -lm
clean:
	rm -rf *.o