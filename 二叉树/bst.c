#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct node
{
    int n;
    struct node *left;
    struct node *right;
};

typedef struct seqQueue
{
    struct node * *data;
    int capacity;
    int front;
    int rear;
}queue;

queue * initQueue(int size)
{
    queue * new = malloc(sizeof(queue));
    if(new == NULL)
        return NULL;

    new->data = malloc(size * sizeof(struct node *));
    if(new->data == NULL)
    {
        free(new);
        return NULL;
    }
    new->front = new->rear = 0;
    new->capacity = size;
    return new;
}
bool isFull(queue * que)
{
    return (que->rear+1)%que->capacity==que->front;
}
bool enQueue(queue * que,struct node *data)
{
    if(isFull(que))
        return false;
    que->data[que->rear]=data;
    que->rear=(que->rear+1)%que->capacity;
    return true;
}
bool isEmpty(queue * que)
{
    return que->rear == que->front;
}
bool outQueue(queue * que,struct node **n)
{
    if(isEmpty(que))
    return false;
    *n = que->data[que->front];
    que->front =(que->front+1)%que->capacity;
    return true;
}
struct node *bst_insert(struct node *root, int n)
{
    if(root == NULL)
    {
        struct node *new_node = (struct node *)malloc(sizeof(struct node));
        if(new_node != NULL)
        {
          new_node->n = n;
          new_node->left = NULL;
          new_node->right = NULL;
        }
        return new_node;
    }

    if(n < root->n)
        root->left = bst_insert(root->left, n);
    else
        root->right = bst_insert(root->right, n);

    return root;
}

// 前序遍历
void pre_travel(struct node *root)
{
    if(root == NULL)
        return;

    // 1. 访问根节点
    printf("%d ", root->n);

    // 2. 访问左子树
    pre_travel(root->left);

    // 3. 访问右子树
    pre_travel(root->right);
}
void in_travel(struct node *root)
{
    if(root == NULL)
        return;

    // 1. 访问左子树
    in_travel(root->left);

    // 2. 访问根节点
    printf("%d ", root->n);

    // 3. 访问右子树
    in_travel(root->right);
}
void post_travel (struct node *root)
{
    if(root == NULL)
        return;
    post_travel(root->left);
    post_travel(root->right);
    printf("%d ", root->n);
}
void insert_travel(struct node *root)
{
    queue * n;
    struct node * temp;

    if(root == NULL)
        return;
    n = initQueue(50);
    if(n== NULL)
    {
        printf("初始化失败\n");
        return;
    }
    enQueue(n,root);
    while(!isEmpty(n))
    {
        outQueue(n,&temp);
        printf("%d ",temp->n);
        if(temp->left!=NULL)
            enQueue(n,temp->left);
        if(temp->right!=NULL)
            enQueue(n,temp->right);
    }
    free(n->data);
    free(n);    
}
struct node * del_data(struct node * root,int data)
{
    struct node * max;
    struct node * min;

    if(root == NULL)
        return NULL;
    if(data<root->n)
        root->left = del_data(root->left,data);
    else if(data>root->n)
        root->right = del_data(root->right,data);   
    else
    {
        if(root->left!=NULL)
        {
            max = root->left;
            for(;max->right!=NULL;max=max->right);
            root->n = max->n;
            root ->left = del_data(root->left,max->n);
        }
        else if(root->right!=NULL)
        {
            min = root->right;
            for(;min->left!=NULL;min=min->left);
            root->n = min->n;
            root-> right = del_data(root->right,min->n);
        }
        else{
            free(root);
            return NULL;
        }
    }
    return root;
}
int main(void)
{
    struct node *root = NULL;
    int n, m;

    printf("请输入二叉树的数值\n");
    while(1)
    {
        scanf("%d", &n);
        if(n == 0)
            break;

        root = bst_insert(root, n);
    }

    // 遍历二叉树
    
    // 1. 前序遍历
    pre_travel(root); printf("\n");

    // 2. 中序遍历
    in_travel(root); printf("\n");

    // 3. 后序遍历
    post_travel(root); printf("\n");
    // 4. 层序遍历
    insert_travel(root);printf("\n");
    printf("请输入你要删除的数字\n");
    scanf("%d", &m);
    root = del_data(root, m);
    pre_travel(root);printf("\n");
    return 0;
    
}
