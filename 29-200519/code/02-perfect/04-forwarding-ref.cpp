#include <utility>

template<typename> struct TD;

template<typename T>
void foo(T &&x) {  // Special `T` deduction rules for 'T&&', not 'vector<T>&&', not 'const T&&': "forwarding reference".
    TD<T>();
    TD<decltype(x)>();  // Same as T&&.
}

int main() {
    int v = 20;
    const int cv = 30;
    //foo(10);             // T=       int ,       int &&x
    //foo(v);              // T=      int& ,       int  &x
    //foo(cv);             // T=const int& , const int  &x
    //foo(std::move(v));   // T=      int  ,       int &&x
    //foo(std::move(cv));  // T=const int  , const int &&x

    /*
     * const T& AND is_reference_v<T> =====> T
     * const T&& AND is_reference_v<T> =====> T
     * (T&) & =====> T&
     * (T&) && =====> T&
     * (T&&) & =====> T&
     * (T&&) && =====> T&&
     */

    /*{
        using T = int;
        TD<const T>();    // const int
        TD<T&>();         //       int&
        TD<T&&>();        //       int&&
        TD<const T&>();   // const int&
        TD<const T&&>();  // const int&&
    }*/

    /*{
        using T = const int;
        TD<const T>();    // const int
        TD<T&>();         // const int&
        TD<T&&>();        // const int&&
        TD<const T&>();   // const int&
        TD<const T&&>();  // const int&&
    }*/

    /*{
        using T = int&;
        TD<const T>();    //       int&; reference cannot be changed.
        TD<T&>();         //       int&
        TD<T&&>();        //       int&
        TD<const T&>();   //       int&
        TD<const T&&>();  //       int&
    }*/

    /*{
        using T = int&&;
        TD<const T>();    //       int&&
        TD<T&>();         //       int&
        TD<T&&>();        //       int&&
        TD<const T&>();   //       int&
        TD<const T&&>();  //       int&&
    }*/

    /*{
        using T = const int&;
        TD<const T>();    // const int&
        TD<T&>();         // const int&
        TD<T&&>();        // const int&
        TD<const T&>();   // const int&
        TD<const T&&>();  // const int&
    }*/

    /*{
        using T = const int&&;
        TD<const T>();    // const int&&
        TD<T&>();         // const int&
        TD<T&&>();        // const int&&
        TD<const T&>();   // const int&
        TD<const T&&>();  // const int&&
    }*/
}