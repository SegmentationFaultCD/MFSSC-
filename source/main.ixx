module;
#define null nullptr
#include <ranges>
export module MSFFCPP;
import std;
namespace libtrees {
    template < typename T, typename Keyofvalue >
    class RedBlackTree {
    public:
        // 红黑树节点的定义

        class RBTreeNode {
            friend RedBlackTree;

        public:
            enum class Color {
                RED,
                BLACK
            };

        public:
            RBTreeNode(Keyofvalue key, T* data = nullptr, Color color = Color::RED) noexcept
                // 这里构造节点的时候颜色默认给为红色因为如果给为黑色就有可能会破坏当前红黑树的性质，导致每条路径的黑色节点个数不同
                :
                _left(nullptr), _right(nullptr), _parent(nullptr), _color(color), _key(key), _data(data) {
            }

        private:
            RBTreeNode* _left;       // 节点的左孩子
            RBTreeNode* _right;      // 节点的右孩子
            RBTreeNode* _parent;     // 节点的双亲(红黑树需要旋转，为了实现简单给出该字段)
            Color       _color;      // 节点的颜色
        public:
            Keyofvalue _key;      // 节点的键值
            T* _data;     // 节点的值域
        };
        using Node = RBTreeNode;

        // 红黑树迭代器：
        template < typename _T, typename Ref, typename Ptr >
        struct RBTreeIterator {
            using self = RBTreeIterator< T, Ref, Ptr >;
            // 构造函数就将红黑树的节点指针传入进来：
            RBTreeIterator(Node* node = nullptr) :
                _pnode{ node } {
            }
            // 迭代器解引用：
            Ref operator*() {
                return *_pnode->_data;
            }
            Ptr operator->() {
                return (&operator*());
            }
            // 迭代器加加:前置加加
            self operator++() {
                this->increament();
                return *this;
            }
            self operator++(int) {
                self temp = *this;
                this->increament();
                return temp;
            }
            self operator--() {
                this->decreament();
                return *this;
            }
            self operator--(int) {
                self temp = *this;
                this->decreament();
                return temp;
            }
            // 将当前迭代器指针的值放到后面大的值上
            void increament() {
                // 如果当前迭代器存在右子树的时候我们将_pnode更新到右子树
                if (_pnode->_right) {
                    _pnode = _pnode->_right;
                    // 去右子树中找最小的节点：
                    while (_pnode->_left) {
                        _pnode = _pnode->_left;
                    }
                }
                else {
                    auto parent = _pnode->_parent;
                    while (parent->_right == _pnode) {
                        _pnode = parent;
                        parent = _pnode->_parent;
                    }
                    if (_pnode->_right != parent) {
                        _pnode = parent;
                    }
                }
            }
            void decreament() {
                if (_pnode->_parent->_parent == _pnode && _pnode->_color == Node::Color::RED) {
                    _pnode = _pnode->_right;
                }
                // 如果当前的pnode的左子树存在那么我们就将节点放在左子树
                else if (_pnode->_left) {
                    _pnode = _pnode->_left;
                    while (_pnode->_right) {
                        _pnode = _pnode->_right;
                    }
                }
                else {
                    auto parent = _pnode->_parent;
                    // 这里如果_pnode到了begin的位置就不可以再减了
                    while (_pnode == parent->_left) {
                        _pnode = parent;
                        parent = _pnode->_parent;
                    }
                    _pnode = parent;
                }
            }
            bool operator==(const self& s) const {
                return _pnode == s._pnode;
            }
            bool operator!=(const self& s) const {
                return _pnode != s._pnode;
            }
            Node* _pnode;
        };

    private:
        Node start_node;

    public:
        using iterator = RBTreeIterator< T, T&, T*>;
        using const_iterator = const RBTreeIterator< T, T&, T* >;
        iterator end() {
            return iterator(_head);
        }
        iterator begin() {
            return iterator(_head->_left);
        }
        const_iterator end() const {
            return const_iterator(_head);
        }
        const_iterator begin() const {
            return const_iterator(_head->_left);
        }

