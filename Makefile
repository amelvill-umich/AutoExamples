all: autotest

# $^ is the list of prerequisites
# $@ is the target
autotest: autotest.cc
	g++ -o $@ $^ -g

clean:
	rm autotest