EXECS=matFact-mpi
MPICXX?=mpicxx -std=c++11

all: ${EXECS}

matFact-mpi: matFact-mpi.cpp src/computeB.cpp src/filterFinalMatrix.cpp src/initialLR.cpp src/readInput.cpp src/updateLR.cpp src/verifyResult.cpp
	${MPICXX} -o matFact-mpi matFact-mpi.cpp src/computeB.cpp src/filterFinalMatrix.cpp src/initialLR.cpp src/readInput.cpp src/updateLR.cpp src/verifyResult.cpp

clean:
	rm -f ${EXECS}