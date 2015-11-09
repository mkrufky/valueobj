#include <object.h>
#include <igloo-listener.h>
#include <igloo/igloo_alt.h>

using namespace igloo;
using namespace valueobj;

#define __DescribeObjectTpl(__SET_TYPE, __GET_TYPE, __ANY_DESCRIPTION, IN, INITIALIZATION) \
  Describe(test_ ## __ANY_DESCRIPTION ## _set_and_get) \
  { \
    void SetUp() \
    { \
      theObj.set<__SET_TYPE>("explicit", IN); \
      theObj.set<__SET_TYPE>(pos, IN); \
      theObj.set("implicit", IN); \
      theObj.set(pos+1, IN); \
    } \
    \
    It(set_explicit_value_correctly) \
    { \
      Assert::That(theObj.get<__GET_TYPE>("explicit"), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(theObj.get("explicit")))->get(), Equals(IN)); \
    } \
    \
    It(set_implicit_value_correctly) \
    { \
      Assert::That(theObj.get<__GET_TYPE>("implicit"), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(theObj.get("implicit")))->get(), Equals(IN)); \
    } \
    \
    It(set_explicit_positioned_value_correctly) \
    { \
      Assert::That(theObj.get<__GET_TYPE>(pos), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(theObj.get(pos)))->get(), Equals(IN)); \
    } \
    \
    It(set_implicit_positioned_value_correctly) \
    { \
      Assert::That(theObj.get<__GET_TYPE>(pos+1), Equals(IN)); \
      Assert::That(((Value<__GET_TYPE>*)(theObj.get(pos+1)))->get(), Equals(IN)); \
    } \
    \
    Object theObj; \
    int pos = rand(); \
    INITIALIZATION; \
  }

#define __DescribeObjectAny(__SET_TYPE, __GET_TYPE, __ANY_DESCRIPTION, INITIALIZER) __DescribeObjectTpl(__SET_TYPE, __GET_TYPE, __ANY_DESCRIPTION, in, __SET_TYPE in = INITIALIZER)
#define __DescribeObjectLiteral(__SET_TYPE, __GET_TYPE, __ANY_DESCRIPTION, IN)  __DescribeObjectTpl(__SET_TYPE, __GET_TYPE, __ANY_DESCRIPTION, IN, )

#define DescribeObjectAnyDescription(__ANY_TYPE, __ANY_DESCRIPTION, INITIALIZER) __DescribeObjectAny(__ANY_TYPE, __ANY_TYPE, __ANY_DESCRIPTION, INITIALIZER)
#define DescribeObjectLiteralDescription(__ANY_TYPE, __ANY_DESCRIPTION, IN) __DescribeObjectLiteral(__ANY_TYPE, __ANY_TYPE, __ANY_DESCRIPTION, IN)
#define DescribeObjectString(__ANY_DESCRIPTION, INITIALIZER) DescribeObjectAnyDescription(std::string, __ANY_DESCRIPTION, INITIALIZER)
#define DescribeObjectStringLiteral(__ANY_DESCRIPTION, IN) DescribeObjectLiteralDescription(std::string, __ANY_DESCRIPTION, IN)
#define DescribeObjectAny(__ANY_TYPE, INITIALIZER) DescribeObjectAnyDescription(__ANY_TYPE, __ANY_TYPE, INITIALIZER)
#define DescribeObjectNumeric(__NUMERIC_TYPE) DescribeObjectAnyDescription(__NUMERIC_TYPE, NUMERIC_ ## __NUMERIC_TYPE, rand())
#define DescribeObjectUnsignedNumeric(__NUMERIC_TYPE) DescribeObjectAnyDescription(unsigned __NUMERIC_TYPE, unsigned ## __NUMERIC_TYPE, rand())

