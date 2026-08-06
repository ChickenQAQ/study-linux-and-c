#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEN 256
#define HIST_MAX 60
#define PI 3.141592653589793

// 历史记录结构体
typedef struct
{
    char expr[MAX_LEN];
    double result;
} History;

History history[HIST_MAX];
int histCount = 0;
double lastResult = 0; // 保存上一次结果，支持复用
int hasLastResult = 0;

// 运算符优先级
int priority(char c)
{
    if (c == '(')
        return 0;
    if (c == '+' || c == '-')
        return 1;
    if (c == '*' || c == '/')
        return 2;
    if (c == '^')
        return 3;
    return -1;
}

// 基础二元运算
double calc(double a, double b, char op)
{
    switch (op)
    {
    case '+':
        return a + b;
    case '-':
        return a - b;
    case '*':
        return a * b;
    case '/':
        if (fabs(b) < 1e-12)
        {
            printf("【错误】除数不能为0！\n");
            return NAN;
        }
        return a / b;
    case '^':
        return pow(a, b);
    default:
        return 0;
    }
}

// 预处理负号：-5 → 0-5，(-3) → (0-3)
void preprocess(char *src, char *dst)
{
    int i = 0, j = 0;
    while (src[i] != '\0')
    {
        if (src[i] == '-')
        {
            if (i == 0 || src[i - 1] == '(' || src[i - 1] == '^' || src[i - 1] == '*' || src[i - 1] == '/')
            {
                dst[j++] = '0';
            }
        }
        dst[j++] = src[i++];
    }
    dst[j] = '\0';
}

// 去除字符串所有空格
void removeSpace(char *s)
{
    int i = 0, j = 0;
    while (s[i] != '\0')
    {
        if (!isspace(s[i]))
            s[j++] = s[i];
        i++;
    }
    s[j] = '\0';
}

// 中缀表达式求值
double expression_calc(char *exp)
{
    char temp[MAX_LEN];
    char exprCopy[MAX_LEN];
    strcpy(exprCopy, exp);
    removeSpace(exprCopy);
    preprocess(exprCopy, temp);

    double num_stack[MAX_LEN];
    char op_stack[MAX_LEN];
    int num_top = -1, op_top = -1;
    int i = 0;

    while (temp[i] != '\0')
    {
        if ((temp[i] >= '0' && temp[i] <= '9') || temp[i] == '.')
        {
            double val = atof(temp + i);
            num_stack[++num_top] = val;
            while ((temp[i] >= '0' && temp[i] <= '9') || temp[i] == '.')
                i++;
        }
        else if (temp[i] == '(')
        {
            op_stack[++op_top] = temp[i];
            i++;
        }
        else if (temp[i] == ')')
        {
            while (op_stack[op_top] != '(')
            {
                double b = num_stack[num_top--];
                double a = num_stack[num_top--];
                char op = op_stack[op_top--];
                num_stack[++num_top] = calc(a, b, op);
                if (isnan(num_stack[num_top]))
                    return NAN;
            }
            op_top--;
            i++;
        }
        else
        {
            while (op_top != -1 && priority(op_stack[op_top]) >= priority(temp[i]))
            {
                double b = num_stack[num_top--];
                double a = num_stack[num_top--];
                char op = op_stack[op_top--];
                num_stack[++num_top] = calc(a, b, op);
                if (isnan(num_stack[num_top]))
                    return NAN;
            }
            op_stack[++op_top] = temp[i];
            i++;
        }
    }

    while (op_top != -1)
    {
        double b = num_stack[num_top--];
        double a = num_stack[num_top--];
        char op = op_stack[op_top--];
        num_stack[++num_top] = calc(a, b, op);
        if (isnan(num_stack[num_top]))
            return NAN;
    }
    return num_stack[0];
}

// 添加历史记录
void addHistory(char *expr, double res)
{
    if (histCount >= HIST_MAX)
    {
        for (int i = 0; i < histCount - 1; i++)
            history[i] = history[i + 1];
        histCount--;
    }
    strcpy(history[histCount].expr, expr);
    history[histCount].result = res;
    histCount++;
}

void showHistory()
{
    if (histCount == 0)
    {
        printf("暂无计算记录！\n");
        return;
    }
    printf("\n===== 计算历史记录 =====\n");
    for (int i = 0; i < histCount; i++)
    {
        printf("%2d: %s = %.6lf\n", i + 1, history[i].expr, history[i].result);
    }
    printf("========================\n");
}

void clearHistory()
{
    histCount = 0;
    printf("? 历史记录已清空！\n");
}

