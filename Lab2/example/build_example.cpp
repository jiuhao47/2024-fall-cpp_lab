#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include <memory>

using namespace llvm;

int main() {
    LLVMContext context;
    IRBuilder<> builder(context);
    // 新建一个LLVM Module
    auto module = new Module("example", context);

    /*
        创建全局变量:
        @res = dso_local global [5 x i32] [i32 0, i32 1, i32 2, i32 3, i32 4], align 16
        @idx = dso_local global i32 0, align 4
    */
   // 创建全局变量res的初始化数组
    std::vector<llvm::Constant *> res_init_values;
    for (int i = 0; i < 5; i++) {
        res_init_values.push_back(builder.getInt32(i));
    }
    llvm::ArrayRef<llvm::Constant *> res_arrayref(res_init_values);
    auto res_init_val_array = llvm::ConstantArray::get(llvm::ArrayType::get(llvm::Type::getInt32Ty(context), 5), res_arrayref);
    // 创建全局变量res
    // 参数：模块，类型，是否常量，链接类型，初始化值，变量名
    auto res = new GlobalVariable(*module, builder.getInt32Ty(), false, GlobalValue::LinkageTypes::ExternalLinkage, res_init_val_array, "res");
    // 创建全局变量idx
    auto idx = new GlobalVariable(*module, builder.getInt32Ty(), false, GlobalValue::LinkageTypes::ExternalLinkage, ConstantInt::get(builder.getInt32Ty(), 0), "idx");

    /*
        创建函数sub定义:
        define dso_local i32 @sub(i32 noundef %0, i32 noundef %1) #0
    */
    auto sub_function = Function::Create(
        FunctionType::get(
            Type::getInt32Ty(context), {Type::getInt32Ty(context), Type::getInt32Ty(context)}, false
            ), 
        GlobalValue::LinkageTypes::ExternalLinkage, "sub",
        *module);
    
    /*
        创建sub函数的入口基本块、if分支为真的基本块、if分支为假的基本块、函数返回基本块
    */
    auto sub_entry = llvm::BasicBlock::Create(context, "entry", sub_function);
    auto sub_if_true = llvm::BasicBlock::Create(context, "if_true", sub_function);
    auto sub_if_false = llvm::BasicBlock::Create(context, "if_false", sub_function);
    auto sub_return = llvm::BasicBlock::Create(context, "return", sub_function);
    // 设置后续指令插入点为entry基本块
    builder.SetInsertPoint(sub_entry); 

    /*
        分配局部变量空间：
        %3 = alloca i32, align 4
        %4 = alloca i32, align 4
        %5 = alloca i32, align 4
    */
    // 参数：类型，数组长度，变量名
    auto var_3 = builder.CreateAlloca(Type::getInt32Ty(context), NULL, "");
    auto var_4 = builder.CreateAlloca(Type::getInt32Ty(context), NULL, "");
    auto var_5 = builder.CreateAlloca(Type::getInt32Ty(context), NULL, "");
    /*
        存储函数参数：
        store i32 %0, i32* %4, align 4
        store i32 %1, i32* %5, align 4
    */
    builder.CreateStore(sub_function->getArg(0), var_4);
    builder.CreateStore(sub_function->getArg(1), var_5);

    /*
        加载参数并做大小比较：
        %6 = load i32, i32* %4, align 4
        %7 = load i32, i32* %5, align 4
        %8 = icmp sgt i32 %6, %7
    */
    auto var_6 = builder.CreateLoad(Type::getInt32Ty(context), var_4);
    auto var_7 = builder.CreateLoad(Type::getInt32Ty(context), var_5);
    auto var_8 = builder.CreateICmpSGT(var_6, var_7);

    /*
        if分支指令（基本块名和clang生成的结果不同）：
        br i1 %8, label %if_true, label %if_false
    */
    builder.CreateCondBr(var_8, sub_if_true, sub_if_false);

    /*
        entry基本块创建完毕，创建if分支为真的基本块：
        9:                                                ; preds = %2
        %10 = load i32, i32* %4, align 4
        %11 = load i32, i32* %5, align 4
        %12 = sub nsw i32 %10, %11
        store i32 %12, i32* %3, align 4
        br label %17
    */
    builder.SetInsertPoint(sub_if_true); //后续指令插入到sub_if_true基本块
    auto var_10 = builder.CreateLoad(Type::getInt32Ty(context), var_4);
    auto var_11 = builder.CreateLoad(Type::getInt32Ty(context), var_5);
    auto var_12 = builder.CreateSub(var_10, var_11);
    builder.CreateStore(var_12, var_3);
    builder.CreateBr(sub_return);   //无条件分支指令

    /*
        创建if分支为假的基本块：
        13:                                               ; preds = %2
        %14 = load i32, i32* %5, align 4
        %15 = load i32, i32* %4, align 4
        %16 = sub nsw i32 %14, %15
        store i32 %16, i32* %3, align 4
        br label %17

    */
    builder.SetInsertPoint(sub_if_false); //后续指令插入到sub_if_false基本块
    auto var_14 = builder.CreateLoad(Type::getInt32Ty(context), var_5);
    auto var_15 = builder.CreateLoad(Type::getInt32Ty(context), var_4);
    auto var_16 = builder.CreateSub(var_14, var_15);
    builder.CreateStore(var_16, var_3);
    builder.CreateBr(sub_return);   //无条件分支指令

    /*
        创建函数返回基本块：
        17:                                               ; preds = %13, %9
        %18 = load i32, i32* %3, align 4
        ret i32 %18
    */
    builder.SetInsertPoint(sub_return); //后续指令插入到sub_return基本块
    auto var_18 = builder.CreateLoad(Type::getInt32Ty(context), var_3);
    builder.CreateRet(var_18);

    /*
        创建main函数：
        define dso_local i32 @main() #0 {
            %1 = call i32 @sub(i32 noundef 2, i32 noundef 5)
            %2 = load i32, i32* @idx, align 4
            %3 = add nsw i32 %2, 1
            store i32 %3, i32* @idx, align 4
            %4 = sext i32 %2 to i64
            %5 = getelementptr inbounds [5 x i32], [5 x i32]* @res, i64 0, i64 %4
            store i32 %1, i32* %5, align 4
            ret void
        }
    */
    auto main_function = Function::Create(
        FunctionType::get(Type::getInt32Ty(context), false),
        GlobalValue::LinkageTypes::ExternalLinkage,
        "main",
        *module
    );
    auto main_entry = BasicBlock::Create(context, "entry", main_function);
    // 设置插入点
    builder.SetInsertPoint(main_entry);
    // 调用sub函数
    auto sub_call1 = builder.CreateCall(sub_function, {builder.getInt32(2), builder.getInt32(5)});
    // 加载idx并在加载后加1
    auto idx_load = builder.CreateLoad(Type::getInt32Ty(context), idx);
    auto idx_add = builder.CreateAdd(idx_load, builder.getInt32(1));
    builder.CreateStore(idx_add, idx);
    // 获取res[idx]地址,使用自增前的idx
    auto res_gep = builder.CreateGEP(Type::getInt32Ty(context), (Value*) res, idx_load);
    // 存储返回值到res[idx]
    builder.CreateStore(sub_call1, res_gep);
    // 返回
    builder.CreateRetVoid();

    // 打印IR
    module->print(outs(), nullptr);
    return 0;
}