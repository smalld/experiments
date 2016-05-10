// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cmath>


//float sin(float& in) {
//
//    return 0;
//}


namespace MyNamespace {
    //using std::sin;


    struct xyz {


    private:

        friend xyz sin(xyz& in) {

            return xyz();
        }

    };

    void F1() {
        float a = 0;

        //sin(a);
    }

}

namespace MyNamespace2 {
    void sin(::MyNamespace::xyz& _) {

    }
    void sin(float& _) {

    }
    void sin(int& _) {

    }
    namespace MyNamespace2 {
        /*void sin(::MyNamespace::xyz& _) {

        }
    */  void sin(float& _) {

        }
        void sin(int& _) {

        }

        using namespace std;
        using namespace MyNamespace;

        void F() {
            float a = 0;
            ::MyNamespace::xyz x;

            sin(x);

            sin(a);
        }
    }

    
}


int main() {
    float a = 0;

    sin(a);

    return 0;
}

