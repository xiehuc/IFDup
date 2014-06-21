; ModuleID = '4-O0-Ins.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %a = alloca [100 x i32], align 16
  %i = alloca i32, align 4
  store i32 0, i32* %1
  %2 = bitcast [100 x i32]* %a to i8*
  %3 = bitcast [100 x i32]* %a to i8*
  call void @llvm.memset.p0i8.i64(i8* %3, i8 0, i64 400, i32 16, i1 false)
  store i32 0, i32* %i, align 4
  br label %4

; <label>:4                                       ; preds = %A4, %0
  %5 = load i32* %i, align 4
  %6 = icmp slt i32 %5, 100
  br i1 %6, label %_T_, label %_F_

_T_:                                              ; preds = %4
  %7 = icmp slt i32 %5, 100
  br i1 %7, label %8, label %main_Error

; <label>:8                                       ; preds = %_T_
  %9 = load i32* %i, align 4
  %10 = sext i32 %9 to i64
  %11 = sext i32 %9 to i64
  %12 = getelementptr inbounds [100 x i32]* %a, i32 0, i64 %10
  %13 = getelementptr inbounds [100 x i32]* %a, i32 0, i64 %11
  %14 = load i32* %13, align 4
  %15 = add nsw i32 %14, 1
  %16 = add nsw i32 %14, 1
  %17 = load i32* %i, align 4
  %18 = sext i32 %17 to i64
  %19 = sext i32 %17 to i64
  %20 = getelementptr inbounds [100 x i32]* %a, i32 0, i64 %18
  %21 = getelementptr inbounds [100 x i32]* %a, i32 0, i64 %19
  %A = icmp eq i32 %16, %15
  br i1 %A, label %A1, label %main_Error

A1:                                               ; preds = %8
  %A2 = icmp eq i32* %21, %20
  br i1 %A2, label %A1A, label %main_Error

A1A:                                              ; preds = %A1
  store i32 %16, i32* %21, align 4
  br label %22

; <label>:22                                      ; preds = %A1A
  %23 = load i32* %i, align 4
  %24 = add nsw i32 %23, 1
  %25 = add nsw i32 %23, 1
  %A3 = icmp eq i32 %25, %24
  br i1 %A3, label %A4, label %main_Error

A4:                                               ; preds = %22
  store i32 %25, i32* %i, align 4
  br label %4

_F_:                                              ; preds = %4
  %26 = icmp slt i32 %5, 100
  br i1 %26, label %main_Error, label %27

; <label>:27                                      ; preds = %_F_
  ret i32 0

main_Error:                                       ; preds = %_F_, %22, %A1, %8, %_T_
  call void @exit(i32 -23)
  unreachable
}

; Function Attrs: nounwind
declare void @llvm.memset.p0i8.i64(i8* nocapture, i8, i64, i32, i1) #1

declare void @exit(i32)

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"Ubuntu clang version 3.4-1ubuntu3 (tags/RELEASE_34/final) (based on LLVM 3.4)"}
