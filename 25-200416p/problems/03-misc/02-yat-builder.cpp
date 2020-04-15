#include <cassert>
#include <memory>
#include <iostream>
#include <utility>

using std::make_unique;
using std::move;
using std::unique_ptr;

struct Expression {
    virtual ~Expression() {};
    virtual int eval() const;
};

struct Const : Expression {
    Const(int value) : m_value(value) {}

    int value() const { return m_value; }

    int eval() const override { return m_value; }

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

private:
    std::unique_ptr<Expression> m_left, m_right;
    Operation m_operation;
};

struct ExpressionBuilder {
    // START SOLUTION
    TODO
    // END SOLUTION

    unique_ptr<Expression> build() && {
        return move(expr);
    }

private:
    unique_ptr<Expression> expr;
};

int main() {
    {
        auto expr = make_unique<BinaryExpression>(
            make_unique<BinaryExpression>(make_unique<Const>(2), BinaryExpression::ADD, make_unique<Const>(3)),
            BinaryExpression::MUL,
            make_unique<Const>(10)
        );
        assert(expr->eval() == (2 + 3) * 10);
    }
    {
       auto expr = ExpressionBuilder{
           {2, BinaryExpression::ADD, 3}, BinaryExpression::MUL, 10
       }.build();
       assert(expr->eval() == (2 + 3) * 10);
    }
    std::cout << "PASSED\n";
}
