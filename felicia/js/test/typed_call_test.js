const typedCall = require("../../../bazel-bin/felicia/js/test/typed_call.node");

function expect(v, v2) {
  if (typeof v === "number") {
    if (Math.abs(v - v2) > Math.EPSILLON) {
      throw new Error(`${v} and ${v2} is not equal.`);
    }
    return;
  }
  if (v !== v2) throw new Error(`${v} and ${v2} is not equal.`);
}

const test = new typedCall.Test();
test.void_return();
expect(test.bool_return(false), true);
expect(test.short_return(120), 240);
expect(test.int_return(120), 360);
expect(test.float_return(120.3), 481.2);
expect(test.double_return(120.3), 601.5);
expect(test.const_char_ptr_return("dodo"), "hello");
expect(test.string_return("hello"), "hello world");
expect(test.string_ref_return("hello"), "thank you");
typedCall.Test.s_void_return();
expect(typedCall.Test.s_int_return(120), 360);
expect(typedCall.Test.s_double_return(120.3), 601.5);
expect(typedCall.Test.s_string_return("hello"), "hello world");

function expect_throw(f, message) {
  let catched = false;
  try {
    f();
  } catch (e) {
    catched = true;
    expect(e.message, message);
  }

  expect(catched, true);
}

expect_throw(function() {
  test.int_return("abc");
}, "A number was expected");
expect_throw(function() {
  test.int_return(123, 456);
}, "Wrong number of arguments");
