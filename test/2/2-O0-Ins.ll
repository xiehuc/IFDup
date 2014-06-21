; ModuleID = '2-O0-Ins.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %x = alloca i32, align 4
  %y = alloca i32, align 4
  store i32 0, i32* %1
  store i32 5, i32* %x, align 4
  store i32 7, i32* %y, align 4
  %2 = load i32* %x, align 4
  %3 = icmp eq i32 %2, 5
  br i1 %3, label %_T_, label %_F_

_T_:                                              ; preds = %0
  %4 = icmp eq i32 %2, 5
  br i1 %4, label %5, label %main_Error

; <label>:5                                       ; preds = %_T_
  %6 = load i32* %y, align 4
  %7 = add nsw i32 %6, 1
  %8 = add nsw i32 %6, 1
  %A = icmp eq i32 %8, %7
  br i1 %A, label %A1, label %main_Error

A1:                                               ; preds = %5
  store i32 %8, i32* %y, align 4
  br label %10

_F_:                                              ; preds = %0
  %9 = icmp eq i32 %2, 5
  br i1 %9, label %main_Error, label %10

; <label>:10                                      ; preds = %_F_, %A1
  %11 = load i32* %y, align 4
  %12 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([3 x i8]* @.str, i32 0, i32 0), i32 %11)
  ret i32 0

main_Error:                                       ; preds = %_F_, %5, %_T_
  call void @exit(i32 -23)
  unreachable
}

declare i32 @printf(i8*, ...) #1

declare void @exit(i32)

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"Ubuntu clang version 3.4-1ubuntu3 (tags/RELEASE_34/final) (based on LLVM 3.4)"}
