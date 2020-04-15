#include <cassert>
#include <memory>
#include <iostream>
#include <sstream>
#include <utility>

using std::make_unique;
using std::move;
using std::unique_ptr;

struct ExpressionVisitor;  // https://ru.wikipedia.org/wiki/%D0%9F%D0%BE%D1%81%D0%B5%D1%82%D0%B8%D1%82%D0%B5%D0%BB%D1%8C_(%D1%88%D0%B0%D0%B1%D0%BB%D0%BE%D0%BD_%D0%BF%D1%80%D0%BE%D0%B5%D0%BA%D1%82%D0%B8%D1%80%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D1%8F)

struct Expression {
    virtual ~Expression() {};
    virtual int eval() const;
    virtual void accept(ExpressionVisitor&) const;
};

struct Const;
struct BinaryExpression;

struct ExpressionVisitor {
    virtual ~ExpressionVisitor() {}
    void operator()(const Expression &e) {  // Non-virtual visit
        e.accept(*this);
    }
    virtual void operator()(const Const&);  // visit
    virtual void operator()(const BinaryExpression&);  // visit
};

struct Const : Expression {
    Const(int value) : m_value(value) {}

    int value() const { return m_value; }

    int eval() const override { return m_value; }
    void accept(ExpressionVisitor &v) const override { v(*this); }

private:
    int m_value;
};

struct BinaryExpression : Expression {
    enum Operation { ADD, MUL };

    BinaryExpression(unique_ptr<Expression> left, Operation operation, unique_ptr<Expression> right)
        : m_left(move(left))
        , m_right(move(right))
        , m_operation(operation)
    {}

    const Expression& left() const { return *m_left; }
    const Expression& right() const { return *m_right; }
    Operation operation() const { return m_operation; }

    int eval() const override {
        int l = m_left->eval(), r = m_right->eval();
        switch (m_operation) {
        case ADD: return l + r;
        case MUL: return l * r;
        default: assert(false);
        }
    }
    void accept(ExpressionVisitor &v) const override { v(*this); }

private:
    std::unique_ptr<Expression> m_left, m_right;
    Operation m_operation;
};

struct PrintVisitor : ExpressionVisitor {
    using ExpressionVisitor::operator();  // Make non-virtual operator visible.

    void operator()(const Const &c) {
        result << c.value();
    }

    void operator()(const BinaryExpression &b) {
        result << '(';
        (*this)(b.left());
        result << ' ';
        switch (b.operation()) {
        case BinaryExpression::ADD: result << '+'; break;
        case BinaryExpression::MUL: result << '*'; break;
        default: assert(false);
        }
        result << ' ';
        (*this)(b.right());
        result << ')';
    }

    auto str() const {
        return result.str();
    }

private:
    std::stringstream result;
};

int main() {
    auto expr = make_unique<BinaryExpression>(
        make_unique<BinaryExpression>(make_unique<Const>(2), BinaryExpression::ADD, make_unique<Const>(3)),
        BinaryExpression::MUL,
        make_unique<Const>(10)
    );
    PrintVisitor printer;
    printer(*expr);
    assert(printer.str() == "((2 + 3) * 10)");
    std::cout << "PASSED\n";
}
