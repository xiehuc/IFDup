; ModuleID = '6-O0-Ins.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@main.a = private unnamed_addr constant [3 x [3 x i32]] [[3 x i32] [i32 1, i32 2, i32 3], [3 x i32] [i32 4, i32 5, i32 6], [3 x i32] [i32 7, i32 8, i32 9]], align 16
@main.b = private unnamed_addr constant [3 x [3 x i32]] [[3 x i32] [i32 2, i32 3, i32 4], [3 x i32] [i32 5, i32 6, i32 7], [3 x i32] [i32 9, i32 1, i32 8]], align 16

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %a = alloca [3 x [3 x i32]], align 16
  %b = alloca [3 x [3 x i32]], align 16
  %c = alloca [3 x [3 x i32]], align 16
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  %k = alloca i32, align 4
  store i32 0, i32* %1
  %2 = bitcast [3 x [3 x i32]]* %a to i8*
  %3 = bitcast [3 x [3 x i32]]* %a to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %3, i8* bitcast ([3 x [3 x i32]]* @main.a to i8*), i64 36, i32 16, i1 false)
  %4 = bitcast [3 x [3 x i32]]* %b to i8*
  %5 = bitcast [3 x [3 x i32]]* %b to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %5, i8* bitcast ([3 x [3 x i32]]* @main.b to i8*), i64 36, i32 16, i1 false)
  %6 = bitcast [3 x [3 x i32]]* %c to i8*
  %7 = bitcast [3 x [3 x i32]]* %c to i8*
  call void @llvm.memset.p0i8.i64(i8* %7, i8 0, i64 36, i32 16, i1 false)
  store i32 0, i32* %i, align 4
  br label %8

; <label>:8                                       ; preds = %A3, %0
  %9 = load i32* %i, align 4
  %10 = icmp slt i32 %9, 3
  br i1 %10, label %_T_, label %_F_

_T_:                                              ; preds = %8
  %11 = icmp slt i32 %9, 3
  br i1 %11, label %12, label %main_Error

; <label>:12                                      ; preds = %_T_
  store i32 0, i32* %j, align 4
  br label %13

; <label>:13                                      ; preds = %A7, %12
  %14 = load i32* %j, align 4
  %15 = icmp slt i32 %14, 3
  br i1 %15, label %_T_1, label %_F_2

_T_1:                                             ; preds = %13
  %16 = icmp slt i32 %14, 3
  br i1 %16, label %17, label %main_Error

; <label>:17                                      ; preds = %_T_1
  store i32 0, i32* %k, align 4
  br label %18

; <label>:18                                      ; preds = %A13, %17
  %19 = load i32* %k, align 4
  %20 = icmp slt i32 %19, 3
  br i1 %20, label %_T_4, label %_F_5

_T_4:                                             ; preds = %18
  %21 = icmp slt i32 %19, 3
  br i1 %21, label %22, label %main_Error

; <label>:22                                      ; preds = %_T_4
  %23 = load i32* %k, align 4
  %24 = sext i32 %23 to i64
  %25 = sext i32 %23 to i64
  %26 = load i32* %i, align 4
  %27 = sext i32 %26 to i64
  %28 = sext i32 %26 to i64
  %29 = getelementptr inbounds [3 x [3 x i32]]* %a, i32 0, i64 %27
  %30 = getelementptr inbounds [3 x [3 x i32]]* %a, i32 0, i64 %28
  %31 = getelementptr inbounds [3 x i32]* %29, i32 0, i64 %24
  %32 = getelementptr inbounds [3 x i32]* %30, i32 0, i64 %25
  %lA = icmp eq i32* %32, %31
  br i1 %lA, label %lA8, label %main_Error

lA8:                                              ; preds = %22
  %33 = load i32* %32, align 4
  %34 = load i32* %j, align 4
  %35 = sext i32 %34 to i64
  %36 = sext i32 %34 to i64
  %37 = load i32* %k, align 4
  %38 = sext i32 %37 to i64
  %39 = sext i32 %37 to i64
  %40 = getelementptr inbounds [3 x [3 x i32]]* %b, i32 0, i64 %38
  %41 = getelementptr inbounds [3 x [3 x i32]]* %b, i32 0, i64 %39
  %42 = getelementptr inbounds [3 x i32]* %40, i32 0, i64 %35
  %43 = getelementptr inbounds [3 x i32]* %41, i32 0, i64 %36
  %lA9 = icmp eq i32* %43, %42
  br i1 %lA9, label %lA8lA, label %main_Error

