#include "scheme.h"

std::shared_ptr<Object> Eval(std::shared_ptr<Object> ast);

std::shared_ptr<Object> EvalInteger(const std::vector<int64_t>& nums, const std::string& op) {
    if (op == "+") {
        int64_t sum = 0;
        for (auto el : nums) {
            sum += el;
        }
        return std::make_shared<Number>(sum);
    }
    if (op == "*") {
        int64_t ans = 1;
        for (auto el : nums) {
            ans *= el;
        }
        return std::make_shared<Number>(ans);
    }
    if (op == "-") {
        if (nums.empty()) {
            throw RuntimeError("");
        }
        int64_t ans = nums[0];
        for (size_t i = 1; i < nums.size(); ++i) {
            ans -= nums[i];
        }
        return std::make_shared<Number>(ans);
    }
    if (op == "/") {
        if (nums.size() < 2) {
            throw RuntimeError("");
        }
        int64_t ans = nums[0];
        for (size_t i = 1; i < nums.size(); ++i) {
            ans /= nums[i];
        }
        return std::make_shared<Number>(ans);
    }
    throw RuntimeError("");
}

std::shared_ptr<Object> EvalQuotes(std::shared_ptr<Cell> cell, const std::string& op) {
    if (op == "quote") {
        return As<Cell>(cell->second_)->first_;
    }

    if (op == "and" || op == "or") {
        if (!cell->second_) {
            return std::make_shared<Boolean>(op == "and");
        }
        auto curr = cell->second_;
        while (curr) {
            auto value = Eval(As<Cell>(curr)->first_);
            if (!As<Cell>(curr)->second_) {
                return value;
            }
            if (Is<Boolean>(value)) {
                bool flag = As<Boolean>(value)->value_;
                if ((op == "and" && !flag) || (op == "or" && flag)) {
                    return value;
                }
            }
            curr = As<Cell>(curr)->second_;
        }
        return std::make_shared<Boolean>(op == "and");
    }
    return nullptr;
}

