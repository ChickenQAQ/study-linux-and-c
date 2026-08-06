    #include <stdio.h>
    #include <stdlib.h>
    typedef struct node
    {
        int data;

        struct node *next;

    } node;
    node *initlist()
    {
        node *head = malloc(sizeof(node));
        if (head != NULL)
            head->next = NULL;
        return head;
    }
    void insertlist(node *head, node *n)
    {

        n->next = head->next;
        head->next = n;
    }
    node *newnode(int n)
    {
        node *new = malloc(sizeof(node));

        new->data = n;
        new->next = NULL;
        return new;
    }
    void showlist(node * list)
    {
        for (node *temp = list->next; temp != NULL; temp = temp->next)
            printf("%d\t", temp->data);
    }
    node *  reverselist(node * list)
    {
        node* prev=NULL;
        node* cur =list->next;
        node* temp;
        while(cur!=NULL)
        {
            temp = cur->next;//记录下一个节点
            cur->next =prev;//将指针逆转
            prev = cur;//将prev后移
            cur =temp;//cur后移
        }
        node* head =malloc(sizeof(node));
        head->next=prev;
        return head;
    }

    int main(int argc, char const *argv[])
    {
        // 初始化链表
        node *list = initlist();
        if (list->next == NULL)
            printf("初始化成功\n");
        // 从头节点插入数据
        for (int i = 1; i <= 5; i++)
        {
            // 创建新节点,插入新节点
            insertlist(list, newnode(i));
        }
        //反转
        showlist(list);
        printf("\n");
        node* p= reverselist(list);
        showlist(p);

        return 0;
    }