lA8lA:                                            ; preds = %lA8
  %44 = load i32* %43, align 4
  %45 = mul nsw i32 %33, %44
  %46 = mul nsw i32 %33, %44
  %47 = load i32* %j, align 4
  %48 = sext i32 %47 to i64
  %49 = sext i32 %47 to i64
  %50 = load i32* %i, align 4
  %51 = sext i32 %50 to i64
  %52 = sext i32 %50 to i64
  %53 = getelementptr inbounds [3 x [3 x i32]]* %c, i32 0, i64 %51
  %54 = getelementptr inbounds [3 x [3 x i32]]* %c, i32 0, i64 %52
  %55 = getelementptr inbounds [3 x i32]* %53, i32 0, i64 %48
  %56 = getelementptr inbounds [3 x i32]* %54, i32 0, i64 %49
  %lA10 = icmp eq i32* %56, %55
  br i1 %lA10, label %lA8lAlA, label %main_Error

lA8lAlA:                                          ; preds = %lA8lA
  %57 = load i32* %56, align 4
  %58 = add nsw i32 %57, %45
  %59 = add nsw i32 %57, %46
  %A11 = icmp eq i32 %59, %58
  br i1 %A11, label %lA8lAlAA, label %main_Error

lA8lAlAA:                                         ; preds = %lA8lAlA
  store i32 %59, i32* %56, align 4
  br label %60

; <label>:60                                      ; preds = %lA8lAlAA
  %61 = load i32* %k, align 4
  %62 = add nsw i32 %61, 1
  %63 = add nsw i32 %61, 1
  %A12 = icmp eq i32 %63, %62
  br i1 %A12, label %A13, label %main_Error

A13:                                              ; preds = %60
  store i32 %63, i32* %k, align 4
  br label %18

_F_5:                                             ; preds = %18
  %64 = icmp slt i32 %19, 3
  br i1 %64, label %main_Error, label %65

; <label>:65                                      ; preds = %_F_5
  br label %66

; <label>:66                                      ; preds = %65
  %67 = load i32* %j, align 4
  %68 = add nsw i32 %67, 1
  %69 = add nsw i32 %67, 1
  %A6 = icmp eq i32 %69, %68
  br i1 %A6, label %A7, label %main_Error

A7:                                               ; preds = %66
  store i32 %69, i32* %j, align 4
  br label %13

_F_2:                                             ; preds = %13
  %70 = icmp slt i32 %14, 3
  br i1 %70, label %main_Error, label %71

; <label>:71                                      ; preds = %_F_2
  br label %72

; <label>:72                                      ; preds = %71
  %73 = load i32* %i, align 4
  %74 = add nsw i32 %73, 1
  %75 = add nsw i32 %73, 1
  %A = icmp eq i32 %75, %74
  br i1 %A, label %A3, label %main_Error

A3:                                               ; preds = %72
  store i32 %75, i32* %i, align 4
  br label %8

_F_:                                              ; preds = %8
  %76 = icmp slt i32 %9, 3
  br i1 %76, label %main_Error, label %77

; <label>:77                                      ; preds = %_F_
  ret i32 0

main_Error:                                       ; preds = %_F_, %72, %_F_2, %66, %_F_5, %60, %lA8lAlA, %lA8lA, %lA8, %22, %_T_4, %_T_1, %_T_
  call void @exit(i32 -23)
  unreachable
}

; Function Attrs: nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture, i8* nocapture readonly, i64, i32, i1) #1

; Function Attrs: nounwind
declare void @llvm.memset.p0i8.i64(i8* nocapture, i8, i64, i32, i1) #1

declare void @exit(i32)

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"Ubuntu clang version 3.4-1ubuntu3 (tags/RELEASE_34/final) (based on LLVM 3.4)"}