Describe(some_object)
{
  void SetUp()
  {
  }

  It(starts_empty)
  {
    Assert::That(someObject.toJson(), Equals("{}"));
  }

  Describe(setting_fields)
  {
    void SetUp()
    {
    }

    It(increases_decreases_member_refcounts)
    {
      Parent().someObject.set("member", 0);
      Assert::That(Parent().someObject.get("member")->getRefCnt(), Equals(1));

      Object anotherObject;

      const ValueBase* m = Parent().someObject.get("member");
      Assert::That(m->getRefCnt(), Equals(1));

      anotherObject.set((ValueBase*)m);
      Assert::That(m->getRefCnt(), Equals(2));

      anotherObject.clear();
      Assert::That(m->getRefCnt(), Equals(1));
    }

    It(copy_constructor_increases_decreases_member_refcounts)
    {
      Parent().someObject.set("member", 0);
      Assert::That(Parent().someObject.get("member")->getRefCnt(), Equals(1));

      const ValueBase* m = Parent().someObject.get("member");
      Assert::That(m->getRefCnt(), Equals(1));

      Object anotherObject(Parent().someObject);
      Assert::That(m->getRefCnt(), Equals(2));

      anotherObject.clear();
      Assert::That(m->getRefCnt(), Equals(1));
    }

    It(inserts_an_object_into_an_object_into_an_object)
    {
      Object anotherObject1;

      anotherObject1.set("a", 1);
      anotherObject1.set("b", 2);
      anotherObject1.set("c", 3);

      Object anotherObject2;

      anotherObject2.set("d", 4);
      anotherObject2.set("e", 5);
      anotherObject2.set("f", 6);

      Object anotherObject3;

      anotherObject3.set("g", 7);
      anotherObject3.set("h", 8);
      anotherObject3.set("i", 9);

      anotherObject2.set("j", anotherObject3);
      anotherObject1.set("k", anotherObject2);

      Assert::That(anotherObject1.toJson(), Equals("{ 'a': 1, 'b': 2, 'c': 3, 'k': { 'd': 4, 'e': 5, 'f': 6, 'j': { 'g': 7, 'h': 8, 'i': 9 } } }"));
    }

    It(inserts_two_arrays_into_an_object)
    {
      Object anotherObject;

      anotherObject.set("a", 1);
      anotherObject.set("b", 2);
      anotherObject.set("c", 3);

      Array someArray1;

      someArray1.push("d");
      someArray1.push("e");
      someArray1.push("f");

      Array someArray2;

      someArray2.push("g");
      someArray2.push("h");
      someArray2.push("i");

      anotherObject.set("j", someArray1);
      anotherObject.set("k", someArray2);

      Assert::That(anotherObject.toJson(), Equals("{ 'a': 1, 'b': 2, 'c': 3, 'j': [ 'd', 'e', 'f' ], 'k': [ 'g', 'h', 'i' ] }"));
    }

    It(decreases_member_refcount_when_unset_via_int)
    {
      Object anotherObject1;
      Object anotherObject2;

      anotherObject1.set(5, "five");

      Assert::That(anotherObject1.toJson(), Equals("{ '5': 'five' }"));
      Assert::That(anotherObject1.get<std::string>(5), Equals("five"));

      Assert::That(((Value<std::string>*)anotherObject1.get(5))->getRefCnt(), Equals(1));

      const ValueBase* m = anotherObject1.get(5);
      Assert::That(m->getRefCnt(), Equals(1));

      anotherObject2.set((ValueBase*)m);
      Assert::That(m->getRefCnt(), Equals(2));

      Assert::That(anotherObject2.toJson(), Equals("{ '5': 'five' }"));

      anotherObject1.unSet(5);
      Assert::That(m->getRefCnt(), Equals(1));

      Assert::That(anotherObject2.toJson(), Equals("{ '5': 'five' }"));
      Assert::That(anotherObject1.toJson(), Equals("{}"));
    }

    It(decreases_member_refcount_when_unset_via_string)
    {
      Object anotherObject1;
      Object anotherObject2;

      anotherObject1.set("number", "five");

      Assert::That(anotherObject1.toJson(), Equals("{ 'number': 'five' }"));
      Assert::That(anotherObject1.get<std::string>("number"), Equals("five"));

      Assert::That(((Value<std::string>*)anotherObject1.get("number"))->getRefCnt(), Equals(1));

      const ValueBase* m = anotherObject1.get("number");
      Assert::That(m->getRefCnt(), Equals(1));

      anotherObject2.set((ValueBase*)m);
      Assert::That(m->getRefCnt(), Equals(2));

      Assert::That(anotherObject2.toJson(), Equals("{ 'number': 'five' }"));

      anotherObject1.unSet("number");
      Assert::That(m->getRefCnt(), Equals(1));

      Assert::That(anotherObject2.toJson(), Equals("{ 'number': 'five' }"));
      Assert::That(anotherObject1.toJson(), Equals("{}"));
    }

    DescribeObjectNumeric(int);
    DescribeObjectAny(int, (-1 * rand()));
    DescribeObjectUnsignedNumeric(int);

    DescribeObjectNumeric(long);
    DescribeObjectAny(long, -1 * rand());
    DescribeObjectUnsignedNumeric(long);

    DescribeObjectAnyDescription(long long, long_long, rand());
    DescribeObjectAnyDescription(long long, neg_long_long, -1 * rand());
    DescribeObjectAnyDescription(unsigned long long, unsigned_long_long, rand());

    DescribeObjectNumeric(short);
    DescribeObjectAny(short, -1 * rand());
    DescribeObjectUnsignedNumeric(short);

    DescribeObjectNumeric(char);
    DescribeObjectAny(char, -1 * rand());
    DescribeObjectUnsignedNumeric(char);

    DescribeObjectAnyDescription(bool, bool_true,  true);
    DescribeObjectAnyDescription(bool, bool_false, false);
    DescribeObjectLiteralDescription(bool, literal_bool_true, false);
    DescribeObjectLiteralDescription(bool, literal_bool_false, false);

    DescribeObjectNumeric(double);
    DescribeObjectAny(double, -1 * rand());
    DescribeObjectAnyDescription(double, double_pi, 3.14159265358979323846264338327950288419716939937510);
    DescribeObjectLiteralDescription(double, literal_double_pi, 3.14159265358979323846264338327950288419716939937510);

    DescribeObjectAnyDescription(std::string, _std_string, "string test value");
    DescribeObjectLiteralDescription(std::string, _literal_std_string, "string test value");

    DescribeObjectString(string, "string test value");
    DescribeObjectStringLiteral(literal_string, "string test value");

    std::string stdString = "string test value";
    DescribeObjectString(std_string, Parent().stdString);
    DescribeObjectStringLiteral(literal_std_string, Parent().stdString);

    const std::string constStdString = "const string test value";
    DescribeObjectString(const_std_string, Parent().constStdString);
    DescribeObjectStringLiteral(literal_const_std_string, Parent().constStdString);

    std::string& stdStringRef = stdString;
    DescribeObjectString(std_string_ref, Parent().stdStringRef);
    DescribeObjectStringLiteral(literal_std_string_ref, Parent().stdStringRef);

    const std::string& constStdStringRef = constStdString;
    DescribeObjectString(const_std_string_ref, Parent().constStdStringRef);
    DescribeObjectStringLiteral(literal_const_std_string_ref, Parent().constStdStringRef);

    char* charPtr = "string test value";
    DescribeObjectString(char_ptr, (char*)Parent().charPtr);
    DescribeObjectStringLiteral(literal_char_ptr, Parent().charPtr);

    const char* constCharPtr = "string test value";
    DescribeObjectString(const_char_ptr, (const char*)Parent().constCharPtr);
    DescribeObjectStringLiteral(literal_const_char_ptr, Parent().constCharPtr);
  };

  Object someObject;
};
