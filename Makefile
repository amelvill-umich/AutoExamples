all: autotest

# $^ is the list of prerequisites
# $@ is the target
autotest: autotest.cc
	g++ -o $@ $^ -g -fconcepts

clean:
	rm autotest
