#ifndef __bst_hpp
#define __bst_hpp

#include <algorithm>
#include <iostream>
#include <memory>
#include <iterator>
#include <utility>

template <typename T>
class node {
    T value;
    std::unique_ptr<node> left;
    std::unique_ptr<node> right;
    node* parent;

    public:
        node(T p): value{p}, parent{nullptr} {};
        node(T p, node* n): value{p}, parent{n} {};
        //Explicit node copy constructor
        explicit node(const std::unique_ptr<node> &p, node* parent): value{p->value}{
            this->parent = parent;
            if(p->right)
                right = std::make_unique<node>(p->right, this); //We need *this in order to copy the parent
            if(p->left)
                left = std::make_unique<node>(p->left, this);
        }

        // //This is wrong: does not copy parent
        // explicit node(const std::unique_ptr<node> &p): value{p->value}{
        //     if(p->right)
        //         right = std::make_unique<node>(p->right); //We need *this in order to copy the parent
        //     if(p->left)
        //         left = std::make_unique<node>(p->left);
        // } //TODO: value or getValue()
        

        //TODO: Move constructor, do we need it?
        //node(T &&p): value{std::move(p)}, left{nullptr}, right{nullptr}, parent{nullptr} {};
        ~node() {}
        using value_type = T;

        //getters
        //TODO: Should we return a value or a reference to the value? Does this mean we can actually
        //Modify the value of the node? 
        T& getValue() { return value;}
        node* getLeft() const {return left.get();}
        node* getRight() const {return right.get();}
        node* getParent() const {return parent;}

        //setters
        void setLeft(node* x) { left.reset(x); }
        void setRight(node* x) { right.reset(x); }
        void setParent(node* x) { parent = x; }
};

template <typename node_type, typename T>
class _iterator {
    node_type* current;
    node_type* next() noexcept;
    node_type* previous() noexcept;

    public:
        _iterator() noexcept: current{nullptr} {};
        explicit _iterator(node_type* x) noexcept : current{x} {};
        //TODO: do we need an iterator destructor?
        using value_type = T;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category =std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;

        //BIGTODO: How the hell is this actually working? 
        //Actually a random super good guy said this is compiler doing its things under curtains
        //"But it's still bad, don't do it"
        // reference operator*() const noexcept { 
        //     value_type d = current->getValue();
        //     value_type &ref = d;
        //     return ref; 
        // }
        reference operator*() const noexcept { 
            return(current->getValue());
        }
        
        pointer operator->() const noexcept { return &(*(*this)); }

        _iterator& operator++() noexcept {  // pre increment
            current = next();
            return *this;
        }

        _iterator operator++(int) noexcept {
            _iterator tmp{current};
            ++(*this);
            return tmp;
        }

        _iterator& operator--() noexcept { //pre decrement
            current = previous();
            return *this;
        }

        _iterator operator--(int) noexcept {
            _iterator tmp{current};
            --(*this);
            return tmp;
        }

        friend bool operator==(const _iterator& a, const _iterator& b) {
            return a.current == b.current;
        }

        friend bool operator!=(const _iterator& a, const _iterator& b) {
            return !(a == b);
        }

        //getter
        node_type* getCurrent() const {return current;}

        //setter
        void setCurrent(node_type* x) { current = x;}
};

template <typename k, typename v, typename c = std::less<k> >
class bst{
    using node_type = node<std::pair<const k,v> >;
    c op;
    std::unique_ptr<node_type> head;

    public:
        bst(): op{c()}, head{nullptr} {};
        bst(c comp): op{comp}, head{nullptr} {};
        bst(k key, v value, c comp): op{comp}, head{ std::make_unique<node_type>(std::pair<k,v>(key,value))} {};

        //bst& operator=(const bst &b) { head = std::make_unique<node>(b.head, nullptr);}
        //~bst() { delete head; }

        using pair_type = typename node_type::value_type;
        using iterator = _iterator<node_type, pair_type>;
        using const_iterator = _iterator<node_type, const pair_type>;

        std::pair<iterator, bool> insert(const pair_type& x);
        std::pair<iterator, bool> insert(pair_type&& x);

        // TODO: what if we have an odd number of args?? Possibly exception!
        // probably args should be pairs because in that way we can pass different types of k and v
        // ask SARTORIIII
        // TODO: if insert thorows an exception, should emplace throw it too?
        // TODO: to implement
        template<class... Types>
        std::pair<iterator,bool> emplace(Types&&... args); 

