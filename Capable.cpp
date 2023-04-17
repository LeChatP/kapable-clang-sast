#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;


namespace {
// this class contains the context of conditions with capable() call
// This would be used to contextualize ebpf results with the source code.
// Then ebpf results could determine if the capable call is needed or not.
class IfConditionVisitor : public RecursiveASTVisitor<IfConditionVisitor> {
public:
  IfConditionVisitor() : Context(nullptr) {}
  bool TraverseIfStmt(IfStmt *stmt) {
    // Print an error/warning message if the "capable" function is not called in
    // the last part of the condition.
    int capable = hasCapable(stmt->getCond());
    if (capable > 0) {
      if (capable == 1) {
        clang::SourceLocation loc = stmt->getCond()->getEndLoc();
        clang::DiagnosticsEngine &diagEngine = Context->getDiagnostics();
        unsigned diagID = diagEngine.getCustomDiagID(
            DiagnosticsEngine::Warning,
            "Condition with capable() should end with capable() call");
        diagEngine.Report(loc, diagID);
      }
      // populate the list of conditions with capable() call
    }

    return RecursiveASTVisitor::TraverseIfStmt(stmt);
  }

private:
  int hasCapable(Expr *expr) {
    if (expr == nullptr)
      return false;

    // Check if the expression is a call to the "capable" function.
    if (clang::CallExpr *call = dyn_cast<CallExpr>(expr)) {
      if (FunctionDecl *func = call->getDirectCallee()) {
        if (func->getName() == "capable" || func->getName() == "ns_capable")
          return 2;
      }
    }

    // Check if the expression is a binary operator.
    if (clang::BinaryOperator *binop = dyn_cast<BinaryOperator>(expr)) {
      // Check if the right operand is a call to the "capable" function.
      int rhs = hasCapable(binop->getRHS());
      if (rhs >= 1)
        return rhs;

      // Check if the left operand is a call to the "capable" function.
      if (hasCapable(binop->getLHS()) >= 1)
        return 1;
    }

    if (isa<UnaryOperator>(expr))
      return hasCapable(cast<UnaryOperator>(expr)->getSubExpr());

    return false;
  }

  clang::ASTContext *Context;

public:
  void setContext(ASTContext &context) { Context = &context; }
};

class IfConditionASTConsumer : public ASTConsumer {
public:
  IfConditionASTConsumer() : Visitor() {}

  void HandleTranslationUnit(ASTContext &context) override {
    Visitor.setContext(context);
    Visitor.TraverseDecl(context.getTranslationUnitDecl());
  }

private:
  IfConditionVisitor Visitor;
};

class IfConditionASTAction : public PluginASTAction {
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
    return std::make_unique<IfConditionASTConsumer>();
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    return true;
  }

  PluginASTAction::ActionType getActionType() override {
    return PluginASTAction::ActionType::AddAfterMainAction;
  }
};

} // namespace

static clang::FrontendPluginRegistry::Add<IfConditionASTAction>
    X("if-condition", "check if the 'capable' function is called in the last "
                      "part of the condition of an if statement");