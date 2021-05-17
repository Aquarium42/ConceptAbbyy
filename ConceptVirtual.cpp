#include <stdio.h>
#include <iostream>
#include <string>
#include <cassert>
#include <unordered_map>
#include <functional>
using namespace std;

#define VIRTUAL_CLASS(class_name)                                                  \
class class_name {                                                                 \
public:                                                                            \
    class_name();                                                                  \


#define END(class_name)                                                            \
public:                                                                            \
	unordered_map<string, bool> class_name##_##methods;								\
	unordered_map<string, function<void(void*)>> v_table;						\
    bool isMethodExist(string method_name) {										  \
        auto method = class_name##_##methods.find(method_name);                    \
        return method != class_name##_##methods.end() && method->second;           \
    }                                                                              \
};                                                                                 \

#define DECLARE_METHOD(class_name, method_name, func)                          \
v_table[#method_name] = func;                                            \
class_name##_##methods[#method_name] = true;

#define VIRTUAL_CLASS_DERIVED(class_name, base_class_name)                         \
class class_name : public base_class_name {                                        \
public:                                                                            \
    class_name();											        			   \


#define END_DERIVE(class_name, base_class_name)                                    \
    unordered_map<string, bool> class_name##_##methods;                  \
    bool isMethodExist(string method_name) {                                      \
        auto method = class_name##_##methods.find(method_name);                    \
		if (method != class_name##_##methods.end()) {                              \
			return method->second;                                                 \
		} else {                                                                   \
			bool has_method = base_class_name::isMethodExist(method_name);             \
            class_name##_##methods[method_name] = has_method;                      \
            return has_method;                                                     \
		}                                                                          \
    }                                                                              \
};                                                                                 \


#define CONSTRUCTOR_START(class_name)                                              \
class_name::class_name() {

#define CONSTRUCTOR_END                                                            \
};

#define VIRTUAL_CALL(class_, method_)									   \
assert(class_->isMethodExist(#method_));                                       \
class_->v_table[#method_](class_);

VIRTUAL_CLASS(Base)
int a = 0;
END(Base)

CONSTRUCTOR_START(Base)
DECLARE_METHOD(Base, Both, [](void* self) {		
	int a = reinterpret_cast<Base*>(self)->a;
	cout << "Base::Both a = " << a << "\n";
})
DECLARE_METHOD(Base, OnlyBase, [](void* self) {
	int a = reinterpret_cast<Base*>(self)->a;
	cout << "Base::OnlyBase a = " << a << "\n";
})
CONSTRUCTOR_END

VIRTUAL_CLASS_DERIVED(Derived, Base)
int a = 1;
int b = -1;
END_DERIVE(Derived, Base)

CONSTRUCTOR_START(Derived)
DECLARE_METHOD(Derived, Both, [](void* self) {
	int b = reinterpret_cast<Derived*>(self)->b;
	cout << "Derived::Both, b = " << b << endl;
})
DECLARE_METHOD(Derived, OnlyDerived, [](void* self) {
	int b = reinterpret_cast<Derived*>(self)->b;
	int a = reinterpret_cast<Derived*>(self)->a;
	cout << "Derived::OnlyDerived, a = " << a <<", b= "<< b << endl;
})
CONSTRUCTOR_END

int main() {

	Base base;
	Derived derived;
	Base* reallyDerived = reinterpret_cast<Base*>(&derived);
	

	VIRTUAL_CALL((&base), Both);
	VIRTUAL_CALL((&derived), Both);
	VIRTUAL_CALL((&derived), OnlyDerived);
	VIRTUAL_CALL(reallyDerived, Both);
	VIRTUAL_CALL(reallyDerived, OnlyBase);

	//VIRTUAL_CALL(reallyDerived, OnlyDerived); // вызов assert

	return 0;
}