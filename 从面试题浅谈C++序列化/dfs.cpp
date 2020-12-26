#include <iostream>
#include <string>

using namespace std;

struct TreeNode {
    int value;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int val, TreeNode* l = nullptr, TreeNode* r = nullptr) : 
        value(val), left(l), right(r) {}
};

struct Binary {
    int value;
    bool left;
    bool right;
    
    string ToString() {
        return {(const char*)this, sizeof(*this)};
    }
};

TreeNode* Load(Binary*& p) {
    Binary bin = *p;
    TreeNode* node = new TreeNode(p->value);
    if(bin.left) node->left = Load(++p);
    if(bin.right) node->right = Load(++p);
    return node;
}

TreeNode* Load(const string& str) {
    if(str.empty()) return nullptr;

    Binary* p = (Binary*)(&str[0]);
    return Load(p);
}

void Save(TreeNode* root, string& str) {
    if(!root) return;
    str.append(Binary{root->value, !!root->left, !!root->right}.ToString());
    Save(root->left, str);
    Save(root->right, str);
}

bool cmp(TreeNode* t1, TreeNode* t2) {
    if(!!t1 != !!t2) return false;
    if(!t1) return true;
    if(t1->value != t2->value) return false;
    if(!cmp(t1->left, t2->left)) return false;
    return cmp(t1->right, t2->right);
}

int main() {
    TreeNode* root = new TreeNode(1, new TreeNode(2, nullptr, new TreeNode(4)), new TreeNode(3, new TreeNode(5, new TreeNode(7), new TreeNode(8)), new TreeNode(6)));
    string str;
    Save(root, str);
    TreeNode* r2 = Load(str);
    cout<<r2->left->value<<"\n";
    cout<<"cmp:"<<cmp(root, r2)<<"\n";
}
