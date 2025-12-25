#ifndef __TREENODE_H__
#define __TREENODE_H__

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

	Node* search_tree(bool (*cmp)(void*))
	{
		Node* n {nullptr};

		if (!cmp) {
			return n;
		}

		if (cmp(this)) {
			n = static_cast<Node*>(this);
		}

		if (!n && _child) {
			n = _child->search_tree(cmp);
		}

		if (!n && _sibling) {
			n = _sibling->search_tree(cmp);
		}

		return n;
	}

	void print_tree(void (*prn)(void*))
	{
		if (!prn) {
			return;
		}

		prn(this);

		if (_child) {
			_child->print_tree(prn);
		}

		if (_sibling) {
			_sibling->print_tree(prn);
		}
	}

	void update_tree(void (*upd)(void*))
	{
		if (!upd) {
			return;
		}

		upd(this);

		if (_child) {
			_child->update_tree(upd);
		}

		if (_sibling) {
			_sibling->update_tree(upd);
		}
	}

private:

	Node* _sibling {nullptr};
	Node* _child {nullptr};
};

} // namespace treenode_ns

#endif //__TREENODE_H__