    public:
        auto init(void) {
            start_node._key = 0;
            start_node._color = RBTreeNode::Color::RED;
            _head = &start_node;
            _size = 0;
            _head->_left = _head;
            _head->_right = _head;
        }
        explicit RedBlackTree(void) noexcept :
            start_node{ 0 } {
            this->init();
        }

        auto insert(Node* node)     // 插入节点
        {
            
            // 先按照二叉搜索树的方式插入

            auto*& _root = _head->_parent;

            if (!_root) {     // 为空树
                _root = node;
                node->_color = Node::Color::RED;
                _root->_parent = this->_head;
            }
            else {     // 树非空
                auto cur = _root;
                auto parent = this->_head;
                while (cur) {
                    parent = cur;
                    if (node->_key < (cur->_key)) {
                        cur = cur->_left;
                    }
                    else if (node->_key > (cur->_key)) {
                        cur = cur->_right;
                    }
                    else {     // 我们这里不允许插入相同值域的节点
                        return false;
                    }
                }
                cur = node;
                if (node->_key < (parent->_key)) {
                    parent->_left = cur;
                }
                else {
                    parent->_right = cur;
                }
                cur->_parent = parent;
                // 插入成功之后我们调整当前红黑树的节点：
                // 这里我们在插入红黑树中调整的时候只有当第一中情况才继续向上更新节点，那么我们只要考虑第一中情况的终止条件即可
                // 第一中情况中如果parent为红色节点那么当前节点就需要继续向上更新，但是我们将head节点也设为红色那么当我们parent
                // 节点更新到head节点那么当前也就不更新了
                while (parent != _head && parent->_color == Node::Color::RED) {
                    // 插入节点双亲为黑色：
                    if (parent->_color == Node::Color::BLACK) {
                        break;
                    }
                    else {                                                          // 插入节点双亲为红色
                        auto grandparent = parent->_parent;                         // 这里如果双亲的节点是红色那么双亲一定是有双亲节点的
                        if (parent == grandparent->_left) {                       // 第一大类插入节点在红黑树的左子树：
                            auto uncle = grandparent->_right;                       // 当前节点的叔叔节点
                            if (uncle && uncle->_color == Node::Color::RED) {     // 第一种情况：叔叔节点存在而且为红色：
                                parent->_color = Node::Color::BLACK;
                                uncle->_color = Node::Color::BLACK;
                                grandparent->_color = Node::Color::RED;
                                cur = grandparent;
                                parent = cur->_parent;
                            }
                            // 第二三种情况：
                            else {
                                // 因为我们要将第三种情况转化为第二种情况处理所以我们先写第三种情况：cur插在内侧
                                if (cur == parent->_right) {
                                    this->rotate_left(parent);
                                    std::swap(parent, cur);
                                }
                                // 第二种情况：先将parent和grandparent颜色互换然后右旋
                                parent->_color = Node::Color::BLACK;
                                grandparent->_color = Node::Color::RED;
                                this->rotate_right(grandparent);
                            }
                        }
                        else {                                                      // 第二大类插入节点在红黑树的右子树：
                            auto uncle = grandparent->_left;                        // 当前节点的叔叔节点
                            if (uncle && uncle->_color == Node::Color::RED) {     // 第一种情况：叔叔节点存在而且为红色：
                                parent->_color = Node::Color::BLACK;
                                uncle->_color = Node::Color::BLACK;
                                grandparent->_color = Node::Color::RED;
                                cur = grandparent;
                                parent = cur->_parent;
                            }
                            // 第二三种情况：
                            else {
                                // 因为我们要将第三种情况转化为第二种情况处理所以我们先写第三种情况：cur插在内侧
                                if (cur == parent->_left) {
                                    this->rotate_right(parent);
                                    std::swap(parent, cur);
                                }
                                // 第二种情况：先将parent和grandparent颜色互换然后右旋
                                parent->_color = Node::Color::BLACK;
                                grandparent->_color = Node::Color::RED;
                                this->rotate_left(grandparent);
                            }
                        }
                    }
                }
            }
            _root->_color = Node::Color::BLACK;
            this->_head->_left = this->get_mostleftnode();
            this->_head->_right = this->get_mostrightnode();
            return true;
        }

