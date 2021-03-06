IDIR=./include
IOPENCV=C:/dev/OpenCV-MinGW-Build-OpenCV-4.1.1-x64/include
LOPENCV=C:/dev/OpenCV-MinGW-Build-OpenCV-4.1.1-x64/x64/mingw/bin
LDLIBS= -llibopencv_core411 -llibopencv_highgui411 -llibopencv_imgcodecs411 -llibopencv_imgproc411

CC=g++
CFLAGS=-Wall -g -I ${IDIR} -I ${IOPENCV} -L ${LOPENCV} ${LDLIBS}
DEPS = $(wildcard ${IDIR}/*.hpp)
SOURCES= $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp, %.o, ${SOURCES})

.PHONY: all clean

default: all

all: lomo

%.o: %.cpp $(DEPS)
	${CC} -c $^ -I ${IDIR} -I ${IOPENCV}

lomo: $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	$(RM) $(OBJECTS) ${wildcard ${IDIR}/*.gch}

destroy:
	$(RM) $(OBJECTS) lomo ${wildcard ${IDIR}/*.gch}

# https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
