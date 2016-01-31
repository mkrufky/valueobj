#include <array.h>
#include <igloo-listener.h>
#include <igloo/igloo_alt.h>

using namespace igloo;
using namespace valueobj;

#define __DescribeArrayTpl(__SET_TYPE, __GET_TYPE, __ANY_DESCRIPTION, IN, INITIALIZATION) \
  Describe(test_ ## __ANY_DESCRIPTION ## _set_and_get) \
  { \
    void SetUp() \
    { \
      theArray.push<__SET_TYPE>(IN); \
      explicitPos = theArray.size() - 1; \
      theArray.set<__SET_TYPE>(pos, IN); \
      theArray.push(IN); \
      implicitPos = theArray.size() - 1; \
      theArray.set(pos+1, IN); \
      theArray.set<__SET_TYPE>("explicit", IN); \
      theArray.set("implicit", IN); \
    } \
    \
    It(set_explicit_value_correctly) \
    { \
      Assert::That(theArray.get<__GET_TYPE>(explicitPos), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(theArray.get(explicitPos)))->get(), Equals(IN)); \
    } \
    \
    It(set_explicit_positioned_value_correctly) \
    { \
      Assert::That(theArray.get<__GET_TYPE>(explicitPos+1), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(theArray.get(explicitPos+1)))->get(), Equals(IN)); \
    } \
    \
    It(set_explicit_string_positioned_value_correctly) \
    { \
      Assert::That(((Value<__GET_TYPE>*)(theArray.getByName("explicit")))->get(), Equals(IN)); \
    } \
    \
    It(set_implicit_value_correctly) \
    { \
      Assert::That(theArray.get<__GET_TYPE>(implicitPos), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(theArray.get(implicitPos)))->get(), Equals(IN)); \
    } \
    \
    It(set_implicit_positioned_value_correctly) \
    { \
      Assert::That(theArray.get<__GET_TYPE>(implicitPos+1), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(theArray.get(implicitPos+1)))->get(), Equals(IN)); \
    } \
    \
    It(set_implicit_string_positioned_value_correctly) \
    { \
      Assert::That(((Value<__GET_TYPE>*)(theArray.getByName("implicit")))->get(), Equals(IN)); \
    } \
    \
    Array theArray; \
    size_t explicitPos; \
    size_t implicitPos; \
    int pos = rand(); \
    INITIALIZATION; \
  }

#define __DescribeArrayAny(__SET_TYPE, __GET_TYPE, __ANY_DESCRIPTION, INITIALIZER) __DescribeArrayTpl(__SET_TYPE, __GET_TYPE, __ANY_DESCRIPTION, in, __SET_TYPE in = INITIALIZER)
#define __DescribeArrayLiteral(__SET_TYPE, __GET_TYPE, __ANY_DESCRIPTION, IN)  __DescribeArrayTpl(__SET_TYPE, __GET_TYPE, __ANY_DESCRIPTION, IN, )

#define DescribeArrayAnyDescription(__ANY_TYPE, __ANY_DESCRIPTION, INITIALIZER) __DescribeArrayAny(__ANY_TYPE, __ANY_TYPE, __ANY_DESCRIPTION, INITIALIZER)
#define DescribeArrayLiteralDescription(__ANY_TYPE, __ANY_DESCRIPTION, IN) __DescribeArrayLiteral(__ANY_TYPE, __ANY_TYPE, __ANY_DESCRIPTION, IN)
#define DescribeArrayString(__ANY_DESCRIPTION, INITIALIZER) DescribeArrayAnyDescription(std::string, __ANY_DESCRIPTION, INITIALIZER)
#define DescribeArrayStringLiteral(__ANY_DESCRIPTION, IN) DescribeArrayLiteralDescription(std::string, __ANY_DESCRIPTION, IN)
#define DescribeArrayAny(__ANY_TYPE, INITIALIZER) DescribeArrayAnyDescription(__ANY_TYPE, __ANY_TYPE, INITIALIZER)
#define DescribeArrayNumeric(__NUMERIC_TYPE) DescribeArrayAnyDescription(__NUMERIC_TYPE, NUMERIC_ ## __NUMERIC_TYPE, rand())
#define DescribeArrayUnsignedNumeric(__NUMERIC_TYPE) DescribeArrayAnyDescription(unsigned __NUMERIC_TYPE, unsigned ## __NUMERIC_TYPE, rand())