        void destroy(Node*& root) {
            if (root) {
                this->destroy(root->_left);
                this->destroy(root->_right);
                root = nullptr;
            }
        }
        auto clear() {
            this->destroy(this->get_root());
            this->_size = 0;
        }
        ~RedBlackTree() {
            this->destroy(this->get_root());
        }
        // 查找方法
        auto search(Keyofvalue key) -> Node* {
            auto* cur = this->get_root();
            while (cur) {
                if ((cur->_key) < key) {
                    cur = cur->_right;
                }
                else if ((cur->_key) > key) {
                    cur = cur->_left;
                }
                else {
                    return cur;
                }
            }
            return nullptr;
        }
        auto swap(RedBlackTree< T, Keyofvalue > _t) {
            std::swap(_head, _t._head);
        }

        auto size() const {
            return _size;
        }
        auto empty() const {
            return !_size;
        }

        auto inoder() {
            auto* _root = this->get_root();
            this->mid(_root);
        }

        auto is_RBtree() {
            auto* root = this->get_root();
            if (!root) {
                return true;
            }
            // 判断根节点是否是黑色节点：
            if (root->_color == Node::Color::RED) {
                return false;
            }
            // 判断每条路径中黑色节点个数是否相同
            auto black_count = 0ul;
            auto cur = root;
            while (cur) {
                if (cur->_color == Node::Color::BLACK) {
                    black_count++;
                }
                cur = cur->_left;
            }
            auto k = 0;
            return this->is_RBtree(black_count, k, root);
        }
        // 判断红黑树中是否满足性质三（两个红色节点不挨在一起）性质四（每条路径中黑色节点树相同）
        auto is_RBtree(std::size_t black_count, int k, Node* root) {
            if (!root) {
                if (k != black_count) {
                    return false;
                }
                return true;
            }
            if (root->_color == Node::Color::BLACK) {
                k++;
            }
            else {
                if (root->_parent->_color == Node::Color::RED) {
                    return false;
                }
            }
            return this->is_RBtree(black_count, k, root->_left) && this->is_RBtree(black_count, k, root->_right);
        }

    private:
        // 中序遍历：
        auto mid(Node* root) {
            if (root) {
                this->mid(root->_left);
                this->mid(root->_right);
            }
        }
        // 这里因为我们红黑树中没有设置根节点在代码实现的时候不容易理解所以这里我们写一个私有函数返回红黑树的根节点：
        auto*& get_root() {
            return _head->_parent;
        }
        // 获取最左侧节点也就是最小节点：
        auto get_mostleftnode() {
            auto* _root = get_root();
            if (_root) {
                while (_root->_left) {
                    _root = _root->_left;
                }
            }
            return _root;
        }
        // 获取最右侧节点也就是最大节点：
        auto get_mostrightnode() {
            auto* _root = this->get_root();
            if (_root) {
                while (_root->_right) {
                    _root = _root->_right;
                }
            }
            return _root;
        }
        // 左单旋
        auto rotate_left(Node* parent) {

            auto pparent = parent->_parent;
            auto subR = parent->_right;
            auto subRL = subR->_left;
            parent->_right = subRL;
            // 更新subRL的双亲：
            if (subRL) {
                subRL->_parent = parent;
            }
            subR->_left = parent;
            parent->_parent = subR;
            subR->_parent = pparent;
            if (pparent == _head) {
                _head->_parent = subR;
            }
            if (pparent) {
                if (pparent->_left == parent) {
                    pparent->_left = subR;
                }
                else {
                    pparent->_right = subR;
                }
            }
        }
        // 右单旋
        auto rotate_right(Node* parent) {
            auto subL = parent->_left;
            auto subLR = subL->_right;
            auto pparent = parent->_parent;
            parent->_left = subLR;
            // 如果subLR存在那么将其父节点更新
            if (subLR) {
                subLR->_parent = parent;
            }
            // 将parent右旋下来：
            subL->_right = parent;
            // parent旋下来就要更新parent的父节点
            parent->_parent = subL;
            // 此时subL就要更新父节点
            subL->_parent = pparent;
            if (pparent == _head) {
                _head->_parent = subL;
            }
            if (pparent) {
                if (parent == pparent->_right) {
                    pparent->_right = subL;
                }
                else {
                    pparent->_left = subL;
                }
            }
        }

