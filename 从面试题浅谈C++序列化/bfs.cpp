#include <string>
#include <iostream>
#include <sstream>
#include <queue>

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
};

ostream& operator<<(ostream& os, const Binary& bin) {
    os<<" "<<bin.value<<" "<<bin.left<<" "<<bin.right;
    return os;
}

istream& operator>>(istream& is, Binary& bin) {
    is>>bin.value>>bin.left>>bin.right;
    return is;
}

ostream& operator<<(ostream& os, TreeNode* root) {
    if(!root) return os;
    queue<TreeNode*> q;
    q.push(root);
    while(!q.empty()) {
        TreeNode* node = q.front();
        q.pop();
        os<<Binary{node->value, !!node->left, !!node->right};
        if(node->left) q.push(node->left);
        if(node->right) q.push(node->right);
    }
    return os;
}

istream& operator>>(istream& is, TreeNode*& root) {
    // 先读取父节点
    Binary bin;
    if(!(is>>bin)) return is;
    root = new TreeNode(bin.value);
    // 将父节点的字节指针放入队列中
    queue<TreeNode**> q;
    if(bin.left) q.push(&root->left);
    if(bin.right) q.push(&root->right);
    // 从队列中取出子节点的指针，逐个读取每个节点的value
    while(!q.empty()) {
        TreeNode** node = q.front();
        q.pop();
        is>>bin;
        *node = new TreeNode(bin.value);
        if(bin.left) q.push(&(*node)->left);
        if(bin.right) q.push(&(*node)->right);
    }
    return is;
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
    stringstream ss;
    ss<<root;
    cout<<ss.str()<<"\n";
    TreeNode* r2;
    ss>>r2;
    cout<<r2->left->value<<"\n";
    cout<<"cmp:"<<cmp(root, r2)<<"\n";
}
