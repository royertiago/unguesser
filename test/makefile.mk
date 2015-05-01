testdir := $(dir $(lastword $(MAKEFILE_LIST)))

test := $(testdir)test
testsrc := $(shell find $(testdir) -name "*.cpp")

prog += $(test)
src += $(testsrc)
dep += $(testsrc:.cpp=.dep.mk)
# We will not expose the object files to the outside world
# because they are only used to form the executable test/test.

$(test): $(testsrc:.cpp=.o)

all : $(test)

.PHONY: test
test: $(test)
	$(test)


.PHONY: test-clean test-mostlyclean

mostlyclean: test-mostlyclean

# We will not remove test/test.o because it is very costly to rebuild.
test-mostlyclean:
	find $(testdir) -name "*.o" -! -wholename $(test).o -exec rm {} +

clean: test-clean
test-clean: test-mostlyclean
	rm -f $(test) $(test).o