std::shared_ptr<Object> EvalSymbol(const std::vector<int64_t>& nums, const std::string& op) {
    if (op == "=") {
        for (size_t i = 1; i < nums.size(); ++i) {
            if (nums[i] != nums[0]) {
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    }
    if (op == "<") {
        for (size_t i = 1; i < nums.size(); ++i) {
            if (nums[i] <= nums[i - 1]) {
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    }
    if (op == ">") {
        for (size_t i = 1; i < nums.size(); ++i) {
            if (nums[i] >= nums[i - 1]) {
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    }
    if (op == "<=") {
        for (size_t i = 1; i < nums.size(); ++i) {
            if (nums[i] < nums[i - 1]) {
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    }
    if (op == ">=") {
        for (size_t i = 1; i < nums.size(); ++i) {
            if (nums[i] > nums[i - 1]) {
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    }
    throw RuntimeError("");
}

std::shared_ptr<Object> EvalNumeric(const std::vector<int64_t>& nums, const std::string& op) {
    if (nums.empty()) {
        throw RuntimeError("");
    }
    if (op == "max") {
        int64_t max_value = nums[0];
        for (auto el : nums) {
            max_value = std::max(max_value, el);
        }
        return std::make_shared<Number>(max_value);
    }
    if (op == "min") {
        int64_t min_value = nums[0];
        for (auto el : nums) {
            min_value = std::min(min_value, el);
        }
        return std::make_shared<Number>(min_value);
    }
    if (op == "abs") {
        if (nums.size() != 1) {
            throw RuntimeError("");
        }
        return std::make_shared<Number>(std::abs(nums[0]));
    }
    throw RuntimeError("");
}

std::shared_ptr<Object> EvalList(const std::vector<std::shared_ptr<Object>>& args,
                                 const std::string& op) {
    if (op == "list") {
        if (args.empty()) {
            return nullptr;
        }
        std::vector<std::shared_ptr<Cell>> res;
        for (const auto& arg : args) {
            res.push_back(std::make_shared<Cell>(arg, nullptr));
        }
        for (size_t i = 0; i < res.size() - 1; ++i) {
            res[i]->second_ = res[i + 1];
        }
        return res[0];
    }

    if (op == "list?") {
        auto curr = args[0];
        while (auto cell = As<Cell>(curr)) {
            curr = cell->second_;
        }
        return std::make_shared<Boolean>(curr == nullptr);
    }

    if (op == "null?") {
        return std::make_shared<Boolean>(args[0] == nullptr);
    }

    if (op == "pair?") {
        return std::make_shared<Boolean>(Is<Cell>(args[0]));
    }

    if (op == "list-ref" || op == "list-tail") {
        auto temp = args[0];
        auto idx = As<Number>(args[1]);
        int64_t pos = 0;
        auto curr = As<Cell>(temp);
        if (op == "list-ref") {
            while (curr && pos != idx->GetValue()) {
                curr = As<Cell>(curr->second_);
                ++pos;
            }
            if (!curr) {
                throw RuntimeError("");
            }
            return curr->first_;
        } else {
            while (pos < idx->GetValue()) {
                if (!curr) {
                    throw RuntimeError("");
                }
                curr = As<Cell>(curr->second_);
                ++pos;
            }
            return curr;
        }
    }

    throw RuntimeError("");
}

std::shared_ptr<Object> EvalBool(const std::vector<std::shared_ptr<Object>>& args,
                                 const std::string& op) {
    if (op == "boolean?") {
        return std::make_shared<Boolean>(Is<Boolean>(args[0]));
    }

    if (op == "not") {
        if (args.size() != 1) {
            throw RuntimeError("");
        }
        if (Is<Boolean>(args[0]) && !As<Boolean>(args[0])->value_) {
            return std::make_shared<Boolean>(true);
        }
        return std::make_shared<Boolean>(false);
    }
    throw RuntimeError("");
}

std::shared_ptr<Object> EvalCell(const std::vector<std::shared_ptr<Object>>& args,
                                 const std::string& op) {
    if (op == "cons") {
        return std::make_shared<Cell>(args[0], args[1]);
    }

    if (op == "car" || op == "cdr") {
        auto cell = As<Cell>(args[0]);
        if (!cell) {
            throw RuntimeError("");
        }
        return op == "car" ? cell->first_ : cell->second_;
    }
    throw RuntimeError("");
}

std::shared_ptr<Object> Eval(std::shared_ptr<Object> ast) {
    if (!ast) {
        throw RuntimeError("");
    }

    if (Is<Number>(ast) || Is<Boolean>(ast)) {
        return ast;
    }

    if (Is<Symbol>(ast)) {
        if (As<Symbol>(ast)->name_ == "#t") {
            return std::make_shared<Boolean>(true);
        } else {
            return std::make_shared<Boolean>(false);
        }
    }

    auto cell = As<Cell>(ast);
    auto symb = As<Symbol>(cell->first_);
    if (!symb) {
        throw RuntimeError("");
    }

    if (symb->name_ == "quote" || symb->name_ == "and" || symb->name_ == "or") {
        return EvalQuotes(cell, symb->name_);
    }

    std::vector<std::shared_ptr<Object>> args;
    auto curr = cell->second_;
    while (auto curr_cell = As<Cell>(curr)) {
        args.push_back(Eval(curr_cell->first_));
        curr = curr_cell->second_;
    }

    if (symb->name_ == "number?") {
        return std::make_shared<Boolean>(Is<Number>(args[0]));
    }

    if (symb->name_ == "boolean?" || symb->name_ == "not") {
        return EvalBool(args, symb->name_);
    }

    if (symb->name_ == "list" || symb->name_ == "list?" || symb->name_ == "null?" ||
        symb->name_ == "pair?" || symb->name_ == "list-ref" || symb->name_ == "list-tail") {
        return EvalList(args, symb->name_);
    }

    if (symb->name_ == "cons" || symb->name_ == "car" || symb->name_ == "cdr") {
        return EvalCell(args, symb->name_);
    }

    std::vector<int64_t> nums;
    for (auto& arg : args) {
        if (auto num = As<Number>(arg)) {
            nums.push_back(num->GetValue());
        } else {
            throw RuntimeError("");
        }
    }

    if (symb->name_ == "+" || symb->name_ == "-" || symb->name_ == "*" || symb->name_ == "/") {
        return EvalInteger(nums, symb->name_);
    }

    if (symb->name_ == "=" || symb->name_ == "<" || symb->name_ == ">" || symb->name_ == "<=" ||
        symb->name_ == ">=") {
        return EvalSymbol(nums, symb->name_);
    }

    if (symb->name_ == "min" || symb->name_ == "max" || symb->name_ == "abs") {
        return EvalNumeric(nums, symb->name_);
    }

    throw RuntimeError("");
}

std::string Output(std::shared_ptr<Object> obj) {
    if (!obj) {
        return "()";
    }

    if (Is<Number>(obj)) {
        return std::to_string(As<Number>(obj)->GetValue());
    }

    if (Is<Boolean>(obj)) {
        return As<Boolean>(obj)->value_ ? "#t" : "#f";
    }

    if (Is<Symbol>(obj)) {
        return As<Symbol>(obj)->GetName();
    }

    if (Is<Cell>(obj)) {
        std::string res = "(";
        auto curr = As<Cell>(obj);
        res.append(Output(curr->GetFirst()));
        auto tmp = curr->GetSecond();
        while (tmp) {
            if (!Is<Cell>(tmp)) {
                res.append(" . ");
                res.append(Output(tmp));
                break;
            }
            res.push_back(' ');
            curr = As<Cell>(tmp);
            res.append(Output(curr->GetFirst()));
            tmp = curr->GetSecond();
        }
        res += ')';
        return res;
    }
    throw RuntimeError("");
}

std::string Interpreter::Run(const std::string& input) {
    std::stringstream ss{input};
    Tokenizer tokenizer{&ss};
    auto ast = Read(&tokenizer);
    auto res = Eval(ast);
    return Output(res);
}