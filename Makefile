# Compiler and flags
CC = g++ -std=c++20
AR = ar
CFLAGS = -g -Wall -Wextra -Werror -c
LDFLAGS = -lpthread

# Targets
TARGET2 = CoordinatorCore/libcoord.a
TARGET3 = CoordinatorCore/coordinator

# Object files
COORDINATOR_OBJS = CoordinatorCore/coord_start.o \
                   CoordinatorCore/coordDb.o \
                   Common/cmsgOp.o

# Default target
all: ${TARGET2} ${TARGET3}

# Static library target
${TARGET2}: ${COORDINATOR_OBJS}
	@echo "Building coordinator static library: $@"
	${AR} rcs ${TARGET2} ${COORDINATOR_OBJS}

# Executable target
${TARGET3}: CoordinatorCore/coord_main.o ${COORDINATOR_OBJS}
	@echo "Building coordinator executable: $@"
	${CC} -g CoordinatorCore/coord_main.o ${COORDINATOR_OBJS} -o ${TARGET3} ${LDFLAGS}

# Compilation rules
CoordinatorCore/coord_main.o: CoordinatorCore/coord_main.cpp CoordinatorCore/coordDb.h
	${CC} ${CFLAGS} CoordinatorCore/coord_main.cpp -o CoordinatorCore/coord_main.o

CoordinatorCore/coord_start.o: CoordinatorCore/coord_start.cpp
	${CC} ${CFLAGS} CoordinatorCore/coord_start.cpp -o CoordinatorCore/coord_start.o

CoordinatorCore/coordDb.o: CoordinatorCore/coordDb.cpp CoordinatorCore/coordDb.h
	${CC} ${CFLAGS} CoordinatorCore/coordDb.cpp -o CoordinatorCore/coordDb.o

Common/cmsgOp.o: Common/cmsgOp.cpp Common/cmsgOp.h
	${CC} ${CFLAGS} Common/cmsgOp.cpp -o Common/cmsgOp.o

# Clean target
.PHONY: clean
clean:
	rm -f CoordinatorCore/*.o
	rm -f CoordinatorCore/*.a
	rm -f Common/*.o
	rm -f CoordinatorCore/coordinator
