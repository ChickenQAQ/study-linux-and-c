#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define TREENODE node
typedef struct node
{
    int data;//用户数据
    struct node *lchild;//左子树指针
    struct node *rchild;//右子树指针
} node;
#include "drawtree.h"
node * createnode(int data)
{
    node *new = malloc(sizeof(node));
    new->data = data;
    new->lchild = NULL;
    new->rchild = NULL;
    return new;
}
node * insert_tree(node *root,int data)
{
    if(root == NULL)
    {
       
        return createnode(data) ;
    }
    else if(data < root->data)
    {
        root->lchild = insert_tree(root->lchild,data);
    }
    else
    {
        root->rchild = insert_tree(root->rchild,data);
    }
    return root;
}
void free_tree(node *root)
{
    if(root == NULL)
    {
        return;
    }
    free_tree(root->lchild);
    free_tree(root->rchild);
    free(root);
    return;
}
void show_tree(node * tree)
{
    if(tree == NULL)
    {
        return;
    }
    show_tree(tree->lchild);
    printf("%d\t",tree->data);
    show_tree(tree->rchild);
}
node * delete_tree(node *root,int del)
{
    if(root == NULL)
    {
        return root;
    }
    else if(del<root->data)
    {
        root->lchild = delete_tree(root->lchild,del);
    }
    else if(del>root->data)
    {
        root->rchild = delete_tree(root->rchild,del);
    }
    else
    {
        if(root->lchild == NULL)
        {
            return root->rchild;
        }
        else if(root->rchild == NULL)
        {
            return root->lchild;
        }
        else
        {
            node *temp = root->rchild;
            while(temp->lchild !=NULL)
            {
                temp = temp->lchild;
            }
            root->data = temp->data;
            root->rchild = delete_tree(root->rchild,root->data);
        }
    }
    return root;
}
int main()
{
    
    node *root = NULL;
    int data[] = {4, 3, 8, 1, 5, 9, 12};
    for(int i = 0;i<7;i++)
    {
        root = insert_tree(root,data[i]);
    }
    show_tree(root);
    printf("\n");
    return 0;
}