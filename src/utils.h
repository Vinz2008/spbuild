#include <memory>
#include "parser.h"

template <class T>
std::unique_ptr<T> downcast_expr(std::unique_ptr<Expr> E){
    std::unique_ptr<T> SubClass;
    T* SubClassPtr = dynamic_cast<T*>(E.get());
    if (!SubClassPtr){
        return nullptr;
    }
    E.release();
    SubClass.reset(SubClassPtr);
    return SubClass;
}