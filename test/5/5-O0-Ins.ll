; ModuleID = '5-O0-Ins.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@main.a = private unnamed_addr constant [10 x i32] [i32 3, i32 2, i32 4, i32 6, i32 8, i32 1, i32 5, i32 7, i32 9, i32 0], align 16

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %a = alloca [10 x i32], align 16
  %x = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 0, i32* %1
  %2 = bitcast [10 x i32]* %a to i8*
  %3 = bitcast [10 x i32]* %a to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %3, i8* bitcast ([10 x i32]* @main.a to i8*), i64 40, i32 16, i1 false)
  store i32 5, i32* %x, align 4
  store i32 0, i32* %i, align 4
  br label %4

; <label>:4                                       ; preds = %A3, %0
  %5 = load i32* %i, align 4
  %6 = icmp slt i32 %5, 10
  br i1 %6, label %_T_, label %_F_

_T_:                                              ; preds = %4
  %7 = icmp slt i32 %5, 10
  br i1 %7, label %8, label %main_Error

; <label>:8                                       ; preds = %_T_
  %9 = load i32* %i, align 4
  %10 = sext i32 %9 to i64
  %11 = sext i32 %9 to i64
  %12 = getelementptr inbounds [10 x i32]* %a, i32 0, i64 %10
  %13 = getelementptr inbounds [10 x i32]* %a, i32 0, i64 %11
  %14 = load i32* %13, align 4
  %15 = load i32* %x, align 4
  %16 = icmp eq i32 %14, %15
  br i1 %16, label %_T_1, label %_F_2

_T_1:                                             ; preds = %8
  %17 = icmp eq i32 %14, %15
  br i1 %17, label %18, label %main_Error

; <label>:18                                      ; preds = %_T_1
  br label %26

_F_2:                                             ; preds = %8
  %19 = icmp eq i32 %14, %15
  br i1 %19, label %main_Error, label %20

; <label>:20                                      ; preds = %_F_2
  br label %21

; <label>:21                                      ; preds = %20
  %22 = load i32* %i, align 4
  %23 = add nsw i32 %22, 1
  %24 = add nsw i32 %22, 1
  %A = icmp eq i32 %24, %23
  br i1 %A, label %A3, label %main_Error

A3:                                               ; preds = %21
  store i32 %24, i32* %i, align 4
  br label %4

_F_:                                              ; preds = %4
  %25 = icmp slt i32 %5, 10
  br i1 %25, label %main_Error, label %26

; <label>:26                                      ; preds = %_F_, %18
  ret i32 0

main_Error:                                       ; preds = %_F_, %21, %_F_2, %_T_1, %_T_
  call void @exit(i32 -23)
  unreachable
}

; Function Attrs: nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture, i8* nocapture readonly, i64, i32, i1) #1

declare void @exit(i32)

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"Ubuntu clang version 3.4-1ubuntu3 (tags/RELEASE_34/final) (based on LLVM 3.4)"}