    private:
        std::uint64_t _size;
        Node* _head;
    };
}     // namespace libtrees

namespace cpp::base {

    /**
     *\class rbtree
     *\brief 红黑树节点
     */
    template <typename TKey, typename TVal>
    class rbtree {
    public:
        enum {
            red,  /**< 红色节点 */
            black /**< 黑色节点 */
        };

        int32_t     color = black; /**< 节点颜色，取值为 red 或 black */
        TKey    key;           /**< 节点键值 */
        TVal    val;           /**< 节点值 */
        rbtree* left = nullptr; /**< 左子节点指针 */
        rbtree* right = nullptr; /**< 右子节点指针 */
        rbtree* parent = nullptr; /**< 父节点指针 */

        explicit rbtree(const TKey& key) : key(key) {}
        rbtree(const TKey& key, const TVal& val) : key(key), val(val) {}
        explicit rbtree(TKey&& key) : key(std::move(key)) {}
        rbtree(TKey&& key, const TVal& val) : key(std::move(key)), val(val) {}
        rbtree(const TKey& key, TVal&& val) : key(key), val(std::move(val)) {}
        rbtree(TKey&& key, TVal&& val) : key(std::move(key)), val(std::move(val)) {}
        ~rbtree() {
            if (left) delete left;
            if (right) delete right;
        }

        auto get(const TKey& query) -> TVal* {
            if (query < key) return left ? left->get(query) : nullptr;
            if (query > key) return right ? right->get(query) : nullptr;
            return &val;
        }

        auto get(const TKey& query) const -> const TVal* {
            if (query < key) return left ? left->get(query) : nullptr;
            if (query > key) return right ? right->get(query) : nullptr;
            return &val;
        }

        auto getnode(const TKey& query) -> rbtree* {
            if (query < key) return left ? left->getnode(query) : nullptr;
            if (query > key) return right ? right->getnode(query) : nullptr;
            return this;
        }

        auto getnode(const TKey& query) const -> const rbtree* {
            if (query < key) return left ? left->getnode(query) : nullptr;
            if (query > key) return right ? right->getnode(query) : nullptr;
            return this;
        }

        auto getref(const TKey& query, rbtree*& r) -> TVal& {
            rbtree* result;
            r = insert_if_notexist(this, query, result);
            return result->val;
        }

        auto search(const TVal& query) -> TKey* {
            if (query == val) return &key;
            if (left)
                if (const auto ret = left->search(query); ret) return ret;
            if (right)
                if (const auto ret = right->search(query); ret) return ret;
            return nullptr;
        }

        auto search(const TVal& query) const -> const TKey* {
            if (query == val) return &key;
            if (left)
                if (const auto ret = left->search(query); ret) return ret;
            if (right)
                if (const auto ret = right->search(query); ret) return ret;
            return nullptr;
        }

        auto searchnode(const TVal& query) -> rbtree* {
            if (query == val) return this;
            if (left)
                if (const auto ret = left->searchnode(query); ret) return ret;
            if (right)
                if (const auto ret = right->searchnode(query); ret) return ret;
            return nullptr;
        }

        auto searchnode(const TVal& query) const -> const rbtree* {
            if (query == val) return this;
            if (left)
                if (const auto ret = left->searchnode(query); ret) return ret;
            if (right)
                if (const auto ret = right->searchnode(query); ret) return ret;
            return nullptr;
        }

        auto min() -> TVal* {
            return left ? left->min() : &val;
        }

        auto min() const -> const TVal* {
            return left ? left->min() : &val;
        }

