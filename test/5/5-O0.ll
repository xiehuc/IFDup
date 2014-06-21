; ModuleID = '5-O0.bc'
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
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %2, i8* bitcast ([10 x i32]* @main.a to i8*), i64 40, i32 16, i1 false)
  store i32 5, i32* %x, align 4
  store i32 0, i32* %i, align 4
  br label %3

; <label>:3                                       ; preds = %15, %0
  %4 = load i32* %i, align 4
  %5 = icmp slt i32 %4, 10
  br i1 %5, label %6, label %18

; <label>:6                                       ; preds = %3
  %7 = load i32* %i, align 4
  %8 = sext i32 %7 to i64
  %9 = getelementptr inbounds [10 x i32]* %a, i32 0, i64 %8
  %10 = load i32* %9, align 4
  %11 = load i32* %x, align 4
  %12 = icmp eq i32 %10, %11
  br i1 %12, label %13, label %14

; <label>:13                                      ; preds = %6
  br label %18

; <label>:14                                      ; preds = %6
  br label %15

; <label>:15                                      ; preds = %14
  %16 = load i32* %i, align 4
  %17 = add nsw i32 %16, 1
  store i32 %17, i32* %i, align 4
  br label %3

; <label>:18                                      ; preds = %13, %3
  ret i32 0
}

; Function Attrs: nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture, i8* nocapture readonly, i64, i32, i1) #1

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"Ubuntu clang version 3.4-1ubuntu3 (tags/RELEASE_34/final) (based on LLVM 3.4)"}
