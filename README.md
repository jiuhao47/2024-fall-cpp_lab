# 2024-fall-cpp_lab

- 2024-11-10-11:37 jiuhao加入协作开发
- 2024-11-10-11:43 Cilrag加入协作开发

## Lab1

### Lab1-1

- 2024-11-10
    - SafeCLexer.g4:
        - Done by jiuhao at 2024-11-10-14:23
        - Optimized by Cilrag at 2024-11-11 18:50
        - 无修改
    - SafeCParser.g4: 
        - Done by jiuhao at 2024-11-10-14:23
        - Optimized by Cilrag at 2024-11-11 18:50
        - 将符号名称改为直接引用字符串；条件表达式cond中明确列举出了支持的运算符号；表达式部分expr不再使用拆分操作符的方式，直接使用符号
        - 证实报错，无需修改
### Lab1-2

- 2024-11-14
    - 基本完成了AstBuilder.cpp的框架编写，但有Bug: Done by jiuhao && Cilrag
    - 需要Cilrag重构visitexp函数，这个函数的分支判断逻辑有大问题（迭代过程与顺序有问题，注意运用

    ```c
    antlrcpp::Any AstBuilder::visitExp(SafeCParser::ExpContext *ctx) {
    ```

    中的ExpContext结构体中的现有判断逻辑。

- 2024-11-21
    - 修改了visitExp函数，新增了对函数调用的识别与处理，完善了类型判断逻辑。着重修改了一元运算，未测试。Done by Cilrag
- 2024-11-23 
    - Lab1-2所有测试点通过。Done by jiuhao
- 2024-11-25
    - TODO: 本地运行部署。 need to be done by Cilrag
- 2024-11-29
    - TODO: 实验报告部分撰写，见doc/section/s1.tex
- 2024-12-16
    - 实验报告Lab1部分撰写完成 Done by Cilrag
    - 修改意见第一版，见doc/section/s1.tex Done by jiuhao
