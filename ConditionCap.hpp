#ifndef CONDITIONCAP_H
#define CONDITIONCAP_H
#include <string>


namespace capable {
  class ContextCap {
private:
  ConditionCapPtr capability;
  std::string function_name;
  std::string syscall_name;
  bool followed_by_return;
public:
  ContextCap(ConditionCapPtr cap, const std::string& func, const std::string& syscall, bool ret);
  std::string toJson();
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
  virtual ~ConditionCap();
  void SetParent(ConditionCapPtr parent);
  ConditionCapPtr GetParent() const { return this->parent_; }
  virtual bool IsComposite() const { return false; }
  virtual std::string toJson() const { return "{}"; }
};

class Cap : public ConditionCap {
private:
  std::string capability = "";

public:

  explicit Cap(const std::string& capabilit);
  std::string toJson();
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
  void Set(ConditionCapPtr ConditionCap) ;
  /**
   * Have in mind that this method removes the pointer to the list but doesn't
   * frees the
   *     memory, you should do it manually or better use smart pointers.
   */
  void Remove();
  bool IsComposite();
  std::string toJson() ;
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
  void SetLHS(ConditionCapPtr ConditionCap);
  void SetRHS(ConditionCapPtr ConditionCap);
  /**
   * Have in mind that this method removes the pointer to the list but doesn't
   * frees the
   *     memory, you should do it manually or better use smart pointers.
   */
  void DelLHS();
  void DelRHS();
  bool IsComposite();
  /**
   * The Composite executes its primary logic in a particular way. It traverses
   * recursively through all its children, collecting and summing their results.
   * Since the composite's children pass these calls to their children and so
   * forth, the whole object tree is traversed as a result.
   */
  std::string toJson();

  
};
typedef class ContextCap* ContextCapPtr;
typedef class ConditionCap* ConditionCapPtr;
} // namespace capable

#endif // CONDITIONCAP_H