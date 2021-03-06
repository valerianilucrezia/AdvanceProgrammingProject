#ifndef _bst_node
#define _bst_node

#include <iostream>
#include <memory>
#include <utility>
#include <iterator>
#include <vector>
#include "iterator.hpp"

/*!
	@file bst.hpp
	@authors Buscaroli Elena, Valeriani Lucrezia
	@brief Header containing the implementation of node struct and bst class
*/


/*!
	@tparam T Template for an object of class pair_type.
*/
template < typename T >
struct node{

/*!
	@brief Unique pointer to the right node.
*/
	std::unique_ptr<node> right;
	
/*!
	@brief Unique pointer to the left node.
*/
	std::unique_ptr<node> left;
	
/*!
	@brief Raw pointer to the parent node.
*/
	node* parent;

/*!
	@brief Member variable of type pair_type.
*/
	T value;

/*!
	@brief Constructor for node in which right and left are initialized to nullptr, parent to the input pointer to node and value to the input data x.
	@tparam x const lvalue reference.
	@tparam p raw pointer to the parent node.
*/
	explicit node(const T& x, node* p=nullptr): right{nullptr}, left{nullptr}, parent{p}, value{x} {}


/*!
	@brief Constructor for node in which right, left and parent are inizialized to input pointer and value to the input data x.
	@tparam x const lvalue reference.
	@tparam r pointer to the right node.
	@tparam l pointer to the left node.
	@tparam p pointer to the parent node.
*/
	node(const T& x, node* r, node* l, node* p=nullptr): right{r}, left{l}, parent{p}, value{x} {}


/*!
	@brief Constructor for node in which right and left node are initialized to nullptr, parent to the input pointer to node and value to the input data x.
	@tparam x rvalue reference.
	@tparam p raw pointer to the parent node.
*/
	explicit node(T&& x, node* p=nullptr): right{nullptr}, left{nullptr}, parent{p}, value{std::move(x)} {}


/*!
	@brief Constructor for node in which right, left and parent are inizialized to input pointer and value to the input data x.
	@tparam x rvalue reference.
	@tparam r pointer to the right node.
	@tparam l pointer to the left node.
	@tparam p pointer to the parent node.
*/
	node(T&& x, node* r, node* l, node* p=nullptr): right{r}, left{l}, parent{p}, value{std::move(x)} {}

/*!
	@brief Copy constructor for node, used by the copy constructor of bst.
	@brief Set the parent with the input pointer, copies the value and recursively calls itself on the left and right node.
	@tparam x std::unique_ptr to node to be copied. 
	@tparam p pointer to the parent node.
*/
	explicit node(const std::unique_ptr<node>& x, node* p=nullptr): parent{p}, value{x->value}{
		if(x->right)
			right.reset(new node{x->right, this});

		if(x->left)
			left.reset(new node{x->left, this});
	}

/*!
	@brief Default destructor.
*/
	~node() noexcept = default;
};




template < typename value_type, typename key_type, typename cmp_op=std::less<key_type> >
class bst{
	using pair_type = std::pair< const key_type, value_type >;
	using node_t = node< pair_type >;
	using iterator = _iterator< node_t , pair_type > ;
	using const_iterator = _iterator< node_t , const pair_type > ;

/*!
	@brief Definition of the unique pointer to the root node of the tree.
*/
	std::unique_ptr<node_t> root;

/*!
	@brief Comparison operator.
*/
	cmp_op op;


/*!
	@brief This function checks if a node is present in the tree by checking its key.
	@brief It iteratively compare the key of each node to the one of its right node, if it is bigger, or of its left node, if it is smaller.
	@tparam x const lvalue reference to the key to look for.
	@return Raw pointer to the found node or a nullptr if the key was not found.
*/
	node_t* _find(const key_type& x) {
		if(!root) { return nullptr; }
		node_t* tmp = root.get();
		while(tmp) {
			if(tmp->value.first == x) { return tmp; }
			else if( op(x, tmp->value.first) ) { tmp = tmp->left.get(); }
			else if( op(tmp->value.first, x) ) { tmp = tmp->right.get(); }
			else { return nullptr; }
		}
		return nullptr;
	}


/*!
	@brief This function inserts a new node in the tree, if not present. 
	@brief It first checks if the key is already present, by means of the _find function, and if the pair is not present in the tree, it searches for the right place to insert it.
	@tparam x reference to the pair to be inserted.
	@return std::pair<iterator,bool> iterator to the inserted node, true, or iterator to the already existing node, false.
*/
	template <typename O>
	std::pair<iterator, bool> _insert(O&& x){
		node_t* n{_find(x.first)};
		if (n) { return std::make_pair(iterator{n}, false); }

		auto tmp = root.get();
		while(tmp) { 
			if( op(tmp->value.first, x.first) ){ 
				if (tmp->right) { tmp = tmp->right.get(); }
				else {
					tmp->right = std::make_unique<node_t> (std::forward<O>(x), tmp); 
					return std::make_pair(iterator{tmp->right.get()}, true); 
				}
			}
			if( op(x.first,tmp->value.first) ){
				if(tmp->left){ tmp = tmp->left.get(); }
				else{
					tmp->left = std::make_unique<node_t> (std::forward<O>(x), tmp);
					return std::make_pair(iterator{tmp->left.get()}, true); 
				}
			}
		}
		root = std::make_unique<node_t> (std::forward<O>(x), nullptr);
		return std::make_pair(iterator{root.get()}, true);
	}


/*!
	@brief This function searches for the left-most node in the tree rooted at the input node x. 
	@brief If the input node does not have any left nodes, it simply returns the input node.
	@tparam x raw pointer to the input node.
	@return Raw pointer to the left-most node in the tree rooted at x.
*/
	node_t* _inorder(node_t* x) {
		while(x->left) { x = x->left.get(); }
		return x;
	}


/*!
	@brief This functions calls recursively itself in order to balance the tree.
	@tparam nodes reference to the vector containing the node pairs ordered by keys.
	@tparam start integer index of the first element of the vector.
	@tparam end integer index of the last element of the vector, namely its length.
*/
	void _balance(std::vector<pair_type>& nodes, int start, int end){
		if (start < end) {
			int mid = (start + end)/2; 
			insert(nodes[mid]);
			_balance(nodes, start, mid);
			_balance(nodes, mid+1, end); 
		}
	}