        auto minnode() -> rbtree* {
            return left ? left->minnode() : this;
        }

        auto minnode() const -> const rbtree* {
            return left ? left->minnode() : this;
        }

        auto max() -> TVal* {
            return right ? right->max() : &val;
        }

        auto max() const -> const TVal* {
            return right ? right->max() : &val;
        }

        auto maxnode() -> rbtree* {
            return right ? right->maxnode() : this;
        }

        auto maxnode() const -> const rbtree* {
            return right ? right->maxnode() : this;
        }

        auto insert(const TKey& key, const TVal& value) -> rbtree* {
            return insert(this, key, value);
        }

        auto ins(const TKey& key, const TVal& value) -> rbtree* {
            return insert(this, key, value);
        }

        auto del(const TKey& key) -> rbtree* {
            return delete_(this, key);
        }

        auto delete_(const TKey& key) -> rbtree* {
            return delete_(this, key);
        }

        void print_inorder(int deepth = 0) {
            if (deepth == 0) std::printf("In-order traversal of the Red-Black Tree: \n");
            if (left) left->print_inorder(deepth + 1);
            for (int i = 0; i < deepth; i++)
                printf("| ");
            printf("%d %p\n", key, val);
            if (right) right->print_inorder(deepth + 1);
        }

        void print_preorder(int deepth = 0) {
            if (deepth == 0) printf("Pre-order traversal of the Red-Black Tree: \n");
            for (int i = 0; i < deepth; i++)
                printf("| ");
            printf("%d %p\n", key, val);
            if (left) left->print_preorder(deepth + 1);
            if (right) right->print_preorder(deepth + 1);
        }

        void print_postorder(int deepth = 0) {
            if (deepth == 0) printf("Post-order traversal of the Red-Black Tree: \n");
            if (left) left->print_postorder(deepth + 1);
            if (right) right->print_postorder(deepth + 1);
            for (int i = 0; i < deepth; i++)
                printf("| ");
            printf("%d %p\n", key, val);
        }

    private:
        static auto left_rotate(rbtree* r, rbtree* x) -> rbtree* {
            auto y = x->right;
            x->right = y->left;

            if (y->left != null) y->left->parent = x;

            y->parent = x->parent;

            if (x->parent == null)
                r = y;
            else if (x == x->parent->left)
                x->parent->left = y;
            else
                x->parent->right = y;

            y->left = x;
            x->parent = y;

            return r;
        }

        static auto right_rotate(rbtree* r, rbtree* y) -> rbtree* {
            auto x = y->left;
            y->left = x->right;

            if (x->right != null) x->right->parent = y;

            x->parent = y->parent;

            if (y->parent == null)
                r = x;
            else if (y == y->parent->left)
                y->parent->left = x;
            else
                y->parent->right = x;

            x->right = y;
            y->parent = x;

            return r;
        }

        static auto transplant(rbtree* r, rbtree* u, rbtree* v) -> rbtree* {
            if (u->parent == null)
                r = v;
            else if (u == u->parent->left)
                u->parent->left = v;
            else
                u->parent->right = v;

            if (v != null) v->parent = u->parent;

            return r;
        }

        static auto insert_fixup(rbtree* r, rbtree* z) -> rbtree* {
            while (z != r && z->parent->color == red) {
                if (z->parent == z->parent->parent->left) {
                    auto y = z->parent->parent->right;
                    if (y != null && y->color == red) {
                        z->parent->color = black;
                        y->color = black;
                        z->parent->parent->color = red;
                        z = z->parent->parent;
                    }
                    else {
                        if (z == z->parent->right) {
                            z = z->parent;
                            r = left_rotate(r, z);
                        }
                        z->parent->color = black;
                        z->parent->parent->color = red;
                        r = right_rotate(r, z->parent->parent);
                    }
                }
                else {
                    auto y = z->parent->parent->left;
                    if (y != null && y->color == red) {
                        z->parent->color = black;
                        y->color = black;
                        z->parent->parent->color = red;
                        z = z->parent->parent;
                    }
                    else {
                        if (z == z->parent->left) {
                            z = z->parent;
                            r = right_rotate(r, z);
                        }
                        z->parent->color = black;
                        z->parent->parent->color = red;
                        r = left_rotate(r, z->parent->parent);
                    }
                }
            }

            r->color = black;
            return r;
        }