void menu()
{
    printf("\n===================终极多功能计算器===================\n");
    printf("【1】四则混合运算(支持()、负数、^幂运算，例：(1-3)^2+8)\n");
    printf("【2】取模运算 a %% b\n");
    printf("【3】平方运算 x?\n");
    printf("【4】平方根 √x\n");
    printf("【5】三角函数（sin/cos/tan，角度制）\n");
    printf("【6】使用上一次结果继续运算\n");
    printf("【7】查看历史计算记录\n");
    printf("【8】清空历史记录\n");
    printf("【0】退出程序\n");
    printf("=======================================================\n");
    printf("请输入功能选项：");
}

int main()
{
    int choice;
    char expr[MAX_LEN];
    double a, b, res;
    char buf[MAX_LEN];

    while (1)
    {
        menu();
        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n')
                ;
            printf("? 输入非法，请输入数字选项！\n");
            continue;
        }
        getchar();

        switch (choice)
        {
        case 1:
            printf("请输入表达式（^代表幂，可用括号）：");
            fgets(expr, MAX_LEN, stdin);
            expr[strcspn(expr, "\n")] = '\0';
            res = expression_calc(expr);
            if (isnan(res))
            {
                printf("? 表达式计算失败！\n");
            }
            else
            {
                printf("? 结果 = %.6lf\n", res);
                addHistory(expr, res);
                lastResult = res;
                hasLastResult = 1;
            }
            break;
        case 2:
            printf("输入 a b，计算a%%b：");
            scanf("%lf %lf", &a, &b);
            if (fabs(b) < 1e-12)
            {
                printf("? 模数不能为0！\n");
                break;
            }
            res = fmod(a, b);
            sprintf(buf, "%.2lf%%%.2lf", a, b);
            printf("%.2lf %% %.2lf = %.6lf\n", a, b, res);
            addHistory(buf, res);
            lastResult = res;
            hasLastResult = 1;
            break;
        case 3:
            printf("输入数字求平方：");
            scanf("%lf", &a);
            res = a * a;
            sprintf(buf, "%.4lf^2", a);
            printf("%.4lf 的平方 = %.6lf\n", a, res);
            addHistory(buf, res);
            lastResult = res;
            hasLastResult = 1;
            break;
        case 4:
            printf("输入数字开平方根：");
            scanf("%lf", &a);
            if (a < 0)
            {
                printf("? 负数不能开平方根！\n");
                break;
            }
            res = sqrt(a);
            sprintf(buf, "sqrt(%.4lf)", a);
            printf("√%.4lf = %.6lf\n", a, res);
            addHistory(buf, res);
            lastResult = res;
            hasLastResult = 1;
            break;
        case 5:
        {
            char func[10];
            double ang;
            printf("输入函数(sin/cos/tan) + 角度，例：sin 30\n");
            scanf("%s %lf", func, &ang);
            double rad = ang * PI / 180.0;
            if (strcmp(func, "sin") == 0)
                res = sin(rad);
            else if (strcmp(func, "cos") == 0)
                res = cos(rad);
            else if (strcmp(func, "tan") == 0)
                res = tan(rad);
            else
            {
                printf("? 不支持该函数！\n");
                break;
            }
            sprintf(buf, "%s(%.2lf°)", func, ang);
            printf("%s = %.6lf\n", buf, res);
            addHistory(buf, res);
            lastResult = res;
            hasLastResult = 1;
            break;
        }
        case 6:
            if (!hasLastResult)
            {
                printf("?? 暂无上一次计算结果！\n");
                break;
            }
            printf("上一次结果 = %.6lf\n", lastResult);
            printf("请输入接续表达式，可用变量R代替上次结果，例：R*2+5\n");
            fgets(expr, MAX_LEN, stdin);
            expr[strcspn(expr, "\n")] = '\0';
            // 替换 R 为上次结果
            char newExp[MAX_LEN] = {0};
            int k = 0, p = 0; // 移到外面定义！修复编译报错
            for (; expr[k]; k++)
            {
                if (expr[k] == 'R')
                {
                    sprintf(newExp + p, "%.10lf", lastResult);
                    p += strlen(newExp + p);
                }
                else
                    newExp[p++] = expr[k];
            }
            newExp[p] = '\0';
            res = expression_calc(newExp);
            if (isnan(res))
            {
                printf("? 计算失败！\n");
            }
            else
            {
                printf("? 结果 = %.6lf\n", res);
                addHistory(expr, res);
                lastResult = res;
            }
            break;
        case 7:
            showHistory();
            break;
        case 8:
            clearHistory();
            break;
        case 0:
            printf("? 计算器程序退出！\n");
            return 0;
        default:
            printf("? 无效选项，请重新选择！\n");
        }
    }
}