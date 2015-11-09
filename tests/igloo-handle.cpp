#include <handle.h>
#include <igloo-listener.h>
#include <igloo/igloo_alt.h>
#include <array.h>
#include <object.h>

using namespace igloo;
using namespace valueobj;

#define NAME "some name"

#define __DescribeHandleTpl(__SET_TYPE, __GET_TYPE, __ANY_DESCRIPTION, IN, INITIALIZATION) \
  Describe(test_ ## __ANY_DESCRIPTION ## _set_and_get) \
  { \
    void SetUp() \
    { \
    } \
    \
    It(set_explicit_value_correctly) \
    { \
      Handle aHdl; \
      aHdl.set<__SET_TYPE>(IN); \
      Assert::That(aHdl.get<__GET_TYPE>(), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(aHdl.get()))->get(), Equals(IN)); \
    } \
    \
    It(set_explicit_name_value_correctly) \
    { \
      Handle aHdl; \
      aHdl.set<__SET_TYPE>(IN, NAME); \
      Assert::That(aHdl.get<__GET_TYPE>(), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(aHdl.get()))->get(), Equals(IN)); \
      Assert::That(((aHdl.get()))->getName(), Equals(NAME)); \
    } \
    \
    It(set_via_handle_explicit_value_correctly) \
    { \
      Handle aHdl; \
      Handle bHdl; \
      bHdl.set<__SET_TYPE>(IN); \
      aHdl.set(bHdl); \
      Assert::That(aHdl.get<__GET_TYPE>(), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(aHdl.get()))->get(), Equals(IN)); \
    } \
    \
    It(set_via_handle_explicit_name_value_correctly) \
    { \
      Handle aHdl; \
      Handle bHdl; \
      bHdl.set<__SET_TYPE>(IN, NAME); \
      aHdl.set(bHdl); \
      Assert::That(aHdl.get<__GET_TYPE>(), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(aHdl.get()))->get(), Equals(IN)); \
      Assert::That(((aHdl.get()))->getName(), Equals(NAME)); \
    } \
    \
    It(set_via_valuebase_explicit_value_correctly) \
    { \
      Handle aHdl; \
      Handle bHdl; \
      bHdl.set<__SET_TYPE>(IN); \
      aHdl.set(bHdl.get()); \
      Assert::That(aHdl.get<__GET_TYPE>(), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(aHdl.get()))->get(), Equals(IN)); \
    } \
    \
    It(set_via_valuebase_explicit_name_value_correctly) \
    { \
      Handle aHdl; \
      Handle bHdl; \
      bHdl.set<__SET_TYPE>(IN, NAME); \
      aHdl.set(bHdl.get()); \
      Assert::That(aHdl.get<__GET_TYPE>(), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(aHdl.get()))->get(), Equals(IN)); \
      Assert::That(((aHdl.get()))->getName(), Equals(NAME)); \
    } \
    \
    It(set_implicit_value_correctly) \
    { \
      Handle aHdl; \
      aHdl.set(IN); \
      Assert::That(aHdl.get<__GET_TYPE>(), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(aHdl.get()))->get(), Equals(IN)); \
    } \
    \
    It(set_implicit_name_value_correctly) \
    { \
      Handle aHdl; \
      aHdl.set(IN, NAME); \
      Assert::That(aHdl.get<__GET_TYPE>(), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(aHdl.get()))->get(), Equals(IN)); \
      Assert::That(((aHdl.get()))->getName(), Equals(NAME)); \
    } \
    \
    It(set_via_handle_implicit_value_correctly) \
    { \
      Handle aHdl; \
      Handle bHdl; \
      bHdl.set(IN); \
      aHdl.set(bHdl); \
      Assert::That(aHdl.get<__GET_TYPE>(), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(aHdl.get()))->get(), Equals(IN)); \
    } \
    \
    It(set_via_handle_implicit_name_value_correctly) \
    { \
      Handle aHdl; \
      Handle bHdl; \
      bHdl.set(IN, NAME); \
      aHdl.set(bHdl); \
      Assert::That(aHdl.get<__GET_TYPE>(), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(aHdl.get()))->get(), Equals(IN)); \
      Assert::That(((aHdl.get()))->getName(), Equals(NAME)); \
    } \
    \
    It(set_via_valuebase_implicit_value_correctly) \
    { \
      Handle aHdl; \
      Handle bHdl; \
      bHdl.set(IN); \
      aHdl.set(bHdl.get()); \
      Assert::That(aHdl.get<__GET_TYPE>(), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(aHdl.get()))->get(), Equals(IN)); \
    } \
    \
    It(set_via_valuebase_implicit_name_value_correctly) \
    { \
      Handle aHdl; \
      Handle bHdl; \
      bHdl.set(IN, NAME); \
      aHdl.set(bHdl.get()); \
      Assert::That(aHdl.get<__GET_TYPE>(), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(aHdl.get()))->get(), Equals(IN)); \
      Assert::That(((aHdl.get()))->getName(), Equals(NAME)); \
    } \
    \
    It(constructor_implicit_value_correctly) \
    { \
      Handle aHdl(IN); \
      Assert::That(aHdl.get<__GET_TYPE>(), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(aHdl.get()))->get(), Equals(IN)); \
    } \
    \
    It(constructor_implicit_name_value_correctly) \
    { \
      Handle aHdl(IN, NAME); \
      Assert::That(aHdl.get<__GET_TYPE>(), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(aHdl.get()))->get(), Equals(IN)); \
      Assert::That(((aHdl.get()))->getName(), Equals(NAME)); \
    } \
    \
    It(constructor_via_handle_implicit_value_correctly) \
    { \
      Handle bHdl(IN); \
      Handle aHdl(bHdl); \
      Assert::That(aHdl.get<__GET_TYPE>(), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(aHdl.get()))->get(), Equals(IN)); \
    } \
    \
    It(constructor_via_handle_implicit_name_value_correctly) \
    { \
      Handle bHdl(IN, NAME); \
      Handle aHdl(bHdl); \
      Assert::That(aHdl.get<__GET_TYPE>(), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(aHdl.get()))->get(), Equals(IN)); \
      Assert::That(((aHdl.get()))->getName(), Equals(NAME)); \
    } \
    \
    It(constructor_via_valuebase_implicit_value_correctly) \
    { \
      Handle bHdl(IN); \
      Handle aHdl(bHdl.get()); \
      Assert::That(aHdl.get<__GET_TYPE>(), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(aHdl.get()))->get(), Equals(IN)); \
    } \
    \
    It(constructor_via_valuebase_implicit_name_value_correctly) \
    { \
      Handle bHdl(IN, NAME); \
      Handle aHdl(bHdl.get()); \
      Assert::That(aHdl.get<__GET_TYPE>(), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(aHdl.get()))->get(), Equals(IN)); \
      Assert::That(((aHdl.get()))->getName(), Equals(NAME)); \
    } \
    \
    It(assignment_operator_implicit_value_correctly) \
    { \
      Handle aHdl = IN; \
      Assert::That(aHdl.get<__GET_TYPE>(), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(aHdl.get()))->get(), Equals(IN)); \
    } \
    \
    It(assignment_operator_via_handle_implicit_value_correctly) \
    { \
      Handle bHdl = IN; \
      Handle aHdl = bHdl; \
      Assert::That(aHdl.get<__GET_TYPE>(), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(aHdl.get()))->get(), Equals(IN)); \
    } \
    \
    It(assignment_operator_via_valuebase_implicit_value_correctly) \
    { \
      Handle bHdl = IN; \
      Handle aHdl = bHdl.get(); \
      Assert::That(aHdl.get<__GET_TYPE>(), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(aHdl.get()))->get(), Equals(IN)); \
    } \
    \
    Handle theHdl; \
    INITIALIZATION; \
  }