        static auto insert(rbtree* r, const TKey& key, const TVal& val) -> rbtree* {
            auto z = new rbtree(key, val);
            z->color = red;

            rbtree* y = null;
            rbtree* x = r;

            while (x != null) {
                y = x;
                if (key < x->key)
                    x = x->left;
                else
                    x = x->right;
            }

            z->parent = y;
            if (y == null)
                r = z;
            else if (key < y->key)
                y->left = z;
            else
                y->right = z;

            return insert_fixup(r, z);
        }

        static auto insert_if_notexist(rbtree* r, const TKey& key, rbtree*& z) -> rbtree* {
            rbtree* y = null;
            rbtree* x = r;

            while (x != null) {
                y = x;
                if (key < x->key)
                    x = x->left;
                else if (key > x->key)
                    x = x->right;
                else {
                    z = x;
                    return r;
                }
            }

            z = new rbtree(key);
            z->color = red;
            z->parent = y;
            if (y == null)
                r = z;
            else if (key < y->key)
                y->left = z;
            else
                y->right = z;

            return insert_fixup(r, z);
        }

        static auto delete_fixup(rbtree* r, rbtree* x, rbtree* x_p) -> rbtree* {
            while (x != r && (x == null || x->color == black)) {
                if (x == x_p->left) {
                    auto w = x_p->right;
                    if (w->color == red) {
                        w->color = black;
                        x_p->color = red;
                        r = left_rotate(r, x_p);
                        w = x_p->right;
                    }
                    if ((w->left == null || w->left->color == black) &&
                        (w->right == null || w->right->color == black)) {
                        w->color = red;
                        x = x_p;
                        x_p = x_p->parent;
                    }
                    else {
                        if (w->right == null || w->right->color == black) {
                            if (w->left != null) w->left->color = black;
                            w->color = red;
                            r = right_rotate(r, w);
                            w = x_p->right;
                        }
                        w->color = x_p->color;
                        x_p->color = black;
                        if (w->right != null) w->right->color = black;
                        r = left_rotate(r, x_p);
                        x = r;
                    }
                }
                else {
                    auto w = x_p->left;
                    if (w->color == red) {
                        w->color = black;
                        x_p->color = red;
                        r = right_rotate(r, x_p);
                        w = x_p->left;
                    }
                    if ((w->right == null || w->right->color == black) &&
                        (w->left == null || w->left->color == black)) {
                        w->color = red;
                        x = x_p;
                        x_p = x_p->parent;
                    }
                    else {
                        if (w->left == null || w->left->color == black) {
                            if (w->right != null) w->right->color = black;
                            w->color = red;
                            r = left_rotate(r, w);
                            w = x_p->left;
                        }
                        w->color = x_p->color;
                        x_p->color = black;
                        if (w->left != null) w->left->color = black;
                        r = right_rotate(r, x_p);
                        x = r;
                    }
                }
            }

            if (x != null) x->color = black;
            return r;
        }

        static auto delete_(rbtree* r, const TKey& key) -> rbtree* {
            if (r == null) return null;
            rbtree* z = r->getnode(key);
            if (z == null) return r;

            rbtree* x;
            rbtree* x_p;
            int32_t o_color = z->color;

            if (z->left == null) {
                x = z->right;
                x_p = z->parent;
                r = transplant(r, z, z->right);
            }
            else if (z->right == null) {
                x = z->left;
                x_p = z->parent;
                r = transplant(r, z, z->left);
            }
            else {
                auto y = z->right->minnode();
                o_color = y->color;
                x = y->right;
                x_p = y;
                if (y->parent == z) {
                    if (x != null) x->parent = y;
                }
                else {
                    r = transplant(r, y, y->right);
                    y->right = z->right;
                    if (y->right != null) y->right->parent = y;
                }
                r = transplant(r, z, y);
                y->left = z->left;
                y->left->parent = y;
                y->color = z->color;
            }

            z->left = z->right = null;
            delete z;

            if (o_color == black) r = delete_fixup(r, x, x_p);

            return r;
        }
    };

} // namespace cpp::base

