#pragma once

// Template class that has to be completely
// implemented in header
namespace treenode_ns
{

template <typename Node>
class treenode
{
public:

	treenode() = default;
	virtual ~treenode() = default;

	void add_node(Node* parent)
	{
		if (!parent) {
			return;
		}

		if (!parent->_child) {
			parent->_child = static_cast<Node*>(this);
		}
		else {
			static_cast<Node*>(this)->_sibling = parent->_child;
			parent->_child = static_cast<Node*>(this);
		}
	}

	using cmp_fn = bool (*)(Node* node, void* data);
	using visit_fn = void (*)(Node* node, void* data);

	Node* search_tree(cmp_fn cmp, void* data = nullptr)
	{
		if (!cmp) {
			return nullptr;
		}

		Node* self = static_cast<Node*>(this);

		if (cmp(self, data)) {
			return self;
		}

		if (_child) {
			if (Node* n = _child->search_tree(cmp, data)) {
				return n;
			}
		}

		if (_sibling) {
			if (Node* n = _sibling->search_tree(cmp, data)) {
				return n;
			}
		}

		return nullptr;
	}

	void print_tree(visit_fn prn, void* data = nullptr)
	{
		if (!prn) {
			return;
		}

		prn(static_cast<Node*>(this), data);

		if (_child) {
			_child->print_tree(prn, data);
		}
		if (_sibling) {
			_sibling->print_tree(prn, data);
		}
	}

	void update_tree(visit_fn upd, void* data = nullptr)
	{
		if (!upd) {
			return;
		}

		upd(static_cast<Node*>(this), data);

		if (_child) {
			_child->update_tree(upd, data);
		}
		if (_sibling) {
			_sibling->update_tree(upd, data);
		}
	}

private:

	Node* _sibling {nullptr};
	Node* _child {nullptr};

	friend Node;
};

} // namespace treenode_ns