	public:

/*!
	@brief Default constructor for the bst class.
*/
		bst() noexcept = default;


/*!
	@brief Default destructor for the bst class.
*/
		~bst() noexcept = default;


/*!
	@brief This function calls the _insert function to insert a new node in the tree, if not already present.
	@tparam x const lvalue reference to the pair to be inserted.
	@return std::pair<iterator, bool> returned by the _insert function.
*/
		std::pair<iterator, bool> insert(const pair_type& x) { return _insert(x); }

/*!
	@brief This function calls the _insert function using std::move() to insert a new node in the tree, if not already present.
	@tparam x rvalue reference to the pair to be inserted.
	@return std::pair<iterator, bool> returned by the _insert function.
*/
		std::pair<iterator, bool> insert(pair_type&& x) { return _insert(std::move(x)); }


/*!
	@brief This function calls the insert function to insert a new node in the tree, if not already present, by both giving as input std::pair<key, value> and giving the key and the value.
	@tparam args a std::pair<key, value> or a key and value.
	@return std::pair<iterator, bool> returned by the insert function.
*/
		template< class... Types >
		std::pair<iterator,bool> emplace(Types&&... args){ return insert(pair_type(std::forward<Types>(args)...)); }


/*!
	@brief This functions clears the content of the tree by setting the root node to nullptr.
*/
		void clear() noexcept { root.reset(); }


/*!
	@brief This function searches for the left-most node of the tree and returns an iterator pointing to that node.
	@return iterator to the left-most node.
*/
 		iterator begin() noexcept {
			if(!root) { return iterator{nullptr}; }
			node_t* tmp = root.get();
			while(tmp->left) { tmp = tmp->left.get(); }
			return iterator{tmp};
		}

/*!
	@brief This function searches for the left-most node of the tree and returns a const iterator pointing to that node.
	@return const_iterator to the left-most node.
*/
		const_iterator begin() const noexcept {
			if(!root) { return const_iterator{nullptr}; }
			node_t* tmp = root.get();
			while(tmp->left) { tmp = tmp->left.get(); }
			return const_iterator{tmp};
		}

/*!
	@brief This function searches for the left-most node of the tree and returns a const iterator pointing to that node.
	@return Const iterator to the left-most node.
*/
		const_iterator cbegin() const noexcept {
			if(!root) { return const_iterator{nullptr}; }
			node_t* tmp = root.get();
			while(tmp->left) { tmp = tmp->left.get(); }
			return const_iterator{tmp};
		}


/*!
	@brief This function returns an iterator pointing to one past the last element of the tree, namely the right-most node.
	@return Iterator to one past the last node.
*/
		iterator end() noexcept { return iterator{nullptr}; }

/*!
	@brief This function returns a const iterator pointing to one past the last element of the tree, namely the right-most node.
	@return Const iterator to one past the last node.
*/
		const_iterator end() const noexcept { return const_iterator{nullptr}; }

/*!
	@brief This function returns a const iterator pointing to one past the last element of the tree, namely the right-most node.
	@return Const iterator to one past the last node.
*/
		const_iterator cend() const noexcept { return const_iterator{nullptr}; }


/*!
	@brief This function calls _find to check if a node is present in the tree by checking its key.
	@tparam x const lvalue reference to the key to look for.
	@return Iterator pointing to the found node or to a null pointer if the key was not found.
*/
		iterator find(const key_type& x) { return iterator{_find(x)}; };

/*!
	@brief This function calls _find to check if a node is present in the tree by checking its key.
	@tparam x const lvalue reference to the key to look for.
	@return Const iterator pointing to the found node or to a null pointer if the key was not found.
*/
		const_iterator find(const key_type& x) const { return const_iterator{_find(x)}; };


/*!
	@brief Copy constructor for bst that creates a deep copy of the tree by calling the node copy constructor on the root node.
	@tparam x const lvalue reference to the tree.
*/
		bst(const bst& x) : op{x.op} {
			if (x.root) { root.reset(new node_t{x.root}); }
		};

/*!
	@brief Copy assignment for a bst tree.
	@tparam x const lvalue reference to the tree to be copied.
	@return lvalue reference to the copied tree.
*/
		bst& operator=(const bst& x){
			root.reset();
			auto tmp = x; //copyctor
			*this = std::move(tmp); //move ass
			return *this;
		}


/*!
	@brief Default move constructor for bst.
	@tparam x rvalue reference to the tree.
*/
		bst(bst&& x) noexcept = default;

/*!
	@brief Default move assignment for bst.
	@tparam x rvalue reference to the tree.
*/
		bst& operator=(bst&& x) noexcept = default;


/*!
	@brief This function is used to balance the tree, by means of the _balance function.
	@brief It first creates a vector containing the nodes pairs ordered by key values, clear the unbalanced tree and then calls _balance on the nodes vector to generate a balanced tree.
*/
		void balance(){
			std::vector<pair_type> nodes;
			for (const auto& i : *this) { nodes.push_back(i); }
			clear();
			_balance(nodes, 0, nodes.size());
		}

/*!
	@brief Overloaded operator that search the key to return corresponding associated value. 
	@brief If the key is not present in the tree, it inserts a node with that key and as value the default value of the value_type.
	@tparam x const lvalue reference to key.
	@return lvalue reference to the value mapped by the key.
*/
		value_type& operator[](const key_type& x) {
			value_type v{};
			auto i = insert(std::pair<key_type,value_type>(x, v));
			return (*i.first).second;
		 }

/*!
	@brief Overloaded operator that search the key to return corresponding associated value. 
	@brief If the key is not present in the tree, it inserts a node with that key and as value the default value of the value_type.
	@tparam x rvalue reference to key.
	@return lvalue reference to the value mapped by the key.
*/
		value_type& operator[](key_type&& x) {
			value_type v{};
			auto i = insert(std::pair<key_type,value_type>(x, v));
			return (*i.first).second;
		 }


/*!
	@brief Friend operator that prints the tree ordered by key values using const iterators.
	@tparam os std::ostream& output stream object.
	@tparam x const lvalue reference to bst tree.
	@return std::ostream& output stream object.
*/
		friend std::ostream& operator<<(std::ostream& os, const bst& x) noexcept {
			for(const auto& i: x) {
				os << i.first << " ";
			}
			return os;
		}


/*!
	@brief This function erases the content of the node with key equal to the input one, if present. 
	@brief First of all, the function calls the _find function to check if the key is present or not in the tree. If not, the function returns. Then we might face three different situations, the node to be erased
	has no left and right nodes, namely it is a leaf, it might have only one left or right node, or it has both left and right nodes. In all the situations, we need to check if the node to be erased is the root node and
	release the node parent's ownership, and in the latter two cases, we also need to reset the ownership of the node to left and/or right nodes.
	When the node to be erased has two children nodes, the function will call _inorder to find the first inorder successor, and then calls itself to erase the successor node, after having stored its value.
	@tparam x const lvalue of the key to look for.
*/
		void erase(const key_type& x);
}; 