#define __DescribeHandleAny(__SET_TYPE, __GET_TYPE, __ANY_DESCRIPTION, INITIALIZER) __DescribeHandleTpl(__SET_TYPE, __GET_TYPE, __ANY_DESCRIPTION, in, __SET_TYPE in = INITIALIZER)
#define __DescribeHandleLiteral(__SET_TYPE, __GET_TYPE, __ANY_DESCRIPTION, IN)  __DescribeHandleTpl(__SET_TYPE, __GET_TYPE, __ANY_DESCRIPTION, IN, )

#define DescribeHandleAnyDescription(__ANY_TYPE, __ANY_DESCRIPTION, INITIALIZER) __DescribeHandleAny(__ANY_TYPE, __ANY_TYPE, __ANY_DESCRIPTION, INITIALIZER)
#define DescribeHandleLiteralDescription(__ANY_TYPE, __ANY_DESCRIPTION, IN) __DescribeHandleLiteral(__ANY_TYPE, __ANY_TYPE, __ANY_DESCRIPTION, IN)
#define DescribeHandleString(__ANY_DESCRIPTION, INITIALIZER) DescribeHandleAnyDescription(std::string, __ANY_DESCRIPTION, INITIALIZER)
#define DescribeHandleStringLiteral(__ANY_DESCRIPTION, IN) DescribeHandleLiteralDescription(std::string, __ANY_DESCRIPTION, IN)
#define DescribeHandleAny(__ANY_TYPE, INITIALIZER) DescribeHandleAnyDescription(__ANY_TYPE, __ANY_TYPE, INITIALIZER)
#define DescribeHandleNumeric(__NUMERIC_TYPE) DescribeHandleAnyDescription(__NUMERIC_TYPE, NUMERIC_ ## __NUMERIC_TYPE, rand())
#define DescribeHandleUnsignedNumeric(__NUMERIC_TYPE) DescribeHandleAnyDescription(unsigned __NUMERIC_TYPE, unsigned ## __NUMERIC_TYPE, rand())

