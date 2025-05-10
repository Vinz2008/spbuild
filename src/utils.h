#include <memory>
#include <optional>
#include "parser.h"

template <class D, class B>
std::unique_ptr<D> downcast_expr(std::unique_ptr<B> b){
    std::unique_ptr<D> SubClass;
    D* SubClassPtr = dynamic_cast<D*>(b.get());
    if (!SubClassPtr){
        return nullptr;
    }
    b.release();
    SubClass.reset(SubClassPtr);
    return SubClass;
}

std::pair<int, std::optional<std::string>> run_cmd(const std::string& cmd, bool keep_output = false);