        //TODO: to implement
        void clear() noexcept; 

        iterator begin() noexcept;
        const_iterator begin() const noexcept;
        // TODO: maybe add cbegin () without const??
        const_iterator cbegin() const noexcept; 

        iterator end() noexcept {return iterator{nullptr};}
        const_iterator end() const noexcept { return const_iterator{nullptr};}
        const_iterator cend() const noexcept { return const_iterator{nullptr};}

        iterator find(const k& x) noexcept; 
        const_iterator find(const k& x) const; 

        //THIS FUNCTION IS FOR DEBUGGING PURPOSES, TO CHECH FIND FUNCTION 
        void isThere(const k& key){
            iterator result; 
            result = find(key);
            if(result == end()) std::cout<<"NOT FOUND"<<std::endl;
            else std::cout<<"FOUND"<<std::endl;
        }

        //TODO: to implement
        void balance(); 

        v& operator[](const k& x) {
            auto it = find(x);
            if(it != end())
                return (*it).second;
            else {
                std::cout << "The key does not exist. Insert a value for the new node\n";
                v inValue;
                std::cin >> inValue;
                auto p = insert({x,inValue});
                return (*(p.first)).second;
            }
        }

        v& operator[](k&& x){
            iterator it = find(std::move(x));
            if(it != end())
                return (*it).second;
            else {
                std::cout << "The key does not exist. Insert a value for the new node\n";
                v inValue;
                std::cin >> inValue;
                auto p = insert({std::move(x),inValue});
                return (*(p.first)).second;
            }
        }

        friend
        std::ostream& operator<<(std::ostream& os, const bst& x){
            auto it = x.begin();
            while (it != x.end()) {
                os << (*it).second << " ";
                ++it;
            }
            return os;
        }

        // copy semantic
        // copy constr 
        bst(const bst &b): op{b.op} { 
            head = std::make_unique<node_type>(b.head,nullptr);
        }
        //copy assignment, TODO: is there space to optimize this by calling the copy constructor?
        bst& operator=(const bst& b){
            this->clear();
            op = b.op;
            head = std::make_unique<node_type>(b.head,nullptr);
            return *this;
        } 

        // move semantic
        // move constr 
        bst(bst&& b) noexcept = default; //This actually works
        bst& operator=(bst&& b) noexcept = default; //move assignment

        //TODO: to implement
        void erase(const k& x); 
};


////////////////////////////////
/////                     //////
/////  ITERATOR FUNCTIONS //////
/////                     //////
////////////////////////////////


//Iterator functions
template <typename node_type, typename T>
node_type* _iterator<node_type,T>::next() noexcept{
    if(current->getRight() != nullptr) {
        current = current->getRight();
        while(current->getLeft() != nullptr)
            current = current->getLeft();
    } else {
        if(current->getParent() == nullptr){
            return nullptr;
        }
        // TODO: we would need to use a compare operator
        while(current->getParent()->getValue().first < current->getValue().first){
            current = current->getParent();
            if(current->getParent() == nullptr)
                break;
        }

        current = current->getParent();
    }
    return current;
}

template <typename node_type, typename T>
node_type* _iterator<node_type,T>::previous() noexcept{
    if(current->getLeft() != nullptr) {
        current = current->getLeft();

        while(current->getRight() != nullptr)
            current = current->getRight();
    } else {
        if(current->getParent() == nullptr){
            return nullptr;
        }
        // TODO: we would need to use a compare operator
        while(current->getParent()->getValue().first > current->getValue().first){
            current = current->getParent();
            if(current->getParent() == nullptr)
                return nullptr;
        }
    }
    return current;
}

///////////////////////////
/////                //////
/////  BST FUNCTIONS //////
/////                //////
///////////////////////////

//The first element of the bst is the leftmost element of the tree
template <typename k, typename v, typename c>
typename bst<k,v,c>::iterator bst<k,v,c>::begin() noexcept {

    if(head == nullptr)
        return iterator(nullptr);
    
    auto it = iterator(head);
    while( it.getCurrent()->getLeft() != nullptr)
        it.setCurrent(it.getCurrent()->getLeft());
    
    return it;   
}

template <typename k, typename v, typename c>
typename bst<k,v,c>::const_iterator bst<k,v,c>::begin() const noexcept {

    if(head == nullptr)
        return const_iterator(nullptr);

    auto it = const_iterator(head.get());
    while( it.getCurrent()->getLeft() != nullptr)
        it.setCurrent(it.getCurrent()->getLeft());
    
    return it;
}