Describe(some_array)
{
  void SetUp()
  {
  }

  It(starts_empty)
  {
    Assert::That(someArray.toJson(), Equals("[]"));
  }

  Describe(setting_fields)
  {
    void SetUp()
    {
    }

    It(increases_decreases_member_refcounts)
    {
      Parent().someArray.push(0);
      Assert::That(((ValueBase*)(Parent().someArray.get(Parent().someArray.size()-1)))->getRefCnt(), Equals(1));

      Array anotherArray;

      const ValueBase* m = Parent().someArray.get(Parent().someArray.size()-1);
      Assert::That(m->getRefCnt(), Equals(1));

      anotherArray.push((ValueBase*)m);
      Assert::That(m->getRefCnt(), Equals(2));

      anotherArray.clear();
      Assert::That(m->getRefCnt(), Equals(1));
    }

    It(copy_constructor_increases_decreases_member_refcounts)
    {
      Parent().someArray.push(0);
      Assert::That(((ValueBase*)(Parent().someArray.get(Parent().someArray.size()-1)))->getRefCnt(), Equals(1));

      const ValueBase* m = Parent().someArray.get(Parent().someArray.size()-1);
      Assert::That(m->getRefCnt(), Equals(1));

      Array anotherArray(Parent().someArray);
      Assert::That(m->getRefCnt(), Equals(2));

      anotherArray.clear();
      Assert::That(m->getRefCnt(), Equals(1));
    }

    It(inserts_an_array_into_an_array_into_an_array)
    {
      Array anotherArray1;

      anotherArray1.push("a");
      anotherArray1.push("b");
      anotherArray1.push("c");

      Array anotherArray2;

      anotherArray2.push("d");
      anotherArray2.push("e");
      anotherArray2.push("f");

      Array anotherArray3;

      anotherArray3.push("g");
      anotherArray3.push("h");
      anotherArray3.push("i");

      anotherArray2.push(anotherArray3);
      anotherArray1.push(anotherArray2);

      Assert::That(anotherArray1.toJson(), Equals("[ \"a\", \"b\", \"c\", [ \"d\", \"e\", \"f\", [ \"g\", \"h\", \"i\" ] ] ]"));
    }

    It(inserts_two_objects_into_an_array)
    {
      Array anotherArray;

      anotherArray.push("a");
      anotherArray.push("b");
      anotherArray.push("c");

      Object someObject1;

      someObject1.set("d", 4);
      someObject1.set("e", 5);
      someObject1.set("f", 6);

      Object someObject2;

      someObject2.set("g", 7);
      someObject2.set("h", 8);
      someObject2.set("i", 9);

      anotherArray.push(someObject1);
      anotherArray.push(someObject2);

      Assert::That(anotherArray.toJson(), Equals("[ \"a\", \"b\", \"c\", { \"d\": 4, \"e\": 5, \"f\": 6 }, { \"g\": 7, \"h\": 8, \"i\": 9 } ]"));
    }

    DescribeArrayNumeric(int);
    DescribeArrayAny(int, (-1 * rand()));
    DescribeArrayUnsignedNumeric(int);

    DescribeArrayNumeric(long);
    DescribeArrayAny(long, -1 * rand());
    DescribeArrayUnsignedNumeric(long);

    DescribeArrayAnyDescription(long long, long_long, rand());
    DescribeArrayAnyDescription(long long, neg_long_long, -1 * rand());
    DescribeArrayAnyDescription(unsigned long long, unsigned_long_long, rand());

    DescribeArrayNumeric(short);
    DescribeArrayAny(short, -1 * rand());
    DescribeArrayUnsignedNumeric(short);

    DescribeArrayNumeric(char);
    DescribeArrayAny(char, -1 * rand());
    DescribeArrayUnsignedNumeric(char);

    DescribeArrayAnyDescription(bool, bool_true,  true);
    DescribeArrayAnyDescription(bool, bool_false, false);
    DescribeArrayLiteralDescription(bool, literal_bool_true, false);
    DescribeArrayLiteralDescription(bool, literal_bool_false, false);

    DescribeArrayNumeric(double);
    DescribeArrayAny(double, -1 * rand());
    DescribeArrayAnyDescription(double, double_pi, 3.14159265358979323846264338327950288419716939937510);
    DescribeArrayLiteralDescription(double, literal_double_pi, 3.14159265358979323846264338327950288419716939937510);

    DescribeArrayAnyDescription(std::string, _std_string, "string test value");
    DescribeArrayLiteralDescription(std::string, _literal_std_string, "string test value");

    DescribeArrayString(string, "string test value");
    DescribeArrayStringLiteral(literal_string, "string test value");

    std::string stdString = "string test value";
    DescribeArrayString(std_string, Parent().stdString);
    DescribeArrayStringLiteral(literal_std_string, Parent().stdString);

    const std::string constStdString = "const string test value";
    DescribeArrayString(const_std_string, Parent().constStdString);
    DescribeArrayStringLiteral(literal_const_std_string, Parent().constStdString);

    std::string& stdStringRef = stdString;
    DescribeArrayString(std_string_ref, Parent().stdStringRef);
    DescribeArrayStringLiteral(literal_std_string_ref, Parent().stdStringRef);

    const std::string& constStdStringRef = constStdString;
    DescribeArrayString(const_std_string_ref, Parent().constStdStringRef);
    DescribeArrayStringLiteral(literal_const_std_string_ref, Parent().constStdStringRef);

    char* charPtr = "string test value";
    DescribeArrayString(char_ptr, (char*)Parent().charPtr);
    DescribeArrayStringLiteral(literal_char_ptr, Parent().charPtr);

    const char* constCharPtr = "string test value";
    DescribeArrayString(const_char_ptr, (const char*)Parent().constCharPtr);
    DescribeArrayStringLiteral(literal_const_char_ptr, Parent().constCharPtr);
  };

  Array someArray;
};