Describe(some_handle)
{
  void SetUp()
  {
  }

  It(starts_empty)
  {
    Assert::That(someHandle.toJson(), Equals(""));
  }

  Describe(setting_fields)
  {
    void SetUp()
    {
    }

    It(increases_decreases_member_refcounts)
    {
      Parent().someHandle.set(0);
      Assert::That(Parent().someHandle.get()->getRefCnt(), Equals(1));

      Handle anotherHandle;

      const ValueBase* m = Parent().someHandle.get();
      Assert::That(m->getRefCnt(), Equals(1));

      anotherHandle.set((ValueBase*)m);
      Assert::That(m->getRefCnt(), Equals(2));

      anotherHandle.clear();
      Assert::That(m->getRefCnt(), Equals(1));
    }

    It(copy_constructor_increases_decreases_member_refcounts)
    {
      Parent().someHandle.set(0);
      Assert::That(Parent().someHandle.get()->getRefCnt(), Equals(1));

      const ValueBase* m = Parent().someHandle.get();
      Assert::That(m->getRefCnt(), Equals(1));

      Handle anotherHandle(Parent().someHandle);
      Assert::That(m->getRefCnt(), Equals(2));

      anotherHandle.clear();
      Assert::That(m->getRefCnt(), Equals(1));
    }

    It(decreases_member_refcount_when_unset_via_clear)
    {
      Handle anotherHandle1;
      Handle anotherHandle2;

      anotherHandle1.set("five");

      Assert::That(anotherHandle1.toJson(), Equals("'five'"));
      Assert::That(anotherHandle1.get<std::string>(), Equals("five"));

      Assert::That(((Value<std::string>*)anotherHandle1.get())->getRefCnt(), Equals(1));

      const ValueBase* m = anotherHandle1.get();
      Assert::That(m->getRefCnt(), Equals(1));

      anotherHandle2.set((ValueBase*)m);
      Assert::That(m->getRefCnt(), Equals(2));

      Assert::That(anotherHandle2.toJson(), Equals("'five'"));

      anotherHandle1.clear();
      Assert::That(m->getRefCnt(), Equals(1));

      Assert::That(anotherHandle2.toJson(), Equals("'five'"));
      Assert::That(anotherHandle1.toJson(), Equals(""));
    }

#if 1
    Describe(setting_a_handle_to_an_array)
    {
      void SetUp()
      {
        anArray.push("a");
        anArray.push("b");
        anArray.push("c");
      }

      It(filled_the_array)
      {
        Assert::That(anArray.toJson(), Equals("[ 'a', 'b', 'c' ]"));
      }

      //DescribeHandleAnyDescription(Array, valobj_array, Parent().anArray);
      //DescribeHandleLiteralDescription(Array, literalvalobj_array, Parent().anArray);

      Array anArray;
    };
#endif
#if 1
    Describe(setting_a_handle_to_an_object)
    {
      void SetUp()
      {
        anObject.set("a", 1);
        anObject.set("b", 2);
        anObject.set("c", 3);
      }

      It(filled_the_object)
      {
        Assert::That(anObject.toJson(), Equals("{ 'a': 1, 'b': 2, 'c': 3 }"));
      }

      //DescribeHandleAnyDescription(Object, valobj_array, Parent().anObject);
      //DescribeHandleLiteralDescription(Object, literalvalobj_array, Parent().anObject);

      Object anObject;
    };
#endif

    DescribeHandleNumeric(int);
    DescribeHandleAny(int, (-1 * rand()));
    DescribeHandleUnsignedNumeric(int);

    DescribeHandleNumeric(long);
    DescribeHandleAny(long, -1 * rand());
    DescribeHandleUnsignedNumeric(long);

    DescribeHandleAnyDescription(long long, long_long, rand());
    DescribeHandleAnyDescription(long long, neg_long_long, -1 * rand());
    DescribeHandleAnyDescription(unsigned long long, unsigned_long_long, rand());

    DescribeHandleNumeric(short);
    DescribeHandleAny(short, -1 * rand());
    DescribeHandleUnsignedNumeric(short);

    DescribeHandleNumeric(char);
    DescribeHandleAny(char, -1 * rand());
    DescribeHandleUnsignedNumeric(char);

    DescribeHandleAnyDescription(bool, bool_true,  true);
    DescribeHandleAnyDescription(bool, bool_false, false);
    DescribeHandleLiteralDescription(bool, literal_bool_true, false);
    DescribeHandleLiteralDescription(bool, literal_bool_false, false);

    DescribeHandleNumeric(double);
    DescribeHandleAny(double, -1 * rand());
    DescribeHandleAnyDescription(double, double_pi, 3.14159265358979323846264338327950288419716939937510);
    DescribeHandleLiteralDescription(double, literal_double_pi, 3.14159265358979323846264338327950288419716939937510);

    DescribeHandleAnyDescription(std::string, _std_string, "string test value");
    DescribeHandleLiteralDescription(std::string, _literal_std_string, "string test value");

    DescribeHandleString(string, "string test value");
    DescribeHandleStringLiteral(literal_string, "string test value");

    std::string stdString = "string test value";
    DescribeHandleString(std_string, Parent().stdString);
    DescribeHandleStringLiteral(literal_std_string, Parent().stdString);

    const std::string constStdString = "const string test value";
    DescribeHandleString(const_std_string, Parent().constStdString);
    DescribeHandleStringLiteral(literal_const_std_string, Parent().constStdString);

    std::string& stdStringRef = stdString;
    DescribeHandleString(std_string_ref, Parent().stdStringRef);
    DescribeHandleStringLiteral(literal_std_string_ref, Parent().stdStringRef);

    const std::string& constStdStringRef = constStdString;
    DescribeHandleString(const_std_string_ref, Parent().constStdStringRef);
    DescribeHandleStringLiteral(literal_const_std_string_ref, Parent().constStdStringRef);

    char* charPtr = "string test value";
    DescribeHandleString(char_ptr, (char*)Parent().charPtr);
    DescribeHandleStringLiteral(literal_char_ptr, Parent().charPtr);

    const char* constCharPtr = "string test value";
    DescribeHandleString(const_char_ptr, (const char*)Parent().constCharPtr);
    DescribeHandleStringLiteral(literal_const_char_ptr, Parent().constCharPtr);
  };

  Handle someHandle;
};