template <typename k, typename v, typename c>
typename bst<k,v,c>::const_iterator bst<k,v,c>::cbegin() const noexcept{

    //TODO: maybe just call begin here
    if(head == nullptr)
        return const_iterator(nullptr);

    auto it = const_iterator(head.get());
    while( it.getCurrent()->getLeft() != nullptr)
        it.setCurrent(it.getCurrent()->getLeft());
    
    return it;
}

template <typename k, typename v, typename c>
std::pair<typename bst<k,v,c>::iterator,bool> bst<k,v,c>::insert(const pair_type& x){
    
    if (head == nullptr)
    {
        head = new node_type(x, nullptr);
        return(std::make_pair(iterator(head.get()),true));
    }
    
    node_type* new_node = nullptr;  
    node_type* tmp = head ;
    
    while (tmp != nullptr)
    {
        new_node = tmp;
        if (x.first < tmp->getValue().first)
        {
            tmp = tmp->getLeft();
        }
        else if ((x.first > tmp->getValue().first))
        {
            tmp = tmp->getRight();
        }  
        else
        {
            //if the key is already exist 
             return(std::make_pair(iterator(),false));
        }
        
    }

    tmp = std::make_unique<node_type>(x, new_node);
    if (x.first < new_node->getValue().first){ 
        new_node->setLeft(tmp); 
    }
    else{
        new_node->setRight(tmp); 
    }
     
    return(std::make_pair(iterator(tmp),true)); 
}

template <typename k, typename v, typename c>
std::pair<typename bst<k,v,c>::iterator,bool> bst<k,v,c>::insert(pair_type&& x){
    
    if (head == nullptr){
        head = std::make_unique<node_type>(std::move(x), nullptr);
        return(std::make_pair(iterator(head.get()),true));
    }
    
    node_type* new_node = nullptr;  
    auto tmp = head.get() ;
    
    while (tmp != nullptr)
    {
        new_node = tmp;
        if (x.first < tmp->getValue().first)
        {
            tmp = tmp->getLeft();
        }
        else if (x.first > tmp->getValue().first)
        {
            tmp = tmp->getRight();
        }  
        else
        {
            //if the key is already exist 
             return(std::pair<iterator, bool> (iterator(),false));
        }
        
    }
    tmp = new node_type(std::move(x), new_node);
    if (x.first < new_node->getValue().first){ 
        new_node->setLeft(tmp); 
    }
    else{
        new_node->setRight(tmp);
    }
    return(std::make_pair(iterator(tmp),true)); 
}

template <typename k, typename v, typename c>
typename bst<k,v,c>::iterator bst<k,v,c>::find(const k& x) noexcept{
    
    auto it = iterator(head.get());

    while(it.getCurrent() != nullptr ){
        if(it.getCurrent()->getValue().first < x){
            it.setCurrent(it.getCurrent()->getRight()); 
        }
        else if(it.getCurrent()->getValue().first > x){
            it.setCurrent(it.getCurrent()->getLeft()); 
        }
        else{
            //it.getCurrent()->getValue() = std::make_pair(9,9);
            return(iterator(it));
        }   
    }
    return end();
}

template <typename k, typename v, typename c>
typename bst<k,v,c>::const_iterator bst<k,v,c>::find(const k& x) const{
        
    auto it = const_iterator(head);
    while(it.getCurrent() != nullptr ){
        if(it.getCurrent()->getValue().first < x){
            it.setCurrent(it.getCurrent()->getRight()); 
        }
        else if(it.getCurrent()->getValue().first > x){
            it.setCurrent(it.getCurrent()->getLeft()); 
        }
        else{
            return(const_iterator(it));
        }  
    }
    return end();
}

template <typename k, typename v, typename c>
void bst<k,v,c>::clear() noexcept {
    if(head)
        head.reset();
}

// template<class... Types>
// std::pair<iterator,bool> emplace(Types&&... args); 

template <typename k, typename v, typename c>
template <class... Types>
std::pair<typename bst<k,v,c>::iterator, bool> bst<k,v,c>::emplace(Types&&... args){

    //TODO: need to find a way to increase the performances with respect to an object constructor
    //and an insert because otherwise is pretty much pointless, isn't it?

    return insert(std::make_pair(std::forward<Types>(args)...)); 
    //RIGHT??
}

#endif
