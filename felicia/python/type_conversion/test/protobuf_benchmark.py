import timeit

from felicia.python.type_conversion.test.test_pb2 import Test, Test2
import protobuf_py

def runtime(name, f, *args):
    start = timeit.default_timer()
    f(*args)
    end = timeit.default_timer()
    print("{}: Took {}".format(name, end - start))

def f(constructor):
    content = "hello" * 1000000
    for _ in range(10000):
        test = constructor()
        test.id = 1
        test.content = content
        protobuf_py.test(test)

runtime("test", f, Test)
runtime("test2", f, Test2)

# test: Took 108.68263805300012
# test2: Took 108.37291221499981