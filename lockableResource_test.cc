#define QT_FORCE_ASSERTS


#include "lockableResource.h"
#include <QString>

#include <iostream>


class MyTestClass {
public:
    MyTestClass() : my_str(new QString) {};
    void testLock() {
        // will assert: the mutex is not locked
        // my_str->toDouble();

        std::cout << "before lock #1\n";
        auto a = CMutexLocker(my_str);

        // will not assert
        my_str->toDouble();

        std::cout << "before lock #2\n";
        auto s = my_str.lockScoped();

        std::cout << "after locks\n";
    };
private:
    COwnedLockableResource<QString> my_str;
};

int main() {
	MyTestClass t;
    t.testLock();
}
