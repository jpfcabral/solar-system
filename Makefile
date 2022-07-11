TARGET=solar
CXX=c++
LD=gcc
OBJS=solar.c
solar:$(OBJS)
	$(LD) -g -o $(TARGET) $(OBJS) -lglut -lGL -lGLU -lm
clean:
	rm -rf *.o