template < typename value_type, typename key_type, typename cmp_op >
void bst<value_type,key_type,cmp_op>::erase(const key_type& x){
	node_t* n{_find(x)};
	if(!n) { return; }

	if( !(n->left) && !(n->right) ) { 
		if( !(n == root.get()) ) {
			if( n == n->parent->left.get() ) { n->parent->left.reset(); }
			else { n->parent->right.reset(); }
		}
		else { root.reset(); }
	}

	else if( n->left && n->right ) {
		node_t* in = _inorder(n->right.get());
		auto v = in->value;
		bool left = false;
		erase(v.first);

		if( n == root.get() ) {
			n = new node_t{v, n->right.release(), n->left.release(), n->parent};
			root.reset(n);
			if(n->left) { n->left->parent = n; }
			if(n->right) { n->right->parent = n; }
		}
		else {
			if( n->parent->left.get() == n ) { left = true; }
			n = new node_t{v, n->right.release(), n->left.release(), n->parent};
			if(n->left) { n->left->parent = n; }
			if(n->right) { n->right->parent = n; }

			if( left ) { n->parent->left.reset(n); }
			else { n->parent->right.reset(n); }
		}
	}

	else { 
		if( n == root.get() ) {
			if(n->right) {
				n->right->parent = nullptr;
				root.reset(n->right.release());
			}
			else {
				n->left->parent = nullptr;
				root.reset(n->left.release());
			}
		}
		else {
			if( n->parent->left.get() == n ) {
				if( n->left ) { 
					n->left->parent = n->parent;
					n->parent->left.reset(n->left.release()); 
				}
				else { 
					n->right->parent = n->parent;
					n->parent->left.reset(n->right.release());
				}
			}
			else {
				if( n->left ) { 
					n->left->parent = n->parent;
					n->parent->right.reset(n->left.release()); 
				}
				else { 
					n->right->parent = n->parent;
					n->parent->right.reset(n->right.release()); 
				}
			}
		}
	}
}


#endif