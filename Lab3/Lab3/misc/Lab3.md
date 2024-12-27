> 实验文档、代码处于迭代更新过程中，如有错误或建议，欢迎同学们联系实验助教提PR。

# 实验三 冗余obc检测的移除

## 实验目的

1. 熟悉数据流分析的概念，掌握数据流分析框架设计实现过程
2. 掌握常量传播算法实现
3. 了解LLVM Pass的使用和编写方法



## 实验任务

1. 完善数据流分析框架
2. 设计实现过程内常量传播算法，实现状态数据结构、传递以及合并规则；并基于结果，移除实验二中添加的冗余obc检测代码
3. 在过程内常量传播算法的基础上，添加过程间状态传递，完成过程间常量传播算法



## 实验内容

实验二中，我们完成了`SafeC`代码的中间表示（`LLVM IR`）生成，并对`obc`数组的访问语句插入了检查代码。本实验中，我们将进入中间代码优化阶段（下图中的机器无关代码优化器），实现一个简单的数据流分析算法（常量传播），从而识别并移除实验二中插入的冗余obc检测。

<img src="Lab3-img/compile_process.png" alt="image-20230327181533725" style="zoom: 50%;" />

### 冗余obc检测示例

一个简单的例子如下：

````cpp
void main() {
    int a = 100;
    int b = 4;
    if (b > 5) {
        a = 2;
    } else {
        a = 8;
    }
    int obc c[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    output_var = c[a];
    output();
}
````

在实验二中，我们插入了代码在`output_var = c[a]`这一语句前进行了检查，要求$0 \le a \lt 10$。但观察代码可以发现，该语句前的if-else语句两个分支中对`a`进行了常量赋值，在执行到`output_var = c[a]`时，必定满足$a \in \{2, 8\}$，不可能越界，因此可以移除实验二中在此处添加的检测代码。

### LLVM Pass

`LLVM Pass`是`LLVM`中的一个核心模块，在 [introduction-what-is-a-pass](https://llvm.org/docs/WritingAnLLVMPass.html#introduction-what-is-a-pass) ，可以找到它的定义。一个`pass`可以视为对IR的一次遍历，LLVM的自带中间代码优化算法就是作为多个不同的`pass`实现。

本次实验需要实现一个`pass`，在`pass`中运行我们设计的常量传播算法，完成冗余obc检测代码的去除。

通过`llvm::legacy::PassManager`进行`pass`的管理，添加我们编写实现的`pass`：

```c++
llvm::legacy::PassManager Passes;
ConstValuePass *CVP = new ConstValuePass(xxxxxx);

Passes.add(CVP);
Passes.run(*module.get());
```

**（注：直接实现`ConstValuePass`即可，勿添加其他`LLVM` 已支持的`pass`）**

### 数据流分析

[数据流分析](https://zh.wikipedia.org/wik      llvm::outs() << "result_map: " << result_map << "\n";
      llvm::outs() << "result: " << result << "\n";
i/%E6%95%B0%E6%8D%AE%E6%B5%81%E5%88%86%E6%9E%90)是一种静态分析技术，用于收集程序中在不同点计算的值，在本门课的`代码生成与优化`章节中会对数据流分析相关内容进行介绍。

具体而言，数据流分析按**前向或后向顺序**迭代遍历程序，对每一条指令计算数据状态，在基本块开始或结尾处进行**合流操作**，

常见的数据流分析算法包括如下三种：

+ 到达定值分析：分析程序中到达某点的定值
+ 活跃变量分析：分析在程序中某点有哪些变量活跃
+ 可用表达式分析：分析在程序的某点有哪些表达式可用。

以活跃变量分析为例，我们称变量在某点活跃，意味着在此点开始的路径上，有对该变量的引用，否则，则认为该变量死亡。活跃变量分析算法如下：

````
输入：每个基本块的def和use已经计算的流图
输出：out[B]，即每个基本块B的出口处活跃变量集合

for 每个基本块B do in[B]:=∅;			# 初始化
work_list = 所有出口基本块的集合
while work_list不为空 do begin
	B = work_list.pop()
	out[B] = B的所有后继基本块in的并集			# 合流操作（May）
	in[B] = use[B] U (out[B] - def[B])		# 变量在B内被引用，则在B之前活跃，在B内被重新定义，则在B之前不活跃
	if in[B]和之前不同 do begin:				# in[B]改变，需要重新计算所有前继基本块
		work_list.insert(B的所有前继基本块)
	end if
end while
````

该算法从出口基本块向前进行数据流分析，是一种**前向**数据流方向算法；在进行合流操作时取并集，是一种**may**操作算法（与之对应的是**must**操作算法，合流操作取交集）。

算法的敏感性还有如下三类：

+ 流敏感：考虑语句的执行顺序。
+ 路径敏感：考虑语句的执行路径。
+ 上下文敏感：考虑函数调用的上下文。

上文给出的活跃变量分析算法是一种流敏感、路径不敏感、上下文不敏感的算法。该算法的具体实现可以在实验目录`liveness_example/`中找到。

本实验中，要求首先实现流敏感的常量传播算法，而后添加上下文敏感支持。

### 常量传播

本实验中，需要设计实现常量传播算法，计算obc数组的访存`index`可能取的值，如果`index`可能的取值在合法范围内，则说明此处的obc检测代码可以移除。

[上文](#冗余obc检测示例)中给出的示例代码的常量传播算法执行过程的简单示意如下：

````cpp
void main() {
    int a = 100; 	// a:{100}
    int b = 4;		// a:{100},b:{4}
    if (b > 5) {	// a:{2},b:{4}
        a = 2;		// a:{2},b:{4}
    } else {
        a = 8;		// a:{8},b:{4}
    }
    int obc c[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};	// b:{4},a:{2,8} (新基本块，合流操作，取前两个基本块输出的并集)
    output_var = c[a];			// b:{4},a:{2,8}
    output();					// b:{4},a:{2,8}
}
````

按照上述示意，常量传播算法逐基本块、逐指令**前向**迭代计算每个基本块输入、输出的常量状态，在合流时进行**取并集**操作。

#### 循环迭代与TOP集

数据流算法通常都是迭代算法，如活跃变量算法计算中，当基本块的输入活跃变量发生改变时，需要重新计算其前继基本块的活跃变量。常量传播算法也类似，当基本块的输出常量状态发生变化时，需要重新计算其后继基本块的常量状态。

因此，当遇到一些循环时，可能出现无限迭代的情况，如：

````cpp
void main() {
    int a = 0;
    int obc c[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    while (a < 10000) {
        a = a + 1;
        c[a] = 1;		// a:{0} -> a:{0,1} -> a:{0,1,2} ->...
    }
}
````

在`while`中，每一次迭代计算出`a`的常量状态都和上一次不一样，都比上一次多一个常数，因此算法中会出现无限循环的情况。为了处理这种情况，引入了集合的上限**ksize**，用**TOP集**来表示达到上限的集合。当`ksize`取4时，上述迭代计算过程变为：

````
a:{0} -> a:{0,1} -> a:{0,1,2} -> a:{0,1,2,4} -> a:TOP -> a:TOP
````

从而终止算法，此时，`c[a]=1`语句处的obc检测代码不可移除。

另外，对于取值无法确定的变量，如来自`input()`的值、上下文不敏感分析中的函数参数值，都应该用TOP集表示。

实验框架中部分类中有`ksize`变量的定义，但对于`ksize`和TOP集的支持并未实现，请自行实现。

#### 上下文敏感

对于下述示例：

````cpp
void f() {
    int obc c[2] = {1, 2};
    c[arg0] = 1;
}

void main() {
    arg0 = 0;
    f();
}
````

在上下文不敏感的情况下，无法确定参数`arg0`的取值，计算`f()`中常量状态时，`arg0`为TOP集，无法移除`f()`内的obc数组检测。

如果实现了上下文敏感相关支持，则应当在函数`f()`的入口基本块处传入`arg0:{0}`这一状态，从而确定`c[arg0] = 1`这一语句处的obc数组检测可以移除。

### 冗余检测移除

为了方便助教检测结果，请修改实验二中的obc检测代码插入部分，使得：

+ obc检测代码和报错代码均在单独的基本块中，不应和正常的程序代码在同一个基本块中，以方便移除冗余检测(使用`BasicBlock::eraseFromParent()`接口)；
+ obc检测代码命名为**obc_check_XX**（XX为序号或其他独特的标识符），obc报错代码基本块命名为**obc_err_XX**（XX为序号或其他独特的标识符），以方便助教检查最终结果。

如：

````cpp
bb1:
	正常代码
    jmp obc_check_1
        
obc_check_2:
	...
    br res, obc_err_3, bb4
        
obc_err_3:
	...
    unreachable
        
bb4:
	正常代码
    ...
````

移除时，删除`obc_check_2`和`obc_err_3`基本块后，将`bb1`最后的跳转指令修改为`jmp bb4`即可。

在实验二中添加obc检测代码部分，可以通过`irbuilder.obc_check_record`记录进行`obc`检测的基本块，实验框架中会将该`record`传到 `PASS` 以便最后移除冗余`obc`检测 。

### 具体步骤

本次实验难度较大，需要自行设计常量传播算法，大致按照如下步骤完成：

+ 前向数据流计算框架实现（`Dataflow.h`）
+ 上下文不敏感的常量传播算法实现以及相应的obc检测代码移除(`ConstantPropagator.h`、`ConstantPropagator.cpp`)
+ 上下文敏感的常量传播算法实现

需要实现的内容主要包括：

+ 前向数据流分析框架：`template<class T>
  void compForwardDataflow(llvm::Function *fn, DataflowVisitor<T> *visitor, typename DataflowBBResult<T>::Type *result, const T & initval)`
+ 常量传播的`LLVM PASS`：`class ConstValuePass : public llvm::ModulePass `
+ 常量传播的`Visitor`：`class ConstantPropagatorVisitor : public DataflowVisitor<struct ConstValueState> `
+ 状态存储结构及其相关方法：`struct KSet`

本次实验可以不受实现框架限制：

+ 本实验提供的`ConstantPropagator.*`部分代码和数据结构，为参考实现的一部分，实现时如果认为难以基于此部分代码进行实现或有更为绝妙的想法，可以全部删掉重新实现。

## 实验框架介绍

### 文件结构

````
Lab3
├── cmake
├── CMakeLists.txt
├── include
│   ├── AstBuilder.h
│   ├── AstNode.h
│   ├── AstNode_Visitor.h
│   ├── ConstantPropagator.h
│   ├── Dataflow.h
│   └── SafeCIRBuilder.h
├── main.cpp
├── runtime
│   ├── io.cpp
│   ├── io.h
│   ├── runtime.cpp
│   └── runtime.h
├── SafeCLexer.g4
├── SafeCParser.g4
├── liveness_example
│   ├── CmakeLists.txt
│   ├── Dataflow.h
│   ├── Liveness.h
│   ├── LLVMAssignment.txt
│   ├── test.bc
│   └── test.c
├── src
│   ├── AstBuilder.cpp
│   ├── AstNode_Visitor.cpp
│   ├── ConstantPropagator.cpp
│   ├── Dataflow.cpp
│   └── SafeCIRBuilder.cpp
└── tests
````

+ `liveness_example/`：该目录下给出了活跃变量算法例子，可以通过和本实验相同的方法编译，该目录下的`test.c`和`test.bc`为活跃变量的样例输入，可以通过`./build/liveness_example test.bc`运行。
+ `tests/`：测试集，共16个文件。
+ `DataFlow.h`：数据流算法框架文件，**需要完善**。
+ `ConstantPropagator.h`和`ConstantPropagator.cpp`：本次实验最主要的常量传播算法实现文件，只给出了简略的参考框架，**需要完善**。

##### 测例说明

本次实验共16个测例，其中：

+ 前8个不考察上下文敏感
+ 后8个主要考察上下文敏感的实现

##### Lab1、Lab2文件

实验框架中已给出实验一、实验二的参考解答代码，**建议拷贝自己实现的实验代码覆盖相应文件**。

其中，实验二中的插入obc检测代码部分内容可能需要修改，请参考[冗余检测移除](#冗余检测移除)一节进行修改。

### 编译和运行

##### 编译

本次实验编译过程和上次实验相同，注意首次编译时保持网络畅通，过程不再赘述。

##### 运行

编译完成后，`build`文件夹下会生成`optbuilder`可执行文件，通过`./optirbuilder [path_to_test_case]`即可生成测例IR并调用实现的常量传播pass进行IR优化，请自行检查最终生成的IR是否符合预期。

### 其他

在数据流分析中，可通过名字过滤`input`、`output`、`input_imp`、`output_imp`、`obc_check_error`等函数代码，不对其进行分析。

全局变量的状态初始化后可传入`main`函数`entry`基本块。



## 实验提交要求和评分标准

1. 对于已给出的测例，实验提交时应当无`Segmentation Fault`问题。

2. 最终提交时，实验目录如下，**不要提交build目录**，如果存在对其他文件修改请自行附上。

```
Lab3
├── cmake
├── CMakeLists.txt
├── include
│   ├── AstBuilder.h
│   ├── AstNode.h
│   ├── AstNode_Visitor.h
│   ├── ConstantPropagator.h
│   ├── Dataflow.h
│   └── SafeCIRBuilder.h
├── main.cpp
├── runtime
│   ├── io.cpp
│   ├── io.h
│   ├── runtime.cpp
│   └── runtime.h
├── SafeCLexer.g4
├── SafeCParser.g4
├── src
    ├── AstBuilder.cpp
    ├── AstNode_Visitor.cpp
    ├── ConstantPropagator.cpp
    ├── Dataflow.cpp
    └── SafeCIRBuilder.cpp
```

请在`readme.txt`说明除了`ConstantPropagator.cpp`、`ConstantPropagator.h`、`Dataflow.h`以外的其他修改，如果编译步骤和实验中给定不一样，也请在`readme.txt`中说明。

另请附上**实验报告**一份，篇幅不宜过长，请尽量以`pdf`格式提交，避免其他格式带来不必要的兼容性问题，内容应当包括但不限于：

+ 小组成员；
+ 自行检测时能够通过的测例列表；
+ 如果修改的代码内容除了之前提到的内容，请简要说明你修改的其他部分以及目的；
+ 实现过程以及实现过程中遇到的困难。

同时可以在实验报告中提出对本次实验设计的建议、实验代码和文档勘误等。

3. 评分标准：本次实验共占30分，实验报告占2分，前8个测例分值相同，共占20分，后8个测例分值相同，共占8分，每个测例给分标准如下：
   + 如果**正确移除冗余**的obc检测代码，则该测例得满分；
   + 否则该测例得0分。
4. 实验成绩公布后，如果对得分有异议，请在一周内联系实验助教复核。



## 拓展内容

除了常量传播算法以外，还有一些方法可以用于检测冗余的obc检测代码，感兴趣的同学可以拓展学习相关内容。

### 基于`def-use`链的检测

对于`def-use`链，以如下为例：

```c++
int a = TOP;
int obc b[] = xxx;

b[a] = 2; 
b[a] += 1;
```

此时两个访问`b[a]`的语句处均插入了obc检测代码，但实际上此处两个语句中`a`一定相同（两个语句之间没有对`a`的def，且前一条语句是后一条语句的必经节点），因此，可以移除其中后一个obc检测。

### 后向切片

参考 [SanRazor](https://github.com/SanRazor-repo/SanRazor) 这篇论文，如以下例子

```c++
int a = 1;
int b = a;
int c = 3;
int obc array[] = xxx;
int tmp = 0;

tmp = array[3 + b*c];
tmp = array[a * c + 3];
```

经过对数组的`index`进行后向切片均可以得到相同的`AST`树，从而判断为冗余的检测，因此可以删除其中一个检测，如下面的`array[a*c+3]`



##   参考资料

> https://www.cs.cmu.edu/afs/cs/academic/class/15745-s16/www/lectures/L2-LLVM1.pdf （修改Module）
>
> https://www.cs.cmu.edu/afs/cs/academic/class/15745-s16/www/lectures/L5-Intro-to-Dataflow.pdf （数据流分析）
>
> https://llvm.org/docs/WritingAnLLVMPass.html
>
> https://llvm.org/docs/Passes.html
