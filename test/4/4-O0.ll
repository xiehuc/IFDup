; ModuleID = '4-O0.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %a = alloca [100 x i32], align 16
  %i = alloca i32, align 4
  store i32 0, i32* %1
  %2 = bitcast [100 x i32]* %a to i8*
  call void @llvm.memset.p0i8.i64(i8* %2, i8 0, i64 400, i32 16, i1 false)
  store i32 0, i32* %i, align 4
  br label %3

; <label>:3                                       ; preds = %15, %0
  %4 = load volatile i32* %i
  %5 = icmp slt i32 %4, 100
  br i1 %5, label %6, label %18

; <label>:6                                       ; preds = %3
  %7 = load  i32* %i,  align 4
  %8 = sext i32 %7 to i64
  %9 = getelementptr inbounds [100 x i32]* %a, i32 0, i64 %8
  %10 = load i32* %9, align 4
  %11 = add nsw i32 %10, 1
  %12 = load volatile i32* %i, align 4
  %13 = sext i32 %12 to i64
  %14 = getelementptr inbounds [100 x i32]* %a, i32 0, i64 %13
  store i32 %11, i32* %14, align 4
  br label %15

; <label>:15                                      ; preds = %6
  %16 = load i32* %i, align 4
  %17 = add nsw i32 %16, 1
  store i32 %17, i32* %i, align 4
  br label %3

; <label>:18                                      ; preds = %3
  ret i32 0
}

; Function Attrs: nounwind
declare void @llvm.memset.p0i8.i64(i8* nocapture, i8, i64, i32, i1) #1

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"Ubuntu clang version 3.4-1ubuntu3 (tags/RELEASE_34/final) (based on LLVM 3.4)"}