namespace cpp {

    template <typename TKey, typename TVal>
    class RBTree {
    private:
        base::rbtree<TKey, TVal>* root = null;

    public:
        RBTree() = default;
        ~RBTree() {
            delete root;
        }

        auto has(const TKey& query) const -> bool {
            return root ? root->getnode(query) != null : false;
        }

        auto get(const TKey& query) -> TVal& {
            return root ? root->getref(query, root) : (root = new base::rbtree<TKey, TVal>(query))->val;
        }

        auto getnode(const TKey& query) const -> const base::rbtree<TKey, TVal>* {
            return root ? root->getnode(query) : null;
        }

        auto search(const TVal& query) const -> const TKey* {
            return root ? root->search(query) : null;
        }

        auto searchnode(const TVal& query) const -> const base::rbtree<TKey, TVal>* {
            return root ? root->searchnode(query) : null;
        }

        auto min() -> TVal* {
            return root ? root->min() : null;
        }

        auto min() const -> const TVal* {
            return root ? root->min() : null;
        }

        auto minnode() const -> const base::rbtree<TKey, TVal>* {
            return root ? root->minnode() : null;
        }

        auto max() -> TVal* {
            return root ? root->max() : null;
        }

        auto max() const -> const TVal* {
            return root ? root->max() : null;
        }

        auto maxnode() const -> const base::rbtree<TKey, TVal>* {
            return root ? root->maxnode() : null;
        }

        auto insert(const TKey& key, const TVal& val) -> void {
            if (root)
                root = root->insert(key, val);
            else
                root = new base::rbtree<TKey, TVal>(key, val);
        }

        auto ins(const TKey& key, const TVal& val) -> void {
            if (root)
                root = root->insert(key, val);
            else
                root = new base::rbtree<TKey, TVal>(key, val);
        }

        auto del(const TKey& key) -> void {
            if (root) root = root->delete_(key);
        }

        auto delete_(const TKey& key) -> void {
            if (root) root = root->delete_(key);
        }
    };

}     // namespace cpp

export auto main(void) -> int {
    using namespace libtrees;
    RedBlackTree< int, int > a{ };
    cpp::RBTree< int, int >  b{ };


    std::chrono::high_resolution_clock s;
    auto now = s.now();
    for (auto i : std::views::iota(1, 500000)) {
        a.insert(new RedBlackTree< int, int >::RBTreeNode(i, nullptr));
    }
    auto than = s.now();
    std::println("{}", (than - now).count());
    now = s.now();
    for (auto i : std::views::iota(1, 500000)) {
        b.insert(i, i);
    }
    than = s.now();
    std::println("{}", (than - now).count());

    // for ( auto &it : a ) {
    //     std::println( "{}", (int)it );
    // }
}
//import MSFFCPP.Parser;
//
//std::vector<std::string> files;
//std::map<std::string, std::string> texts;
//export auto main(int argc, char** argv, char** envp) -> int {
//	// 从命令行读取文件
//	for (auto i = 0; i < argc - 1; ++i) {
//		files.push_back(argv[i + 1]);
//	}
//	std::filesystem::path building_directory = "./build";
//	if (!std::filesystem::exists(building_directory)) {
//		// 不存在
//		std::filesystem::create_directory(building_directory);
//	}
//	std::ifstream ifs{};
//	for (auto& file : files) {
//		if (!std::filesystem::exists(file)) {
//			std::println("The provided file '{}' does not exist.", file);
//			return 0;
//		}
//		ifs.open(file);
//		char ch{};
//		std::uint64_t line{};
//		while (ifs >> ch) {
//			texts[file].push_back(ch);
//		}
//		ifs.close();
//		ifs.clear();
//	}
//
//	
//}