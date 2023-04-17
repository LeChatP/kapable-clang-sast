/**
 * Trying to implement the Composite pattern in C++ to represent the capabilities checked by the kernel.
*/
#include "ConditionCap.h"
#include "clang/AST/OperationKinds.h"
#include <algorithm>
#include <iostream>
#include <list>
#include <string>

using namespace clang;

class ContextualizerCap {
public:
  static ContextCap toContextCap(clang::IfStmt *ifStmt, clang::ASTContext *context) {
    // Get the capability from the condition.
    ConditionCap *capability = getCapabilities(ifStmt->getCond());

    // Get the function name.
    std::string function_name = getFunctionName(context);

    // Get the return value.
    bool followed_by_return = getFollowedByReturn(context);

    return ContextCap(capability, function_name,
                      followed_by_return);
  }

private:
  static ConditionCap *getCapabilities(clang::Expr *expr) {
    if (expr == nullptr)
      return nullptr;

    // Check if the expression is a call to the "capable" function.
    if (clang::CallExpr *call = dyn_cast<clang::CallExpr>(expr)) {
      if (clang::FunctionDecl *func = call->getDirectCallee()) {
        if (func->getName() == "capable") {
          // Get the capability name.
          std::string capability = call->getArg(0);

          // Create the capability object.
          return new Cap(capability);
        } else if (func->getName() == "ns_capable") {
          // Get the capability name.
          std::string capability = call->getArg(1);

          // Create the capability object.
          return new Cap(capability);
        }
      }
    }

    // Check if the expression is a binary operator.
    if (clang::BinaryOperator *binop = dyn_cast<clang::BinaryOperator>(expr)) {
      // Check if the right operand is a call to the "capable" function.
      ConditionCap *rhs = getCapabilities(binop->getRHS(), context);
      if (rhs != nullptr) {
        // Create the capability object.
        return new BinaryConditionCap(
            binop->getOpcodeStr(), rhs,
            getCapabilities(binop->getRHS(), context));
      }

      // Check if the left operand is a call to the "capable" function.
      ConditionCap *lhs = getCapabilities(binop->getLHS(), context);
      if (lhs != nullptr) {
        // Create the capability object.
        return new BinaryConditionCap(
            binop->getOpcodeStr(), lhs,
            getCapabilities(binop->getLHS(), context));
      }
    }

    if (clang::UnaryOperator *unop = dyn_cast<clang::UnaryOperator>(expr)) {
      // Check if the operand is a call to the "capable" function.
      ConditionCap *operand = getCapabilities(unop->getSubExpr(), context);
      if (operand != nullptr) {
        // Create the capability object.
        return new UnaryConditionCap(unop->getOpcodeStr(), operand);
      }
    }

    return nullptr;
  }

  static std::string getFunctionName(clang::ASTContext *context) {
    // Get the function declaration.
    clang::FunctionDecl *func = context->getFunctionLevelDecl(0);

    // Get the function name.
    std::string function_name = func->getName();

    return function_name;
  }

  static std::vec<std::string> getSyscallName(clang::ASTContext *context) {
    // Get current CallExpr
    clang::CallExpr *call = context->getCallExpr();
    // get parents of CallExpr
    clang::DynTypedNodeList *parent = context->getParents(call);
    // Get the function name.
    std::string function_name = func->getName();
    // get call graph

  }
}

namespace capable {
  class ContextCap {
private:
  ConditionCapPtr capability;
  std::string function_name;
  bool followed_by_return;
public:
  ContextCap(ConditionCapPtr cap, const std::string& func, const std::string& syscall, bool ret) : 
    capability(cap), function_name(func), syscall_name(syscall), followed_by_return(ret) {}
  std::string toJson() {
    std::string json = "{";
    json += "\"capability\": " + capability->toJson() + ",";
    json += "\"function_name\": \"" + function_name + "\",";
    json += "\"followed_by_return\": " + std::to_string(followed_by_return);
    json += "}";
    return json;
  }
};


/**
 * The base ConditionCap class declares common operations for both simple and
 * complex objects of a composition.
 */
class ConditionCap {
  /**
   * @var ConditionCap
   */
protected:
  ConditionCapPtr parent_ = nullptr;
public:
  virtual ~ConditionCap() {}
  void SetParent(ConditionCapPtr parent) { this->parent_ = parent; }
  ConditionCapPtr GetParent() const { return this->parent_; }
  virtual bool IsComposite() const { return false; }
  virtual std::string toJson() const = 0;
};

class Cap : public ConditionCap {
private:
  std::string capability = "";

public:

  explicit Cap(const std::string& capabilit) : capability(capabilit) {}
  std::string toJson() {
    return "\"" + capability + "\"";
  }
};

class UnaryConditionCap : public ConditionCap {
  /**
   * @var \SplObjectStorage
   */
protected:
  ConditionCapPtr child;
  clang::UnaryOperatorKind op;

public:
  /**
   * A composite object can add or remove other ConditionCaps (both simple or
   * complex) to or from its child list.
   */
  void Set(ConditionCapPtr ConditionCap) {
    this->child = ConditionCap;
    ConditionCap->SetParent((ConditionCapPtr)this);
  }
  /**
   * Have in mind that this method removes the pointer to the list but doesn't
   * frees the
   *     memory, you should do it manually or better use smart pointers.
   */
  void Remove() {
    if (this->child != nullptr) {
      this->child->SetParent(nullptr);
    }
    this->child = nullptr;
  }
  bool IsComposite() { return true; }
  std::string toJson() {
    return "{\"op\": \"" + std::to_string(op) + "\", \"child\": " + child->toJson() + "}";
  }
};

/**
 * The Composite class represents the complex ConditionCaps that may have
 * children. Usually, the Composite objects delegate the actual work to their
 * children and then "sum-up" the result.
 */
class BinaryConditionCaps : public ConditionCap {
  /**
   * @var \SplObjectStorage
   */
protected:
  ConditionCapPtr left;
  ConditionCapPtr right;
  clang::BinaryOperatorKind op;

public:
  void SetLHS(ConditionCapPtr ConditionCap) {
    this->left = ConditionCap;
  }
  void SetRHS(ConditionCapPtr ConditionCap) {
    this->right = ConditionCap;
  }
  /**
   * Have in mind that this method removes the pointer to the list but doesn't
   * frees the
   *     memory, you should do it manually or better use smart pointers.
   */
  void DelLHS() {
    if (this->left != nullptr) {
      this->left->SetParent(nullptr);
    }
    this->left = nullptr;
  }
  void DelRHS() {
    if (this->right != nullptr) {
      this->right->SetParent(nullptr);
    }
    this->right = nullptr;
  }
  bool IsComposite() const override { return true; }
  /**
   * The Composite executes its primary logic in a particular way. It traverses
   * recursively through all its children, collecting and summing their results.
   * Since the composite's children pass these calls to their children and so
   * forth, the whole object tree is traversed as a result.
   */
  std::string toJson() const override {
    std::string result;
    result += "{ \"op\" : " + op;
    if (left != nullptr)
      result += ", \"left\" : " + left->toJson();
    if (right != nullptr)
      result += ", \"right\" : " + right->toJson();
    result += "}";
    return result;
  }
};

}



